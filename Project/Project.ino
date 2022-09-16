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
#define LED_PIN  21 // TTGO T8 INTERNAL PIN
unsigned long Tstart;

#define DEBUG
#define VBAT_PIN 35

#define LED4_PIN 4
#define LED3_PIN 12
#define LED2_PIN 13
#define LED1_PIN 27

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
    pinMode(LED_PIN, OUTPUT);
    // SD
    SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
    SD.begin(13, SPI);
    dir = SD.open("/");
    // AUDIO
    source = new AudioFileSourceSD();
    output = new AudioOutputI2S();
    output->SetPinout(26,25,22); // BCLK, WCLK, DOUT
    decoder = new AudioGeneratorWAV();
    // BATTERY
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);
}

void loop() {
    updateRFID();
    updateLed();
    updateAudio();
    updateBattery();
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
    digitalWrite(LED_PIN, HIGH);
  }else{
    digitalWrite(LED_PIN, LOW);
  }
}

void updateBattery(){

  // Charging     4.2
  // Low          3.4
  // Empty        3.2

  float read = analogRead(VBAT_PIN);
  // Serial.print("Analog read: ");
  // Serial.println(read);
  float vbat = (read  / 4096) * 3.3 * 2 * 1.05;
  Serial.print("V Bat: ");
  Serial.println(vbat);

  if(vbat > 4){                   // Charging
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
  if((vbat > 3.4)&&(vbat <= 4)){  // OK
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
  if((vbat > 3.2)&&(vbat <= 3.4)){  // Low
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
  if(vbat <= 3.2){
    digitalWrite(LED1_PIN, LOW);  // Empty
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, HIGH);
  }

}
