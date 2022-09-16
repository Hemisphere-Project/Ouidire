#include <Arduino.h>
#include <SD.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

File dir;
AudioFileSourceSD *source;
AudioOutputI2S *output;
AudioGeneratorWAV *decoder;

// SD
// SCK --- 14
// MISO -- 2
// MOSI -- 15
// CS ---- 13

// DAC (PCM5102)
// bclk -- 26 (BCK)
// wclk -- 25 (LCK)
// dout -- 22 (DIN)
// SCK --- GND

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.printf("WAV start\n");

  SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
  SD.begin(13, SPI);
  dir = SD.open("/");
  // printDirectory(dir, 0);

  source = new AudioFileSourceSD();
  output = new AudioOutputI2S();
  output->SetPinout(26,25,22); // BCLK, WCLK, DOUT
  decoder = new AudioGeneratorWAV();

  output->SetGain(1.0);

  // DIRECT OPEN
  source->open("/3162207.wav");
  decoder->begin(source, output);


}

void loop()
{
  if (decoder->isRunning()) {
    if (!decoder->loop()) decoder->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }

  // SCAN & LISTEN
  // if ((decoder) && (decoder->isRunning())) {
  //   if (!decoder->loop()) decoder->stop();
  // } else {
  //   File file = dir.openNextFile();
  //   if (file) {
  //     if (String(file.name()).endsWith(".wav")) {
  //       source->close();
  //       if (source->open(file.name())) {
  //         Serial.println(file.name());
  //         Serial.printf_P(PSTR("Playing '%s' from SD card...\n"), file.name());
  //         decoder->begin(source, output);
  //       } else {
  //         Serial.printf_P(PSTR("Error opening '%s'\n"), file.name());
  //       }
  //     }
  //   } else {
  //     Serial.println(F("Playback form SD card done\n"));
  //     delay(1000);
  //   }
  // }

}




// void printDirectory(File dir, int depth) {
//   while (true) {
//     File entry =  dir.openNextFile();
//     if (! entry)
//       break;
//
//     for (uint8_t i = 0; i < depth; i++)
//       Serial.print("|    ");
//
//     std::string name = entry.name();
//     if (entry.isDirectory()) {
//       Serial.print("+----" + String(name.substr(name.find_last_of("/\\") + 1).c_str()) + "\r\n");
//       printDirectory(entry, depth + 1);
//     } else {
//       Serial.print("+--" + String(name.substr(name.find_last_of("/\\") + 1).c_str()));
//       Serial.print("\t\t\t(");
//       Serial.print(entry.size(), DEC);
//       Serial.println(")");
//     }
//     entry.close();
//   }
// }
