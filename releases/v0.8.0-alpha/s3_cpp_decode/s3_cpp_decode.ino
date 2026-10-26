#include <WiFi.h>
#include <WebServer.h>
#include <JPEGDEC.h>
#include "EPD_1in54e.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"
#include "page.h"
const char* ssid = "epaper";
const char* pass = "00000000";
WebServer server(80);
JPEGDEC jpeg;
static const int W = 240;
static const int H = 240;
static const int FRAME = W * H / 2;
static const int JPG_MAX = 800000;

uint8_t* jpg = nullptr;
uint8_t* rpg;
uint8_t* frame;
size_t jpg_n = 0;
uint8_t* rgb = nullptr;
volatile bool pending = false;

void clear() {
  EPD_1IN54E_Init();
  EPD_1IN54E_Clear(EPD_1IN54E_WHITE);
  EPD_1IN54E_Sleep();
  server.send(200, "text/plain", "cleared");
}

void upload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    jpg_n = 0;
    Serial.println("START");
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (!jpg) return;
    size_t room = JPG_MAX - jpg_n;
    size_t c = upload.currentSize < room ? upload.currentSize : room;
    memcpy(jpg + jpg_n, upload.buf, c);
    jpg_n += c;
    Serial.printf("WRITE +%u total=%u\n", (unsigned)c, (unsigned)jpg_n);
  }
  else if (upload.status == UPLOAD_FILE_END) {
    Serial.printf("END total=%u %02X %02X\n",
                  (unsigned)jpg_n, jpg[0], jpg[1]);
    if (jpg_n > 100 && decode_and_show()) {
      server.send(200, "text/plain", "JPEG Accepted");
  }
    else {
      server.send(400, "text/plain", "Bad JPEG");
  }
}
}

int srcW = 0, srcH = 0;

int drawMCU(JPEGDRAW *p) {
  if (!p || !p->pPixels) return 0;
  if (srcW <= 0 || srcH <= 0) return 0;

  int bw = p->iWidth;
  int bh = p->iHeight;
  for (int y = 0; y < bh; y++) {
    for (int x = 0; x < bw; x++) {
      int dx = (p->x + x) * W / srcW;
      int dy = (p->y + y) * H / srcH;
      if ((unsigned)dx >= W || (unsigned)dy >= H) continue;
      uint16_t pix = p->pPixels[y * bw + x];
      int r = ((pix >> 11) & 0x1F) * 255 / 31;
      int g = ((pix >> 5)  & 0x3F) * 255 / 63;
      int b = (pix & 0x1F) * 255 / 31;
      int o = (dy * W + dx) * 3;
      rgb[o] = r; rgb[o + 1] = g; rgb[o + 2] = b;
    }
  }
  return 1;
}

bool decode_and_show() {
  Serial.println("openRAM");
  if (!jpeg.openRAM(jpg, (int)jpg_n, drawMCU)) {
    Serial.printf("open err %d\n", jpeg.getLastError());
    return false;
  }
  srcW = jpeg.getWidth();
  srcH = jpeg.getHeight();
  Serial.printf("size %d x %d\n", srcW, srcH);

  jpeg.setPixelType(RGB565_LITTLE_ENDIAN);
  jpeg.setMaxOutputSize(8);
  memset(rgb, 255, W * H * 3);
  Serial.println("decode");
  bool ok = jpeg.decode(0, 0, 0);
  jpeg.close();
  Serial.printf("decode ok=%d err=%d\n", ok, jpeg.getLastError());
  if (!ok) return false;

  Serial.println("dither");
  dither_pack();
  Serial.println("display");
  EPD_1IN54E_Init();
  EPD_1IN54E_Display(frame);
  EPD_1IN54E_Sleep();
  Serial.println("done");
  return true;
}

const uint8_t PAL[6][3] = {
  {0, 0, 0}, {255, 255, 255}, {255, 255, 0}, {255, 0, 0}, {0, 0, 255}, {0, 255, 0}
};

int nearest(int r, int g, int b) {
  int best = 0, bd = 1e9;
  for (int i = 0; i < 6; i++) {
    int dr = r-PAL[i][0], dg = g-PAL[i][1], db = b-PAL[i][2];
    int d = dr * dr + dg * dg + db * db;
    if (d < bd) { bd = d; best = i; }
  }
  return best;
}

void dither_pack() {
  int16_t* R = (int16_t*)ps_malloc(W * H *2);
  int16_t* G = (int16_t*)ps_malloc(W * H * 2);
  int16_t* B = (int16_t*)ps_malloc(W * H * 2);
  uint8_t* idx = (uint8_t*)ps_malloc(W * H);
  if (!R || !G || !B || !idx) {
    Serial.println("dither malloc fail");
    free(R); free(G); free(B); free(idx);
    return;
  }
  for (int i = 0; i < W * H; i++) {
    R[i] = rgb[i * 3];
    G[i] = rgb[i * 3 + 1];
    B[i] = rgb[i * 3 + 2];
  }
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      int i = y * W + x;
      int r = constrain(R[i], 0, 255);
      int g = constrain(G[i], 0, 255);
      int b = constrain(B[i], 0, 255);
      int p = nearest(r,g,b);
      idx[i] = p;
      int er = r - PAL[p][0], eg = g - PAL[p][1], eb = b - PAL[p][2];
      auto spread = [&](int nx, int ny, int num) {
        if (nx < 0 || nx >=W || ny < 0 || ny >= H) return;
        int j = ny * W + nx;
        R[j] += er * num / 16;
        G[j] += eg * num / 16;
        B[j] += eb * num / 16;
      };
      spread(x + 1, y, 7); spread(x - 1, y + 1, 3); spread(x, y + 1, 5); spread(x + 1, y + 1, 1);
    }
  }
  for (int i = 0; i < W * H; i += 2)
    frame[i / 2] = (idx[i] << 4) | idx[i + 1];
  free(R);
  free(G);
  free(B);
  free(idx);
}

void setup() {
  Serial.begin(115200);
  DEV_Module_Init();
  jpg   = (uint8_t*)ps_malloc(JPG_MAX);
  rgb   = (uint8_t*)ps_malloc(W * H * 3);
  frame = (uint8_t*)ps_malloc(FRAME);
  Serial.printf("jpg=%p rgb=%p frame=%p\n", jpg, rgb, frame);
  if (!jpg || !rgb || !frame) {
    Serial.println("PSRAM off or malloc failed");
    while (true) delay(1000);
  }
  WiFi.softAP(ssid, pass);
  server.on("/", HTTP_GET, []() {
  server.send_P(200, "text/html", PAGE);
  });
  server.on("/clear", HTTP_GET, clear);
  server.begin();
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "uploaded");
  }, upload);
  pending = true;
}

void loop() {
  server.handleClient();
  if (pending) {
    pending = false;
    Serial.println("Start decoding");
    decode_and_show();
    Serial.println("Decoding done");
  }
}
