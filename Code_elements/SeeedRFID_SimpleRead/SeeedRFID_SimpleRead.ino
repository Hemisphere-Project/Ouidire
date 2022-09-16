#include <SoftwareSerial.h>
#include <SeeedRFID.h>

#define RFID_TX_PIN 19 // RX on the RFID side
#define RFID_RX_PIN 23 // TX on the RFID side

int ledPin = 21; // TTGO T8 INTERNAL PIN
unsigned long Tstart;

#define DEBUG
// #define TEST

SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;

void setup() {
    Serial.begin(115200);
    Serial.println("RFID Test..");
    pinMode(ledPin, OUTPUT);
}

void loop() {
    if (RFID.isAvailable()) {
        tag = RFID.data();
        Serial.print("RFID card number: ");
        Serial.println(RFID.cardNumber());
        Tstart = millis();
        #ifdef TEST
        Serial.print("RFID raw data: ");
        for (int i = 0; i < tag.dataLen; i++) {
            Serial.print(tag.raw[i], HEX);
            Serial.print('\t');
        }
        #endif
    }else{

    }
    updateLed();

}


void updateLed(){
  unsigned long Tnow = millis();
  if(Tnow-Tstart<1000){
    digitalWrite(ledPin, HIGH);
  }else{
    digitalWrite(ledPin, LOW);
  }
}
