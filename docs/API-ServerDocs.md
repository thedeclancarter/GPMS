# API/Server Documentation
## Overall Implementation
The **Stable Diffusion Image Generation API** allows you to generate and refine images based on textual prompts and input images using Stable Diffusion and ControlNet.

## File Structure
```
├── api-server/                            # Server-side API handling for generating images
│   ├── local_packages/                    # Directory for locally hosted packages (e.g., .whl files)
│   ├── input_images/                      # Directory for input images
│   ├── output_images/                     # Directory for generated output images
│   │   ├── YYYYMMDD_HHMMSS.png            # Sample output image file
│   ├── ssl_cert/                          # Directory for SSL certificates
│   │   ├── cert.pem                       # Public SSL certificate file
│   │   └── key.pem                        # Private SSL key file
│   ├── stable_diffusion/                  # Core module for Stable Diffusion-related scripts
│   │   ├── __init__.py                    # Initialization script for the stable_diffusion module
│   │   ├── gif_creator.py                 # Script for generating GIFs from images
│   │   ├── image_processing.py            # Image processing functions
│   │   ├── pipeline_initialization.py     # Pipeline setup for the Stable Diffusion model
│   │   └── pipeline_service.py            # Service script for managing the generation pipeline
│   ├── venv/                              # Virtual environment directory
│   ├── api.log                            # Log file for API requests and responses
│   ├── app.py                             # Main application file for the API server
│   └── requirements.txt                   # Python dependencies
```

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

| Parameter         | Type   | Required | Description                                   |
|-------------------|--------|----------|-----------------------------------------------|
| `image`           | File   | Yes      | Input image file (formats: png, jpg, jpeg, gif). |
| `prompt`          | String | Yes      | Textual description for image generation.     |
| `negative_prompt` | String | No       | Description to exclude certain features.      |
| `sensitivity`     | Float  | No       | ControlNet conditioning scale (default: 1.0). |

## Response

- **Success (200):** Returns the generated image as a downloadable file.
- **Error:** Returns a JSON object with an `error` message and appropriate HTTP status code.

## Usage Examples

### Using `curl`

```bash
curl -X POST http://127.0.0.1:5000/generate \
  -H "x-api-key: your-secure-api-key" \
  -F image=@/path/to/your/image.jpg \
  -F prompt="A cozy Christmas cottage with festive decorations." \
  -F negative_prompt="low resolution, blurry" \
  -F sensitivity=0.7 \
  --output generated_image.png