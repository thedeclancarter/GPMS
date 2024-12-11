# API/Server Documentation
## Overall Implementation
The **Stable Diffusion Image Generation API** allows you to generate and refine images based on textual prompts and input images using Stable Diffusion and ControlNet.

## File Structure
```
api-server/
├── stable_diffusion/
│   ├── __init__.py
│   ├── gif_creator.py
│   ├── image_processing.py
│   ├── pipeline_initialization.py
│   ├── pipeline_service.py
│   ├── app.py
│   └── requirements.txt
├── input_images/         # Stores temporarily uploaded images before processing
├── output_images/        # Stores generated images after processing
│   └── YYYYMMDD_HHMMSS_UUID.png
├── ssl_cert/             # Directory to hold SSL certificates
│   ├── cert.pem
│   └── key.pem
├── api.log               # Log file for API activities
└── .env                  # Environment variables (e.g., API_KEY, SERVER_IP, SERVER_PORT)
```

## File Breakdown
### app.py

The `App.py` module hosts a Flask-based REST API that provides endpoints to generate refined images based on user-provided input images and text prompts. Key functionalities include:

- **Flask Server Initialization:**
  - Launches a Flask application to accept incoming requests.
  - Optionally uses SSL certificates if available for encrypted communication.

- **API Key Authentication:**
  - Ensures that requests include a valid `x-api-key` header.
  - Rejects unauthorized requests with appropriate HTTP status codes.

- **File Handling and Validation:**
  - Accepts uploaded images (PNG, JPG, JPEG, GIF) up to a certain size limit.
  - Validates and saves images to `input_images/` directory.
  - Resizes images to a fixed resolution (640x360) for consistency.

- **Integration with Stable Diffusion Pipeline:**
  - Loads and initializes the Stable Diffusion XL pipelines (controlled by ControlNet) at startup.
  - Offloads image generation to `generate_image()` in `pipeline_service.py`.

- **Logging and Error Handling:**
  - Configured logging for both file and console output.
  - Returns JSON error messages and appropriate HTTP status codes on failures.

- **Secure Threaded Operations:**
  - Uses a threading lock to ensure that image generation tasks are not run concurrently, preventing resource conflicts.

- **Configurable Environment:**
  - Reads configuration such as `API_KEY`, `SERVER_IP`, and `SERVER_PORT` from `.env` files.
  - Adjusts runtime parameters based on environment variables.

---

### image_processing.py

This module focuses on preparing and transforming images before passing them into the Stable Diffusion pipeline. Key functionalities include:

- **Format Normalization:**
  - Ensures image arrays are in proper HWC3 (Height x Width x 3-channel) format.

- **ControlNet Image Preparation:**
  - Reads input images via OpenCV.
  - Resizes images based on a calculated ratio to fit a target control space.
  - Converts images to grayscale and applies Gaussian blur to avoid excess noise in generation.
  - Uses Canny edge detection to produce edge maps that guide the ControlNet model.

- **Final Image Resizing:**
  - Resizes the final generated image to a standard output resolution (e.g., 1280x720) for consistency across results.

---

### pipeline_initialization.py

This module sets up and initializes the stable diffusion pipeline and associated components. It includes:

- **Model Loading:**
  - Loads the base Stable Diffusion XL model (e.g., "RunDiffusion/Juggernaut-XL-v8").
  - Integrates ControlNet models (e.g., "xinsir/controlnet-canny-sdxl-1.0") to guide the generation process using edge maps.
  - Incorporates a VAE for enhanced image fidelity ("madebyollin/sdxl-vae-fp16-fix").

- **Scheduler Configuration:**
  - Sets up `DPMSolverMultistepScheduler` for efficient inference steps.

- **Refiner Pipeline:**
  - Loads the `StableDiffusionXLImg2ImgPipeline` for refining initial generated images to achieve higher quality and detail.

- **Device Handling:**
  - Automatically selects GPU if available, otherwise falls back to CPU.

- **Return Handles:**
  - Provides references (`pipe`, `refiner`) that can be used in other modules to perform generation tasks.

---

### pipeline_service.py

The `pipeline_service.py` module provides a higher-level API to generate and refine images using the loaded Stable Diffusion pipelines. It orchestrates the process and interfaces with the image processing functions. Key functionalities include:

- **Lazy Initialization:**
  - Uses a global lock and checks initialization state to ensure the pipelines are loaded only once.

- **Style-Specific Prompts:**
  - Applies different prompt templates and negative prompts based on the specified style (e.g., "animated" or "realistic").
  - Enhances creative control by merging user prompts with style-specific instructions.

- **Image Generation and Refinement:**
  - Processes the control image from `image_processing.py` to generate an initial result.
  - Passes the generated image to a refiner pipeline for further improvements in quality and detail.

- **ControlNet Conditioning:**
  - Adjusts parameters like `controlnet_conditioning_scale`, `guidance_scale`, and `control_guidance_end` to fine-tune how strongly ControlNet influences the generation.

- **Resource Management:**
  - Clears cache after generation steps to optimize memory usage.

- **Optional GIF Creation:**
  - Offers a `create_fading_gif()` function to transition from the original image to the refined result, visually demonstrating the transformation process.

---


## Installation Note:
- In `.env` in the root of your repo add:
    - `API_KEY=XXXXXXXXXXXXXXX`
    - `SERVER_IP=0.0.0.0`
    - `SERVER_PORT=5000`
- Place the following installs in a dir named `local_packages` in `api-server`
    - `pytorch-triton-rocm`
    - `torch`
    - `torchvision`
- Create a dir named `ssl_cert` in `api-server`
    - Run the following command: `openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes`
- Initialize virtual enviorment from `requirements.txt`
- To start the service run `python app.py`

## Authentication

All requests must include an API key in the headers.

- **Header:** `x-api-key`
- **Example:** `x-api-key: your-secure-api-key`

*Ensure your API key is kept confidential and not shared publicly.*

## Endpoint
### Generate Image

- **URL:** `/generate`
- **Method:** `POST`
- **Description:** Generates and refines an image based on the provided prompt and input image.

## Request Parameters

| Parameter       | Type    | Required | Description                                             |
|-----------------|---------|----------|---------------------------------------------------------|
| `image`         | File    | Yes      | Input image file (formats: png, jpg, jpeg, gif).        |
| `prompt`        | String  | Yes      | Textual description for image generation.               |
| `style`         | String  | No       | Style mode to apply (default: "animated").              |
| `lo_threshold`  | Integer | No       | Lower threshold for image processing (default: 100).    |
| `hi_threshold`  | Integer | No       | Higher threshold for image processing (default: 200).   |

## Response

- **Success (200):** Returns the generated image as a downloadable file.
- **Error:** Returns a JSON object with an `error` message and the appropriate HTTP status code.

## Usage Examples

### Using `curl`

```bash
curl -X POST http://127.0.0.1:5000/generate \
  -H "x-api-key: your-secure-api-key" \
  -F image=@/path/to/your/image.jpg \
  -F prompt="A cozy Christmas cottage with festive decorations." \
  -F style="animated" \
  -F lo_threshold=100 \
  -F hi_threshold=200 \
  --output generated_image.png
```