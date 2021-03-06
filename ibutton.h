#ifndef ibuttonDEF
#define ibuttonDEF
#include "Arduino.h"
#include <EEPROM.h>
#include <OneWire.h>
#include "core.h"

#define ibuttonID 4
#define ibuttonName "ibut"
#define ibuttonDesc "This is app for reading/writing ibutton keys"


#define ibuttonExtrenalEEPROMID 10
//len = 10

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
  

  int add_cur_key();
  void set_key_note(int id, String note);
  int isCurInDB(); //if db contains key - return it's ID, else - (-1)

  void generate_cache();
  void clear_cache();

private:
  int intEepromAddr;
  int extEepromAddr;

  int KeyCount;

  int selectedKey, lstselectedKey;

  char** previewCache;
  int startPreviewID;
  
  byte* Data;
  
  OneWire ds;
  byte curState, addck;

  byte curMenuShow, 
      writeResult; //1 - success
                   //2 - All zeros or all FF
                   //3 - key don't rewritable
                   //4 - Error while writing


  void drawreadMenu();
  void drawwriteMenu();
};
#endif