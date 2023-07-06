// SD
#include <Arduino.h>
#include <SD.h>
// SeeedRFID
// https://github.com/Seeed-Studio/RFID_Library
#include <SoftwareSerial.h>
#include <SeeedRFID.h>
// ESP8266Audio
// https://github.com/earlephilhower/ESP8266Audio
#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

// RFID
// TX -- 19 (RX On the RFID Side)
// RX -- 23 (TX On the RFID Side)

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

//RFID
#define RFID_TX_PIN 19 // RX on the RFID side
#define RFID_RX_PIN 23 // TX on the RFID side
unsigned long Tdetect = 0;
unsigned long Tblink = 0;

#define DEBUG
#define VBAT_PIN 35
#define VBUS_PIN 34

// VISU
#define LED_PCB  21 // TTGO T8 INTERNAL PIN
#define LED_PIN  12 // LED BTN
int ledState = LOW;

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
    output->SetGain(1);//0-----1 (50% = 0.5)
    playAudio("Bonjour");
    // LEDS & INFO
    pinMode(LED_PCB, OUTPUT);
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
      Tdetect = millis();
      playAudio(String(RFID.cardNumber()));
  }
}

void updateAudio(){
  if (decoder->isRunning()) {
    if (!decoder->loop()) decoder->stop();
  } else {
    // Serial.printf("WAV done\n");
    delay(100);
  }
}

void playAudio(String cardNumber){
  Serial.println("Play /"+cardNumber+".wav");

  decoder->stop();
  source->close();
  source->open(("/"+cardNumber+".wav").c_str());
  decoder->begin(source, output);

}

void updateLeds(){

  unsigned long Tnow = millis();
  String state = "undefined";
  // detecting      LED OFF
  // charging       LED BLINK 1sec
  // normal         LED ON
  // low_battery    LED BLINK 200ms

  // BATTERY -- VBAT
  float vbat_raw = analogRead(VBAT_PIN);
  float vbat = (vbat_raw  / 4096) * 3.3 * 2 * 1.05;

  // CHARGE USB -- VBUS
  float vbus_raw = analogRead(VBUS_PIN);


  // DEFINE STATES
  // DETECT RFID
  if(Tnow-Tdetect<1000){
    state = "detecting";
  }else{
    // VBAT
    if(vbat > 3.4){
      state = "normal";
    }
    if((vbat > 3.2)&&(vbat <= 3.4)){
      state = "low_battery";
    }
    // VBUS
    if (vbus_raw > 100){
      state = "charging";
    }
  }

  // LED
  if(state=="detecting"){
    ledState = LOW;
  }
  if(state=="normal"){
    ledState = HIGH;
  }
  if(state=="low_battery"){
    if(Tnow-Tblink>100){
      Tblink = Tnow;
      if (ledState == LOW) { ledState = HIGH; }
      else { ledState = LOW; }
    }
  }
  if(state=="charging"){
    if(Tnow-Tblink>1000){
      Tblink = Tnow;
      if (ledState == LOW) { ledState = HIGH; }
      else { ledState = LOW; }
    }
  }
  digitalWrite(LED_PIN, ledState);

  // LED PCB
  if(state=="detecting"){
    digitalWrite(LED_PCB, HIGH);
  }else{
    digitalWrite(LED_PCB, LOW);
  }




}
