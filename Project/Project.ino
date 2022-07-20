// SeeedRFID
#include <SoftwareSerial.h>
#include <SeeedRFID.h>
// SD
#include <Arduino.h>
#include <SD.h>
// ESP8266Audio
#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

// RFID
// TX -- 19 (RX On the RFID Side)
// RX -- 19 (TX On the RFID Side)

// SD
// SCK --- 14
// MISO -- 2
// MOSI -- 15
// CS ---- 13
// (format SD: FAT, MBR)

// DAC (PCM5102)
// bclk -- 26 (BCK)
// wclk -- 25 (LCK)
// dout -- 22 (DIN)
// SCK --- GND

// TO FLASH:
// Mac: Install USB manager CH9102 (CH34XSER_MAC)
// Click 'upload', & Turn off & on the power switch on the TTGO while arduino IDE tries to connect (after compiling)


#define RFID_TX_PIN 19 // RX on the RFID side
#define RFID_RX_PIN 23 // TX on the RFID side
int ledPin = 21; // TTGO T8 INTERNAL PIN
unsigned long Tstart;

#define DEBUG

File dir;
AudioFileSourceSD *source;
AudioOutputI2S *output;
AudioGeneratorWAV *decoder;


SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;

void setup() {
    // GLOBAL
    Serial.begin(115200);
    delay(1000);
    Serial.println("OUI DIRE");
    pinMode(ledPin, OUTPUT);
    // SD
    SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
    SD.begin(13, SPI);
    dir = SD.open("/");
    // AUDIO
    source = new AudioFileSourceSD();
    output = new AudioOutputI2S();
    output->SetPinout(26,25,22); // BCLK, WCLK, DOUT
    decoder = new AudioGeneratorWAV();
}

void loop() {
    updateRFID();
    updateLed();
    updateAudio();
}

void updateRFID(){
  if (RFID.isAvailable()) {
      tag = RFID.data();
      Serial.print("RFID card number: ");
      Serial.println(RFID.cardNumber());
      Tstart = millis();
      playAudio(RFID.cardNumber());
  }
}

void updateAudio(){
  if (decoder->isRunning()) {
    if (!decoder->loop()) decoder->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}

void playAudio(unsigned long cardNumber){
  Serial.println("/"+String(cardNumber)+".wav");

  decoder->stop();
  source->close();
  source->open(("/"+String(cardNumber)+".wav").c_str());
  decoder->begin(source, output);


}


void updateLed(){
  unsigned long Tnow = millis();
  if(Tnow-Tstart<1000){
    digitalWrite(ledPin, HIGH);
  }else{
    digitalWrite(ledPin, LOW);
  }
}
