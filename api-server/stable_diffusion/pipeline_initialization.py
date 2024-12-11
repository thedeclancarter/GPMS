import torch
from diffusers import ControlNetModel, StableDiffusionXLControlNetPipeline, StableDiffusionXLImg2ImgPipeline, AutoencoderKL, DPMSolverMultistepScheduler

def initialize_pipelines(device):
    # Initialize the scheduler
    scheduler = DPMSolverMultistepScheduler.from_pretrained(
        "stabilityai/stable-diffusion-xl-base-1.0", subfolder="scheduler"
    )

    # Initialize the ControlNet model
    controlnet = ControlNetModel.from_pretrained(
        "xinsir/controlnet-canny-sdxl-1.0",
        torch_dtype=torch.float16
    ).to(device)

    # Initialize the VAE
    vae = AutoencoderKL.from_pretrained(
        "madebyollin/sdxl-vae-fp16-fix", torch_dtype=torch.float16
    ).to(device)

    # Initialize the base pipeline with JuggernautXL_v8
    pipe = StableDiffusionXLControlNetPipeline.from_pretrained(
        "RunDiffusion/Juggernaut-XL-v8",
        controlnet=controlnet,
        vae=vae,
        torch_dtype=torch.float16,
        scheduler=scheduler,
    ).to(device)

    # Initialize the refiner pipeline
    refiner = StableDiffusionXLImg2ImgPipeline.from_pretrained(
        "stabilityai/stable-diffusion-xl-refiner-1.0",
        vae=vae,
        torch_dtype=torch.float16
    ).to(device)

    return pipe, refiner
