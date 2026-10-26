# gadget32 v0.8.0-alpha

**Release type:** Alpha  
**Release date:** 2026-10-27

## Issues
To report issues or to enquire, please open a ticket in the [`Issues`](https://github.com/lpkpaco/gadget32/issues) page.

> **Alpha notice:** This release is functional but not production-ready. Hardware
> revisions, display variants, ESP32-S3 board configurations, and third-party
> library versions may affect results. Validate the design and firmware on your
> own hardware before manufacturing or deployment.

## Highlights

- ESP32-S3 support with PSRAM for image decoding and frame preparation.
- Support for the e-paper panel's six-color palette:
  black, white, yellow, red, blue, and green.
- 240 × 240 image fitting with centered layout and two 4-bit pixels packed
  into each byte.
- Python-side image conversion to a 28,800-byte `frame.bin` file.
- C++ image conversion output as `ImageData.cpp` and `ImageData.h`.
- On-device JPEG decoding, resizing, palette conversion, and dithering.
- Browser-based image upload and display control through an ESP32 access point.
- Gradio-based desktop utility for image conversion and calendar generation.
- Calendar rendering with event conflict detection and direct display upload.
- Custom ESP32-S3-WROOM-1-N16R8 hardware design for the display controller.

## Included in the release package

### Hardware design

- Schematic for the gadget32 ESP32-S3 and e-paper display controller.
- EasyEDA Pro PCB design files.
- Bill of materials reference for the ESP32-S3 module, power circuitry,
  protection components, connectors, and supporting passives.

The design targets an `ESP32-S3-WROOM-1-N16R8` module or an
ESP32-S3 development board with PSRAM and a Waveshare 1.54-inch e-Paper E
display using the nine-pin interface.

### ESP32 firmware

- `s3_cpp_decode.cpp`
  - Runs an ESP32-hosted browser interface.
  - Accepts JPEG uploads over HTTP.
  - Decodes, resizes, dithers, and displays images on the ESP32.
  - Provides browser controls for showing an image and clearing the display.
- `s3_py_decode.cpp`
  - Receives pre-packed `frame.bin` image data from a computer or other client.
  - Displays the packed frame without requiring JPEG decoding on the ESP32.
- `ESP32-S3/s3_host/`
  - E-paper driver, display drawing support, fonts, embedded page content,
    and example image data used by the host firmware.

### Python tools

- `gui.py` — Gradio interface for image conversion, upload, display clearing,
  and calendar generation.
- `convert_epd.py` — Converts images to the six-color palette and writes
  `frame.bin`, `ImageData.cpp`, and `ImageData.h`.
- `gen_cal.py` — Generates 240 × 240 calendar images and detects overlapping
  events.
- `request.py` — Uploads packed frames to the ESP32 or clears the display.
- `requirements.txt` — Python dependencies for the desktop tools.

## Getting started

### Hardware and firmware

#### Setting up
1. Install the Espressif ESP32 board support package in Arduino IDE.
   Some ESP32-S3 configurations require version 2.0.9 or earlier.
2. Install the `JPEGDEC` library when using the C++ JPEG-decoding pipeline.
3. Extract the release package.
4. Open the desired firmware source in Arduino IDE. (If this option is not available, install ESP32 from board manager by Espressif **version 2.0.9**.
5. Select an ESP32-S3 board with PSRAM enabled.
6. Configure the board with the appropriate flash size, partition scheme,
   and PSRAM mode. A typical configuration is:
   - Flash size: 16 MB
   - Partition scheme: Huge APP
   - PSRAM: OPI PSRAM
7. Verify the display wiring and power requirements before connecting the panel.

#### Flashing software
1. Unzip the release package. Open `s3_cpp_decode.ino` or `s3_python_decode.ino` in Arduino IDE. Other required files should also be loaded in the Arduino IDE automatically.
2. Verify and flash onto the ESP32 board.
3. Connect your device to the ESP32 AP network.

#### Connecting to ESP32
##### (a) If you are using s3_cpp_decode.ino:

After boot, the ESP32 creates this Wi-Fi network:

| Setting | Value |
| --- | --- |
| SSID | `epaper` |
| Password | `00000000` |
| Device address | `192.168.4.1` |

Connect your computer or phone to the network. In browser mode, visit [http://192.168.4.1](http://192.168.4.1), choose a JPEG, and select **Show**. The **Clear** button clears the panel.

The browser uploader centers the image on a white 240 x 240 canvas and sends it as a JPEG. Large uploads are limited by the firmware's 800,000-byte JPEG buffer.

##### (b) If you are using s3_python_decode.ino:

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

After boot, the ESP32 creates the following Wi-Fi access point:

| Setting | Value |
| --- | --- |
| SSID | `epaper` |
| Password | `00000000` |
| Device address | `192.168.4.1` |

Connect a phone or computer to the access point and open
`http://192.168.4.1`. Select a JPEG and choose **Show**. Use **Clear** to
clear the display.

The browser pipeline centers the image on a white 240 × 240 canvas. JPEG
uploads are limited by the firmware's 800,000-byte input buffer.

### Python conversion pipeline

Create and activate a virtual environment, then install the dependencies:

```text
python -m venv .venv
.venv\Scripts\activate
python -m pip install -r requirements.txt
```

Start the Gradio interface:

```text
python gui.py
```

For an image workflow:

1. Select an image and save it as `photo.jpg`.
5. Choose **Convert to C++ array + binary**.
6. Choose **Upload to Display** while connected to the `epaper` network.

The converter generates a 28,800-byte `frame.bin` file and C++ image data
that can be embedded in firmware.

## Known limitations

- The ESP32 must have working PSRAM for the browser/JPEG-decoding sketches.
- The display dimensions and six-color palette are currently fixed at
  240 × 240 and six colors.
- PCB routing and component placement are functional but not fully optimized.
- Display revisions, ESP32-S3 board variants, and power supplies may require
  hardware or configuration changes.
- Calendar text wrapping and support for larger event sets are limited.
- The firmware and desktop tools have limited validation across board and
  operating-system combinations in this alpha release.

## Planned follow-up work

- Add configurable display dimensions and palette settings.
- Improve dithering and decoding quality.
- Improve calendar text wrapping and event support.
- Document tested ESP32-S3 boards and display revisions.
- Further optimize and validate the PCB design.

## Credits and licensing

gadget32 uses or is based on the Waveshare driver code and the
[JPEGDEC](https://github.com/bitbank2/JPEGDEC) library.

This repository is licensed under the [Apache License 2.0](LICENSE).
Some files in the release package are licensed by Waveshare; check the
individual files for their applicable license notices.

## Disclaimer

This release is provided as-is. The project authors are not responsible for
damage caused by incorrect wiring, unsuitable power supplies, incompatible
display revisions, or changes made to the hardware and firmware.
