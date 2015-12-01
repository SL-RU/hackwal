#ifndef testappDEF
#define testappDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define testappID 2
#define testappName "tsta"
#define testappDesc "App for testing"

//#define testappExtrenalEEPROMID 1

class testapp : public App
{
public:
  testapp(Core *cr);
  ~testapp(){}
  void update();
  byte getID();
  void drawGUI();
  void input_button(byte ID);
  void input_button_press(byte ID, unsigned long tm){}
  void print_all_commands();
  void input_command(char * comm, byte * commln, int commc, int len);
private:
	int var;
	int intEepromAddr;
  int extEepromAddr;
  void writeVar();
};
#endif