# gadget32
Source codes for ESP32-S3 powered Wi-Fi image rendering and decoding for the WaveShare 1.54" 240 x 240 E6 color space e-paper display. Beside computer-reliant Python-based controlling and image drawing, a C++ image decoding script is developed for ESP32 to decode image data into C-Style array and properly display image on the e-paper display. 

This project also includes a small Python/Gradio utility for preparing images and rendering a simple calendar.

## Description

The firmware targets the Waveshare 1.54-inch e-Paper E display, using the panel's six supported colors:
- black
- white
- yellow
- red
- blue
- green

Images are fitted into a 240 x 240 canvas, converted to the six-color palette, and packed as two 4-bit pixels per byte. A complete frame is 28,800 bytes.

There are two pipelines to send an image:

- **Decode via Python:** using Gradio frontend GUI with Python backend to convert an image or calendar to `frame.bin`, then upload the already-packed frame to the display to save computation power on the ESP32 and bypass certain ESP32 memory limits **(especially issues related to PSRAM)**.

- **Decode via C++:** using ESP32 as an AP to host a control panel and send a JPEG directly through JavaScript and HTTP requests. The ESP32 performs JPEG decoding, resizing, palette conversion, and dithering.

| | Decode via Python on PC | Decode via C++ on ESP32 (Recommended) |
| --- | --- | --- |
| Compatibility | Requires PC (or any device) that can run Python natively to upload images. | Cross-platform capability. Allow uploading image from any devices including phones and PC as long as there is browser and JavaScript access. |
| GUI support | Yes. Based on Gradio. | Yes. In the form of webpage hosted on ESP32. Not as good-looking as the one running on Gradio but still in active development. |

## Hardware

- ESP32-S3 with PSRAM
- Waveshare 1.54-inch e-Paper E display, 240 x 240

The default SPI and control pins are defined with reference to the [official guide](https://docs.waveshare.com/1.54inch_e-Paper_E).
The display is configured for the 9-pin interface. Verify the pinout and voltage requirements of your particular display before powering the circuit.

## Get started

### 1. Build and upload firmware

1. Install the ESP32 board support package by Espressif (for certain ESP32-S3 models, a package version of <=2.0.9 is required) and the **JPEGDEC** library.

2. Download a release package from the [Releases](https://github.com/lpkpaco/gadget32/releases) page and unzip it.

3. Choose your decoding method and open one of the following sketches in Arduino IDE:

- [`s3_cpp_decode.ino`](s3_cpp_decode.ino) for the ESP32-hosted browser uploader and JPEG decoding.
- [`s3_py_decode.ino`](s3_py_decode.ino) for the same JPEG decoding path in the standalone sketch.

4. Select your COM port and a ESP32-S3 board model with PSRAM enabled. **Do not forget to enable OPI PSRAM (or other mode) and set the flash sizes and partition mode in the Tools menu.** If you have no idea what your settings should be, try flash size = 16 MB, partition = HUGE APP and PSRAM = OPI PSRAM.

### 2. Connect to the display

After boot, the ESP32 creates this Wi-Fi network:

| Setting | Value |
| --- | --- |
| SSID | `epaper` |
| Password | `00000000` |
| Device address | `192.168.4.1` |

Connect your computer or phone to the network. In browser mode, visit [http://192.168.4.1](http://192.168.4.1), choose a JPEG, and select **Show**. The **Clear** button clears the panel.

The browser uploader centers the image on a white 240 x 240 canvas and sends it as a JPEG. Large uploads are limited by the firmware's 800,000-byte JPEG buffer.

## Python tools

Create a virtual environment if desired, then install the dependencies:

```text
python -m venv .venv
.venv\\Scripts\\activate
python -m pip install -r requirements.txt
```

Start the local interface:

```text
python gui.py
```

The interface provides two workflows:

### Upload an image

1. Choose an image and save it as `photo.jpg`.
2. Select **Convert to C++ array + binary**.
3. Select **Upload to Display**.

The converter writes:

- `frame.bin`, the 28,800-byte frame uploaded to the ESP32.
- `ImageData.cpp` and `ImageData.h`, a C++ representation suitable for embedding an image in firmware.

The Python uploader expects the computer to be connected to the `epaper` access point and posts `frame.bin` to `http://192.168.4.1/upload`.

### Generate a calendar

Use the **Calendar Generator** section to add non-overlapping events in 24-hour `HHMM` format. Generate the preview, then select **Draw and Upload Calendar to Display**. The result is saved as `calendar.png`, converted to `frame.bin`, and sent to the display.

The calendar renderer currently supports up to five events comfortably; longer event names or larger event lists may need font and layout adjustments in [`gen_cal.py`](gen_cal.py).

## Repository layout

| Path | Purpose |
| --- | --- |
| [`convert_epd.py`](convert_epd.py) | Resizes, quantizes, dithers, and packs images. |
| [`gen_cal.py`](gen_cal.py) | Validates events and renders the calendar image. |
| [`gui.py`](gui.py) | Gradio interface for image and calendar workflows. |
| [`request.py`](request.py) | HTTP upload and clear helpers. |
| [`s3_cpp_decode.ino`](s3_cpp_decode.ino) | Browser uploader with on-device JPEG decoding. |
| [`s3_py_decode.ino`](s3_py_decode.ino) | Standalone browser/JPEG decoding sketch. |
| [`ESP32-S3/s3_host`](ESP32-S3/s3_host) | Display driver, fonts, configuration, and host sketch. |
| [`page.h`](page.h) | Embedded browser upload page. |

## Known limitations

- The Wi-Fi credentials and device address are currently hard-coded in the firmware and Python helper.
- The browser firmware accepts JPEG input; the Python converter accepts images supported by Pillow.
- The panel has only six colors, so photographic images will show visible palette reduction and dithering.
- The ESP32 must have PSRAM available for the browser/JPEG decoding sketches.
- The Python and Arduino workflows are alternatives. Use the frame-upload firmware with `gui.py`; use the browser firmware with the built-in web page.

## Future work

- Move Wi-Fi credentials and the device address into configuration instead of hard-coding them.
- Add configurable display dimensions and palette settings.
- Improve calendar text wrapping and support for more events.
- Add a cleaner status response and upload error reporting to the Python client.
- Document tested ESP32-S3 board and display revisions.

## Credits

This project uses or is based on the following work:

- [Waveshare e-Paper examples and driver code](https://www.waveshare.com/wiki/1.54inch_e-Paper_Module_(E))
- [JPEGDEC](https://github.com/bitbank2/JPEGDEC) for on-device JPEG decoding
- [Pillow](https://python-pillow.org/) for image processing
- [Gradio](https://www.gradio.app/) for the local Python interface

## License

This repository is licensed under the [Apache License 2.0](LICENSE).

## Disclaimer

This project is provided as-is. Hardware wiring, power supplies, display revisions, and third-party libraries can affect operation. Check the documentation for your board and display before use.