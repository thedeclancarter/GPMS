# stable_diffusion/pipeline_service.py

import torch
from .image_processing import process_controlnet_image
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
    negative_prompt: str = "",
    controlnet_conditioning_scale: float = 1.0,
    guidance_scale: float = 10.0,
    control_guidance_end: float = 0.8,
    num_inference_steps: int = 60,
    num_refiner_steps: int = 40
) -> Image.Image:
    """
    Generates and refines an image using the Stable Diffusion pipeline.

    Args:
        image_path (str): Path to the input image.
        prompt (str): The text prompt for image generation.
        negative_prompt (str, optional): Negative prompt to exclude unwanted features. Defaults to "".
        controlnet_conditioning_scale (float, optional): Scaling factor for ControlNet conditioning. Defaults to 1.0.
        guidance_scale (float, optional): Guidance scale for the pipeline. Defaults to 10.0.
        control_guidance_end (float, optional): ControlNet influence percentage. Defaults to 0.8.
        num_inference_steps (int, optional): Number of inference steps for the base image. Defaults to 60.
        num_refiner_steps (int, optional): Number of inference steps for refining. Defaults to 40.

    Returns:
        Image.Image: The refined generated image.
    """
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    load_pipelines(device)
    pipe, refiner = get_pipelines()

    if pipe is None or refiner is None:
        raise RuntimeError("Pipelines are not initialized.")

    with torch.no_grad():
        # Read and process the ControlNet image
        controlnet_img, new_width, new_height = process_controlnet_image(image_path)

        # Generate the base image
        base_images = pipe(
            prompt=[prompt],
            image=controlnet_img,
            controlnet_conditioning_scale=controlnet_conditioning_scale,
            width=new_width,
            height=new_height,
            guidance_scale=guidance_scale,
            control_guidance_end=control_guidance_end,
            num_inference_steps=num_inference_steps,
            negative_prompt=[negative_prompt],
        ).images

        # Refine the generated image
        refined_images = refiner(
            prompt=[prompt],
            image=base_images,
            num_inference_steps=num_refiner_steps,
            negative_prompt=[negative_prompt],
        ).images

        # Clear PyTorch cache
        torch.cuda.empty_cache()

        # Return the refined image
        return refined_images[0]

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
