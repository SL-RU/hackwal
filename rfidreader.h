#ifndef rfidreaderDEF
#define rfidreaderDEF
#include <SoftwareSerial.h>
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define rfidreaderID 5
#define rfidreaderName "RFID reader"
#define rfidreaderDesc "Read data of rfid cards"

#define rfidreaderExtrenalEEPROMID 0

class rfidreader : public App
{
public:
  rfidreader(Core *cr);
  ~rfidreader(){}
  void update();
  byte getID();
  void drawGUI();
  void input_button(byte ID);
  void input_button_press(byte ID, unsigned long tm){}
  void print_all_commands();
  void input_command(char * comm, byte * commln, int commc, int len);
private:
  int intEepromAddr;
  int extEepromAddr;
  SoftwareSerial ser;
  String inp;
};
#endif