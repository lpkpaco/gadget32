import gradio as gr
from request import upload, clear
from convert_epd import writebin, writecpph
from gen_cal import add_event, draw_calendar
import shutil
from PIL import Image

def convert_both():
    writebin()
    writecpph()
    return "Converted to C++ array and binary file"

cal_dict = {}
def save_image(path):
    shutil.copy(path, "photo.jpg")
    return "Saved as photo.jpg"

def add_event_gui(event, time1, time2):
    success, status = add_event(cal_dict, event, time1, time2, False)
    return status

def draw_calendar_gui():
    return draw_calendar(cal_dict)

def clear_calendar_gui():
    cal_dict.clear()
    return "Calendar cleared"
    
with gr.Blocks() as webui:
    with gr.Row():
        with gr.Column():
            gr.Markdown("## Calendar Generator")
            event = gr.Textbox(label="Event Name")
            time1 = gr.Textbox(label="Start Time (HHMM in 24-hour format)")
            time2 = gr.Textbox(label="End Time (HHMM in 24-hour format)")
            add_button = gr.Button("Add Event")
            generate_button = gr.Button("Generate Calendar")
            clear_button = gr.Button("Clear Calendar")
            draw_upload_button = gr.Button("Draw and Upload Calendar to Display")
        with gr.Column():
            image_output = gr.Image(label="Calendar Generated", type="pil")
        add_button.click(
            fn = add_event_gui,
            inputs = [event, time1, time2],
            outputs = [gr.Textbox(label="Status")],
        )
        generate_button.click(
            fn = draw_calendar_gui,
            inputs = [],
            outputs = [image_output],
        )
        clear_button.click(
            fn = clear_calendar_gui,
            inputs = None,
            outputs = [gr.Textbox(label="Status")],
        )
        draw_upload_button.click(
            fn = lambda: [draw_calendar(cal_dict).save("frame.bin"), upload()],
            inputs = None,
            outputs = [gr.Textbox(label="Upload Status")],
        )
    with gr.Row():
        with gr.Column():
            gr.Markdown("## Upload Image")
            image_input = gr.Image(label="Upload Image", type="filepath")
            save_button = gr.Button("Upload Image")
            convert_button = gr.Button("Convert to C++ array + binary")
            upload_button = gr.Button("Upload to Display")
            convert_button.click(
                fn = convert_both,
                inputs = [],
                outputs = [gr.Textbox(label="Conversion Status")],
            )
            save_button.click(
                fn = save_image,
                inputs = [image_input],
                outputs = [gr.Textbox(label="Upload Status")],
            )
            upload_button.click(
                fn = upload,
                inputs = None,
                outputs = [gr.Textbox(label="Upload Status")],
            )
webui.launch()