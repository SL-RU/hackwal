#ifndef pingpongDEF
#define pingpongDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define pingpongID 4
#define pingpongName "pipo"
#define pingpongDesc "Simple ping pong game"


class pingpong : public App
{
public:
  pingpong(Core *cr);
  ~pingpong(){}
  void update();
  byte getID();
  void drawGUI();
  void input_button(byte ID);
  void input_button_press(byte ID, unsigned long tm);
  void print_all_commands();
  void input_command(char * comm, byte * commln, int commc, int len);
private:
  int intEepromAddr;

  int x, y, w, h;
  byte lastId;
  unsigned long lastTm;

};
#endif