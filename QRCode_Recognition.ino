#include "ESPino32CAM.h"
#include "ESPino32CAM_QRCode.h"
ESPino32CAM cam;
ESPino32QRCode qr;

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();
#define SIZEX 480
#define SIZEY 320

#define BUTTON_QR 0
void setup() {
  //pinMode(4,OUTPUT);
  //digitalWrite(4,HIGH);
  Serial.begin(115200);
  Serial.println("\r\nESPino32CAM");
  if (cam.init() != ESP_OK)
  {
    Serial.println(F("Fail"));
    while (1);
  }
  qr.init(&cam);
  sensor_t *s = cam.sensor();
  s->set_framesize(s, FRAMESIZE_CIF);
  s->set_whitebal(s, true);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  delay(1000);
  tft.fillScreen(TFT_WHITE);
  delay(1000);
  tft.fillScreen(TFT_BLACK);
}
void loop()
{
  unsigned long pv_time  = millis();
  camera_fb_t *fb = cam.capture();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }
  dl_matrix3du_t *rgb888, *rgb565;
  if (cam.jpg2rgb(fb, &rgb888))
  {
    rgb565 = cam.rgb565(rgb888);
    tft.pushImage(40, 11, rgb565->w, rgb565->h, (uint16_t*) rgb565->item);
  }
  cam.clearMemory(rgb888);
  cam.clearMemory(rgb565);
  //Serial.write(fb->buf, fb->len);
  dl_matrix3du_t *image_rgb;
  if (cam.jpg2rgb(fb, &image_rgb))
  {
    cam.clearMemory(fb);
    if (!digitalRead(BUTTON_QR))
    {
      cam.printDebug("\r\nQR Read:");
      qrResoult res = qr.recognition(image_rgb);
      if (res.status)
      {
        cam.printDebug("Payload: " + res.payload);
        tft.setTextColor(TFT_WHITE, TFT_BLUE);
        String text = "QR Read:" + res.payload;
        tft.drawCentreString(text, 240, 280, 4);
        delay(1000);
      }
      else
      {
        cam.printDebug("FAIL");
        tft.setTextColor(TFT_WHITE, TFT_BLUE);
        String text = "QR Read: FAIL";
        tft.drawCentreString(text, 240, 280, 4);
      }
    }
  }
  cam.clearMemory(image_rgb);
}
