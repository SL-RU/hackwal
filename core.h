#ifndef CORE_DEF
#define CORE_DEF
#include "Arduino.h"
#include <Wire.h>
#include <Eeprom24C32_64.h>
#include <EEPROM.h>
#include <U8glib.h>


#define EEPROM_ADDRESS  0x50
#define InternalEEPROMlen 100
#define ExternalEEPROMlen 1000


#define AppCount 3

class App;

class Core
{
public:
	Core();
	App * getCurApp();
	App * setCurApp(App * app);
	char * getAppInfo(byte id);
	char *getAppName(byte id);
	void print_apps();
	void start_app(int id);
	void writeIntEEPROM(byte * b, int len, int ID);
	byte * readIntEEPROM(int len, int ID);
	void writeExtEEPROM(byte b, word ID);
	byte readExtEEPROM(word ID);
	void update();
	void drawGUI();
	void input_button(int b);
	void print_all_commands();
	void input_command(char * comm, byte * commln, int commc, int len); //comm - массив длинной len со всеми символами комманд
																		//commln - массив длинной commc где каждый байт - длинна комманды в массиве comm
	void showMessage(String msg);
	void showMenu();
	void closeMenu();

	void ResetArd();
	int freeRam();

	U8GLIB_SSD1306_128X64 * u8g;
	Eeprom24C32_64 * extEeprom;
private:
	App * curApp;
	String GUImessage;
	byte GUImenu;
	byte selectedMenuItem;

	void drawMessage();
	void drawHeader();
	void drawMenuBG();
	void drawMenu();
};


class App
{
public:
	App(Core * cr);
	virtual byte getID() = 0;
	virtual char * getName();
	virtual void update() = 0;
	virtual void drawGUI() = 0;
	virtual void input_button(int ID) = 0;
	virtual void print_all_commands() = 0;
	virtual void input_command(char * comm, byte * commln, int commc, int len) = 0; //comm - массив длинной len со всеми символами комманд
																			        //commln - массив длинной commc где каждый байт - длинна комманды в массиве comm
protected:
	Core * core;
};
#endif