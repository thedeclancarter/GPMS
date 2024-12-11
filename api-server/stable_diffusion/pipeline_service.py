# stable_diffusion/pipeline_service.py

import torch
from .image_processing import process_controlnet_image, resize_final_image
from .pipeline_initialization import initialize_pipelines
from .gif_creator import save_gif
from PIL import Image
from datetime import datetime
import os
import threading

# Use threading.Lock to ensure thread-safe initialization
_init_lock = threading.Lock()
_pipe = None
_refiner = None

# Define prompt templates for styles
STYLE_PROMPTS = {
    "animated": (
        "surreal painting representing strange vision of {prompt}. harmonious madness, synergy with chance. "
        "unique artstyle, mindbending art, magical surrealism. best quality, high resolution"
    ),
    "realistic": "{prompt}"
}

# Define negative prompts for styles
STYLE_NEGATIVE_PROMPTS = {
    "animated": (
        "photography, illustration, drawing. realistic, possible. logical, sane. low quality, low resolution"
    ),
    "realistic": (
        "(worst quality, low quality, normal quality, lowres, low details, oversaturated, undersaturated, "
        "overexposed, underexposed, grayscale, bw, bad photo, bad photography, bad art:1.4), "
        "(watermark, signature, text font, username, error, logo, words, letters, digits, autograph, trademark, "
        "name:1.2), (blur, blurry, grainy), morbid, ugly, asymmetrical, mutated malformed, mutilated, poorly lit, "
        "bad shadow, draft, cropped, out of frame, cut off, censored, jpeg artifacts, out of focus, glitch, "
        "duplicate, (airbrushed, cartoon, anime, semi-realistic, cgi, render, blender, digital art, manga, "
        "amateur:1.3), (3D ,3D Game, 3D Game Scene, 3D Character:1.1), (bad hands, bad anatomy, bad body, bad face, "
        "bad teeth, bad arms, bad legs, deformities:1.3)"
    )
}

def load_pipelines(device):
    global _pipe, _refiner
    if _pipe is None and _refiner is None:
        with _init_lock:
            if _pipe is None and _refiner is None:
                _pipe, _refiner = initialize_pipelines(device)

def get_pipelines():
    global _pipe, _refiner
    return _pipe, _refiner

def generate_image(
    image_path: str,
    prompt: str,
    style: str = "",  # "animated" or "realistic"
    controlnet_conditioning_scale: float = 1.0,
    guidance_scale: float = 10.0,
    control_guidance_end: float = 0.8,
    num_inference_steps: int = 60,
    num_refiner_steps: int = 40,
    lo_threshold: int = 100,
    hi_threshold: int = 200
) -> Image.Image:
    """
    Generates and refines an image using the Stable Diffusion pipeline with style-specific prompts.
    """
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    load_pipelines(device)
    pipe, refiner = get_pipelines()

    if pipe is None or refiner is None:
        raise RuntimeError("Pipelines are not initialized.")

    # Retrieve style-specific prompt template and negative prompt
    style_prompt_template = STYLE_PROMPTS.get(style.lower(), "{prompt}")
    style_negative_prompt = STYLE_NEGATIVE_PROMPTS.get(style.lower(), "")

    # Embed the prompt into the style-specific template
    full_prompt = style_prompt_template.format(prompt=prompt)

    print(f"Full Prompt: {full_prompt}")

    with torch.no_grad():
        # Read and process the ControlNet image
        controlnet_img, new_width, new_height = process_controlnet_image(image_path, lo_threshold, hi_threshold)

        # Generate the base image
        base_images = pipe(
            prompt=[full_prompt],
            image=controlnet_img,
            controlnet_conditioning_scale=controlnet_conditioning_scale,
            width=new_width,
            height=new_height,
            guidance_scale=guidance_scale,
            control_guidance_end=control_guidance_end,
            num_inference_steps=num_inference_steps,
            negative_prompt=[style_negative_prompt] if style_negative_prompt else None,
        ).images

        # Refine the generated image
        refined_images = refiner(
            prompt=[full_prompt],
            image=base_images,
            num_inference_steps=num_refiner_steps,
            negative_prompt=[style_negative_prompt] if style_negative_prompt else None,
        ).images

        # Clear PyTorch cache
        torch.cuda.empty_cache()

        final_image = resize_final_image(refined_images[0], 1280, 720)

        # Return the refined image
        return final_image


def create_fading_gif(
    original_image_path: str,
    refined_image: Image.Image,
    output_gif_path: str
) -> None:
    """
    Creates a fading GIF between the original and refined images.

    Args:
        original_image_path (str): Path to the original input image.
        refined_image (Image.Image): The refined generated image.
        output_gif_path (str): Path to save the output GIF.
    """
    save_gif(original_image_path, refined_image, output_gif_path)
