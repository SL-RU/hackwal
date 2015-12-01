#ifndef notesappDEF
#define notesappDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define notesID 7
#define notesName "note"
#define notesDesc "Simple notes app"

#define notesExtrenalEEPROMID 20
//len = 70

class notesapp : public App
{
public:
  notesapp(Core *cr);
  ~notesapp();
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