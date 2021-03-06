#ifndef RFIDSpooferDEF
#define RFIDSpooferDEF
#include "Arduino.h"
#include <EEPROM.h>
#include "core.h"

#define RFIDSpooferID 1
#define RFIDSpooferExtrenalEEPROMID 0

class RFIDSpoofer : public App  //RFIDSpoofer class. Emulates rfid card with particular code.
{
  /*
  EEPROM:
    block size: 22
      first byte - if 0 - no data, if 1 - data exists
      next 5 bytes - rfid content
      next 16 bytes - description
  */
public:
  RFIDSpoofer(short Pin, Core *cr);
  //~RFIDSpoofer();
  void update();
  byte getID();
  char * getName();
  void drawGUI();
  void input_button(byte ID);
  void input_button_press(byte ID, unsigned long tm){}
private:
  /*
    Cache:
    Length - 14.
    byte on 0 position - is cache here(1 or 0).
    1 ... 5 - five bytes of card content(code).
    6 ... 13 - 8 bytes of compiled and builded data to write.
   */
  byte * cache;
  void generate_cache();
  void spoofnow();
  void send_manchester(int clock_half, int signal);

  short pin;
  int intEepromAddr;
  int extEepromAddr;
  byte status; // 0 - data doesn't exist, don't spoof
               // 1 - data exists, don't spoof
               // 2 - data exists, spoofing
};
#endif