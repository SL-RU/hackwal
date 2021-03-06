#ifndef sysinfDEF
#define sysinfDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define sysinfID 3
#define sysinfName "sysi"
#define sysinfDesc "System information"

class sysinf : public App
{
public:
  sysinf(Core *cr);
  ~sysinf();
  void update();
  byte getID();
  void drawGUI();
  void drawButtons();
  void input_button(byte ID);
  void input_button_press(byte ID, unsigned long tm){}
  void print_all_commands();
  void input_command(char * comm, byte * commln, int commc, int len);
private:
	int intEepromAddr;
	String buttonsbuf;
	int selButton;
};
#endif