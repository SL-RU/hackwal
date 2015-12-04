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

  String get_note(unsigned int id);
  unsigned int add_new_note(String s);
private:
  int intEepromAddr;
  int extEepromAddr;

  unsigned int selectedNote = 0;

#define notePreviewLen 21
#define notePreviewCount 6
  char ** notesCache;
  void clearCache();
  byte selectedPreview = 0;
  void buildCache();

  unsigned int NotesCount = 0;
  unsigned int get_note_pos(unsigned int id);
};
#endif