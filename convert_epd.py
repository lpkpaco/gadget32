from PIL import Image
import sys
SIZE = 240
OUT_CPP = "ImageData.cpp"
OUT_H = "ImageData.h"
ARRAY = "Image6color"
PALETTE = [
    (0, 0, 0),
    (255, 255, 255),
    (255, 255, 0),
    (255, 0, 0),
    (0, 0, 255),
    (0, 255, 0),
] #BWYRBG
def pack(img):
    img = img.convert("RGB")
    img.thumbnail((SIZE, SIZE), Image.Resampling.LANCZOS)
    canvas = Image.new("RGB", (SIZE, SIZE), (255, 255, 255))
    canvas.paste(img, ((SIZE - img.width) // 2, (SIZE - img.height) // 2))
    pal = Image.new("P", (1, 1))
    raw = []
    for c in PALETTE:
        raw.extend(c)
    raw += [0, 0, 0] * (256 - len(PALETTE))
    pal.putpalette(raw)
    q = canvas.quantize(palette=pal, dither=Image.FLOYDSTEINBERG)
    pix = list(q.getdata())
    buf = bytearray()
    for i in range(0, len(pix), 2):
        a = pix[i] & 0x0F
        b = pix[i + 1] & 0x0F
        buf.append((a << 4) | b)
    return buf
def calendar_convert():
    buf = pack(Image.open("calendar.png"))
    print("bytes:", len(buf), "(must be 28800)")
    open("frame.bin", "wb").write(buf)
    print("Wrote to frame.bin")
def writecpph():
    src = sys.argv[1] if len(sys.argv) > 1 else "photo.jpg"
    buf = pack(Image.open(src))
    print("bytes:", len(buf), "(must be 28800)")
    with open(OUT_CPP, "w") as f:
        f.write('#include "ImageData.h"\n\n')
        f.write(f"const unsigned char {ARRAY}[] = {{\n")
        for i, b in enumerate(buf):
            if i % 16 == 0:
                f.write("  ")
            f.write(f"0x{b:02X},")
            f.write("\n" if i % 16 == 15 else " ")
        f.write("\n};\n")
    with open(OUT_H, "w") as f:
        f.write("#ifndef IMAGEDATA_H\n#define IMAGEDATA_H\n\n")
        f.write(f"extern const unsigned char {ARRAY}[];\n\n")
        f.write("#endif\n")

    print("Wrote to", OUT_CPP, "and", OUT_H)

def writebin():
    src = sys.argv[1] if len(sys.argv) > 1 else "photo.jpg"
    buf = pack(Image.open(src))
    print("bytes:", len(buf), "(must be 28800)")
    open("frame.bin", "wb").write(buf)
    print("Wrote to frame.bin")

if __name__ == "__main__":
    writecpph()
    writebin()