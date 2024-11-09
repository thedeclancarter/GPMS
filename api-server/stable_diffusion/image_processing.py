import cv2
import numpy as np
from PIL import Image

def HWC3(x):
    if x.ndim == 2:
        x = x[:, :, None]
    assert x.ndim == 3
    H, W, C = x.shape
    if C == 3:
        return x
    if C == 1:
        return np.concatenate([x, x, x], axis=2)
    if C == 4:
        color = x[:, :, 0:3].astype(np.float32)
        alpha = x[:, :, 3:4].astype(np.float32) / 255.0
        y = color * alpha + 255.0 * (1.0 - alpha)
        return y.clip(0, 255).astype(np.uint8)

def process_controlnet_image(image_path):
    controlnet_img = cv2.imread(image_path)
    height, width, _ = controlnet_img.shape
    ratio = np.sqrt(1024 * 1024 / (width * height))
    new_width, new_height = int(width * ratio), int(height * ratio)
    controlnet_img = cv2.resize(controlnet_img, (new_width, new_height))

    gray = cv2.cvtColor(controlnet_img, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    edges = cv2.Canny(blurred, 100, 200)
    edges = cv2.GaussianBlur(edges, (5, 5), 0)
    controlnet_img = HWC3(edges)

    return Image.fromarray(controlnet_img).convert("RGB"), new_width, new_height
