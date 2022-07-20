#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(19, 23);
unsigned char buffer[64];
int count = 0;

void setup() {
    SoftSerial.begin(9600);
    Serial.begin(115200);
    Serial.println("RFID RAW TEST");
}

void loop() {
    // if date is coming from software serial port ==> data is coming from SoftSerial shield
    if (SoftSerial.available())
    {
        while(SoftSerial.available())               // reading data into char array
        {
            buffer[count++] = SoftSerial.read();      // writing data into array
            if(count == 64)break;
            Serial.println(SoftSerial.read());
        }
        Serial.write(buffer, count);     // if no data transmission ends, write buffer to hardware serial port
        clearBufferArray();             // call clearBufferArray function to clear the stored data from the array
        count = 0;                      // set counter of while loop to zero
    }
    if (Serial.available())             // if data is available on hardware serial port ==> data is coming from PC or notebook
    SoftSerial.write(Serial.read());    // write it to the SoftSerial shield



}

void clearBufferArray() {
    for (int i=0; i<count; i++)
    {
        buffer[i]=NULL;
    }
}
