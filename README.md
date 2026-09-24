# gadget32
<hr>
Source codes and design files for ESP32-S3 powered image rendering and dithering for the WaveShare 1.54" 240 x 240 E6 color space e-paper display on a gadget. Beside computer-reliant Python-based controlling and image drawing, a C++ decoding with PSRAM allocating script is developed for ESP32 to decode image data into C-Style array and properly display image on the e-paper display. 

This project also includes a small Python/Gradio utility for preparing images and rendering a simple calendar.


## Description
### Software
The firmware targets the Waveshare 1.54-inch e-Paper E display, using the panel's six supported colors:
- black
- white
- yellow
- red
- blue
- green

Images are fitted into a 240 x 240 canvas, converted to the six-color palette, and packed as two 4-bit pixels per byte. A complete frame is 28,800 bytes.

There are two pipelines to send an image:

- **Decode via Python:** using Gradio frontend GUI with Python backend to convert an image or calendar to `frame.bin`, then upload the already-packed frame to the display to save computation power on the ESP32 and bypass certain ESP32 memory limits **(especially issues related to PSRAM due to allocation issues)**.

- **Decode via C++:** using ESP32 as an AP to host a control panel and send a JPEG directly through JavaScript and HTTP requests. The ESP32 performs JPEG decoding, resizing, palette conversion, and dithering.

| | Decode via Python on PC | Decode via C++ on ESP32 (Recommended) |
| --- | --- | --- |
| Compatibility | Requires PC (or any device) that can run Python natively to upload images. | Cross-platform capability. Allow uploading image from any devices including phones and PC as long as there is browser and JavaScript access. |
| GUI support | Yes. Based on Gradio. | Yes. In the form of webpage hosted on ESP32. |

<br>
<hr>

### Hardware

- ESP32-S3-DevModule with PSRAM, or
- Embeded **`ESP32-S3-WROOM-1-N16R8`** on a custom PCB (this repository)
- Waveshare 1.54-inch e-Paper E display, 240 x 240

The default SPI and control pins are defined with reference to the [official guide](https://docs.waveshare.com/1.54inch_e-Paper_E).
The display is configured for the 9-pin interface. Verify the pinout and voltage requirements of your particular display before powering the circuit.

#### Schematic
<img src="/assets/readme/SCH_gadget32-v1_0.svg">
The PDF version can be found in the code section.

#### BOM

| Package <br> Component Type | 0603 | 1206 | Other|
| --- | --- | --- | --- |
| Capacitor | 22 pF <br> 100 nF <br> 1 uF <br> 10 uF | None | None |
| Resistor | 0 Ohm <br> 10k Ohm | 5.1k Ohm | None |
| Diode | None | None | 1N5819 <br> LESD5D5.0CT1G-MS |
| Other | None | None | ESP32-S3-WROOM-1-N16R8 <br> TS-1187A-B-A-B <br> GT-USB-7010ASV <br> AMS1117-3.3V <br> 9-pin 2.54 mm connector|

#### PCB
The PCB is designed using EasyEDA Pro. This design is not the most optimized one but functional. It may subject to change in the future. \
The D+ and D- nets are differential pairs.

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


## Known limitations

- The ESP32 must have PSRAM available for the browser/JPEG decoding sketches.

## Future work

- [ ] Add configurable display dimensions and palette settings to support different models of display.
- [ ] Improve dithering and decoding algorithm.
- [ ] Improve calendar text wrapping and support for more events.
- [ ] Document tested ESP32-S3 board and display revisions.

## FAQ

Submit an issue ticket if there is a problem.

## Credits

This project uses or is based on the following work:

- [Waveshare driver code](https://www.waveshare.com/wiki/1.54inch_e-Paper_Module_(E))
- [JPEGDEC](https://github.com/bitbank2/JPEGDEC)

## License

Some codes in the full release packages have been licensed by Waveshare. Check the specific files for more information.
This repository is licensed under the [Apache License 2.0](LICENSE).

## Disclaimer

This project is provided as-is. Hardware wiring, power supplies, display revisions, and third-party libraries can affect operation. Check the documentation for your board and display before use.