#ifndef ibuttonDEF
#define ibuttonDEF
#include "Arduino.h"
#include <EEPROM.h>
#include <OneWire.h>
#include "core.h"

#define ibuttonID 6
#define ibuttonName "IButton"
#define ibuttonDesc "This is app for reading/writing ibutton keys"

#define ibuttonExtrenalEEPROMID 2

class ibutton : public App
{
public:
  ibutton(Core *cr);
  ~ibutton();
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
  byte Data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  OneWire ds;
  bool read, write;
};
#endif