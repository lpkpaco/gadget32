#include <WiFi.h>
#include <WebServer.h>
#include "EPD_1in54e.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"
const char* ssid = "epaper";
const char* pass = "00000000";
WebServer server(80);
uint8_t frame[28800];

void clear() {
  EPD_1IN54E_Init();
  EPD_1IN54E_Clear(EPD_1IN54E_WHITE);
  EPD_1IN54E_Sleep();
  server.send(200, "text/plain", "cleared");
}

void upload() {
  HTTPUpload& upload = server.upload();
  static size_t n =0;
  if (upload.status == UPLOAD_FILE_START) {
    n = 0;
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    size_t room = sizeof(frame) - n;
    size_t c = upload.currentSize < room ? upload.currentSize : room;
    memcpy(frame + n, upload.buf, c);
    n += c;
  }
  else if (upload.status == UPLOAD_FILE_END) {
    if (n == 28800) {
      EPD_1IN54E_Init();
      EPD_1IN54E_Display(frame);
      EPD_1IN54E_Sleep();
    }
  }
}
void setup() {
  DEV_Module_Init();
  WiFi.softAP(ssid, pass);
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "uploaded");
  }, upload);
  server.on("/clear", HTTP_GET, clear);
  server.begin();
}

void loop() {
  server.handleClient();
}
