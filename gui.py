import gradio as gr
from request import req_post
from convert_epd import writebin, writecpph
import shutil
from PIL import Image

def save_image(path):
    shutil.copy(path, "photo.jpg")
    return "Saved as photo.jpg"

webui = gr.Interface(
    fn=save_image,
    inputs=gr.Image(type="filepath", label="Upload"),
    outputs=gr.Textbox(label="Status"),
)

webui.launch()