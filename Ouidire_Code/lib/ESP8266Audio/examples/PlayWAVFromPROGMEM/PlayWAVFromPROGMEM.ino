#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

// bip sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
#include "bip.h"

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioOutputI2SNoDAC *out;

void setup()
{
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(1000);
  Serial.printf("WAV start\n");

  audioLogger = &Serial;
  file = new AudioFileSourcePROGMEM( bip, sizeof(bip) );
  out = new AudioOutputI2SNoDAC();
  out->SetPinout(26,25,32);
  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
}

void loop()
{
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}
