//T8 V1.8 ESP32-WROVER 4MB PSRAM micro SD Test

#include <SD.h>


File dir;


void setup()
{

  Serial.begin(115200);
  Serial.println("MicroSD Test...");


  //Built in micro SD card slot
  SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI,SS
  SD.begin(13, SPI);

  dir = SD.open("/");
  printDirectory(dir, 0);

}

void loop()
{

}

void printDirectory(File dir, int depth) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry)
      break;

    for (uint8_t i = 0; i < depth; i++)
      Serial.print("|    ");

    std::string name = entry.name();
    if (entry.isDirectory()) {
      Serial.print("+----" + String(name.substr(name.find_last_of("/\\") + 1).c_str()) + "\r\n");
      printDirectory(entry, depth + 1);
    } else {
      Serial.print("+--" + String(name.substr(name.find_last_of("/\\") + 1).c_str()));
      Serial.print("\t\t\t(");
      Serial.print(entry.size(), DEC);
      Serial.println(")");
    }
    entry.close();
  }
}
