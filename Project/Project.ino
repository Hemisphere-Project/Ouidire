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
#define VBUS_PIN 34

#define LED3_PIN 13
#define LED2_PIN 12
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
    // SD
    SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
    SD.begin(13, SPI);
    dir = SD.open("/");
    // AUDIO
    source = new AudioFileSourceSD();
    output = new AudioOutputI2S();
    output->SetPinout(26,25,22); // BCLK, WCLK, DOUT
    decoder = new AudioGeneratorWAV();
    // LEDS & INFO
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(VBUS_PIN, INPUT);
}

void loop() {
    updateRFID();
    updateAudio();
    updateLeds();
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
    delay(100);
  }
}

void playAudio(unsigned long cardNumber){
  Serial.println("/"+String(cardNumber)+".wav");

  decoder->stop();
  source->close();
  source->open(("/"+String(cardNumber)+".wav").c_str());
  decoder->begin(source, output);

}

void updateLeds(){

  // DETECT RFID
  unsigned long Tnow = millis();
  if(Tnow-Tstart<1000){
    digitalWrite(LED_PIN, HIGH);
  }else{
    digitalWrite(LED_PIN, LOW);
  }

  // VBUS
  bool charging = false;
  float vbus_raw = analogRead(VBUS_PIN);
  if (vbus_raw > 100){
    bool charging = true;
    digitalWrite(LED1_PIN, HIGH);
  }else{
    digitalWrite(LED1_PIN, LOW);
  }

  // BATTERY
  // Charging     4.2
  // Low          3.4
  // Empty        3.2
  float vbat_raw = analogRead(VBAT_PIN);
  float vbat = (vbat_raw  / 4096) * 3.3 * 2 * 1.05;

  if(vbat > 3.4){  // OK
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
  }
  if((vbat > 3.2)&&(vbat <= 3.4)){  // Low
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, HIGH); /// LOW PEAK WHEN TRIGGERING SOUND ?
  }
  // if(vbat <= 3.2){               // Empty
  //   digitalWrite(LED2_PIN, LOW);
  //   digitalWrite(LED3_PIN, LOW);
  // }


  // TO DO: Merge into one led
  // RFID detect - mega rapid blink - 1sec
  // CHARGING - blink tranquille
  // > 3.4 && !charging - ON
  // <= 3.4 - LOW - rapid blink
  if(Tnow-Tlast<period){
    digitalWrite(LED_PIN, HIGH);
  }else{
    digitalWrite(LED_PIN, LOW);
  }

}
