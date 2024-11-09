from PIL import Image
from datetime import datetime
import os

def blend_images(img1, img2, alpha):
    """Blend two images together with a given alpha."""
    return Image.blend(img1, img2, alpha)

def create_fade_gif(img1, img2, output_gif_path, duration=100):
    # Open the input images
    img1 = img1.convert("RGBA")
    img2 = img2.convert("RGBA")

    # Check if the sizes are different, and resize to the smaller dimensions
    if img1.size != img2.size:
        img1 = img1.resize((min(img1.size[0], img2.size[0]), min(img1.size[1], img2.size[1])))
        img2 = img2.resize((min(img1.size[0], img2.size[0]), min(img1.size[1], img2.size[1])))

    frames = []

    # Fade from image 1 to image 2
    for i in range(20):  # 20 steps for a 2-second crossfade
        alpha = i / 20.0
        frame = blend_images(img1, img2, alpha)
        frames.append(frame)

    # Hold image 2 for 2 seconds (20 frames at 100ms each)
    for _ in range(20):
        frames.append(img2)

    # Fade from image 2 back to image 1
    for i in range(20):
        alpha = (20 - i) / 20.0
        frame = blend_images(img1, img2, alpha)
        frames.append(frame)

    # Hold image 1 for 2 seconds (20 frames at 100ms each)
    for _ in range(20):
        frames.append(img1)

    # Save as a GIF
    frames[0].save(output_gif_path, save_all=True, append_images=frames[1:], duration=duration, loop=0, dither=Image.NONE)
    print(f"GIF saved @ {output_gif_path}")

def save_gif(img1_path, img2, output_dir="animated"):
    timestamp = datetime.now().strftime("%H_%M_%S")
    os.makedirs(output_dir, exist_ok=True)
    output_gif_path = f"{output_dir}/{timestamp}.gif"
    img1 = Image.open(img1_path).convert("RGBA")

    create_fade_gif(img1, img2.convert("RGBA"), output_gif_path)
