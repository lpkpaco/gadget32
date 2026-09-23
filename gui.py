import gradio as gr
from request import upload, clear
from convert_epd import writebin, writecpph
from gen_cal import add_event, draw_calendar
import shutil

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
            event = gr.Textbox(label="Event Name")
            time1 = gr.Textbox(label="Start Time (HHMM in 24-hour format)")
            time2 = gr.Textbox(label="End Time (HHMM in 24-hour format)")
            add_button = gr.Button("Add Event")
            generate_button = gr.Button("Generate Calendar")
            clear_button = gr.Button("Clear Calendar")
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
webui.launch()