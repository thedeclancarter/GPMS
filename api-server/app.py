import sys
from pathlib import Path
from flask import Flask, request, jsonify, send_file
from werkzeug.utils import secure_filename
import os
import io
from PIL import Image
from stable_diffusion.pipeline_service import generate_image, load_pipelines
from datetime import datetime
import logging
from functools import wraps
from dotenv import load_dotenv
import torch
import threading  # Import threading
import uuid  # Import uuid for unique filenames

# Add project root to sys.path if necessary
project_root = Path(__file__).resolve().parent.parent
sys.path.append(str(project_root))

# Load environment variables from .env in project_root
env_path = project_root / '.env'
load_dotenv(dotenv_path=env_path)

# Fetch configuration from environment variables
API_KEY = os.getenv('API_KEY')
SERVER_IP = os.getenv('SERVER_IP', '0.0.0.0')  # Default to '0.0.0.0' if not set
SERVER_PORT = int(os.getenv('SERVER_PORT', 5000))  # Default to 5000 if not set

app = Flask(__name__)

# Configuration
INPUT_FOLDER = Path('input_images')
OUTPUT_FOLDER = Path('output_images')
CERTS_FOLDER = Path('ssl_cert')
ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg', 'gif'}
MAX_CONTENT_LENGTH = 16 * 1024 * 1024  # 16 MB limit

app.config['INPUT_FOLDER'] = INPUT_FOLDER
app.config['OUTPUT_FOLDER'] = OUTPUT_FOLDER
app.config['MAX_CONTENT_LENGTH'] = MAX_CONTENT_LENGTH

# Ensure the upload and output folders exist
INPUT_FOLDER.mkdir(parents=True, exist_ok=True)
OUTPUT_FOLDER.mkdir(parents=True, exist_ok=True)

# Initialize logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s %(levelname)s %(name)s %(threadName)s : %(message)s',
    handlers=[
        logging.FileHandler("api.log"),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def require_api_key(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        api_key = request.headers.get('x-api-key')
        if not api_key:
            logger.warning("API key missing in request.")
            return jsonify({'error': 'API key missing'}), 401
        if api_key != API_KEY:
            logger.warning(f"Invalid API key: {api_key}")
            return jsonify({'error': 'Invalid API key'}), 401
        return f(*args, **kwargs)
    return decorated

# Initialize pipelines upon application startup
def initialize_pipelines():
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    logger.info("Initializing pipelines...")
    load_pipelines(device)
    logger.info("Pipelines initialized successfully.")

# Manually push the application context and initialize pipelines
with app.app_context():
    initialize_pipelines()

# Create a global lock
generate_image_lock = threading.Lock()

@app.route('/generate', methods=['POST'])
@require_api_key  # Apply the API key requirement
def generate():
    # Check if the post request has the file part
    if 'image' not in request.files:
        logger.error("No image part in the request.")
        return jsonify({'error': 'No image part in the request'}), 400

    file = request.files['image']

    if file.filename == '':
        logger.error("No selected file in the request.")
        return jsonify({'error': 'No selected file'}), 400

    if file and allowed_file(file.filename):
        # Generate a unique filename using UUID
        file_ext = file.filename.rsplit('.', 1)[1].lower()
        unique_filename = f"{uuid.uuid4().hex}.{file_ext}"
        upload_path = INPUT_FOLDER / unique_filename
        file.save(upload_path)
        logger.info(f"Image saved to {upload_path}.")
    else:
        logger.error("Unsupported file type uploaded.")
        return jsonify({'error': 'Unsupported file type'}), 400

    # Get additional form data
    prompt = request.form.get('prompt')
    style = request.form.get('style', "animated")  # Default style
    lo_treshold = request.form.get('lo_treshold', 100)
    hi_treshold = request.form.get('hi_treshold', 200)

    if not prompt:
        logger.error("No prompt provided in the request.")
        return jsonify({'error': 'No prompt provided'}), 400

    try:
        # Acquire the lock before starting image generation
        with generate_image_lock:
            logger.info(f"Processing image generation for {upload_path} with prompt '{prompt}'.")
            # Generate the image using the pipeline
            generated_image = generate_image(
                image_path=str(upload_path),
                prompt=prompt,
                style=style,
                lo_treshold=lo_treshold,
                hi_treshold=hi_treshold
            )

        # Convert PIL Image to bytes
        img_byte_arr = io.BytesIO()
        generated_image.save(img_byte_arr, format='PNG')
        img_byte_arr.seek(0)

        # Optionally, save the generated image to the output folder
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
        output_image_filename = f"{timestamp}_{uuid.uuid4().hex}.png"
        output_image_path = OUTPUT_FOLDER / output_image_filename
        generated_image.save(output_image_path)
        logger.info(f"Generated image saved to {output_image_path}.")

        # Remove the uploaded file after processing to save space
        os.remove(upload_path)
        logger.info(f"Uploaded file {upload_path} removed after processing.")

        return send_file(
            img_byte_arr,
            mimetype='image/png',
            as_attachment=True,
            download_name='generated_image.png'
        )

    except Exception as e:
        logger.exception("Error during image generation.")
        # Ensure the uploaded file is removed even if an error occurs
        if os.path.exists(upload_path):
            os.remove(upload_path)
            logger.info(f"Uploaded file {upload_path} removed after error.")
        return jsonify({'error': str(e)}), 500

@app.errorhandler(413)
def request_entity_too_large(error):
    logger.error("Uploaded file is too large.")
    return jsonify({'error': 'File too large. Maximum size is 16MB.'}), 413

if __name__ == '__main__':
    # Path to your SSL certificates inside api-server/certs/
    ssl_cert = CERTS_FOLDER / 'cert.pem'
    ssl_key = CERTS_FOLDER / 'key.pem'

    if ssl_cert.exists() and ssl_key.exists():
        logger.info("Starting Flask app with HTTPS.")
        app.run(
            host=SERVER_IP,
            port=SERVER_PORT,
            debug=True,  # Enable debug mode
            use_reloader=False,  # Disable the auto-reloader
            ssl_context=(str(ssl_cert), str(ssl_key))
        )
    else:
        logger.warning("SSL certificates not found. Starting Flask app without HTTPS.")
        app.run(
            host=SERVER_IP,
            port=SERVER_PORT,
            debug=True,
            use_reloader=False  # Disable the auto-reloader
        )
