#ifndef templateappDEF
#define templateappDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define templateappID 228
#define templateappName "Template app"
#define templateappDesc "This is clear app for copypasting"

#define templateappExtrenalEEPROMID -300

class templateapp : public App
{
public:
  templateapp(Core *cr);
  ~templateapp();
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
};
#endif