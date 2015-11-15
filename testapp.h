#ifndef testappDEF
#define testappDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define testappID 2
#define testappName "Test app"
#define testappDesc "App for testing"

#define testappExtrenalEEPROMID 1

class testapp : public App
{
public:
  testapp(Core *cr);
  ~testapp();
  void update();
  byte getID();
  void drawGUI();
  void input_button(int ID);
  void print_all_commands();
  void input_command(char * comm, byte * commln, int commc, int len);
private:
	byte var;
	int intEepromAddr;
  int extEepromAddr;
  void writeVar();
};
#endif