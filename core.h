#ifndef CORE_DEF
#define CORE_DEF
#include "Arduino.h"
#include <Wire.h>
#include <EEPROM.h>
#include <U8gl.h>


#define EEPROM_ADDRESS  0x50
#define InternalEEPROMlen 20
#define ExternalEEPROMlen 1024

//void *operator new( size_t size );
//void *operator new[]( size_t size );
//void operator delete( void *ptr );
//void operator delete[]( void *ptr );

#define AppCount 5
/*
App:
1 - rfid spoofer
2 - test
3 - sysinf
4 - ibutton
5 - rfid spoofer
//4 - ping pong
//7 - notes
extEEPROM:
[0; 9] - rfid spoofer & rfid reader
[10; 19] - ibutton
[20; 90] - notes
*/

class App;

class Core
{
public:
	Core();
	App * getCurApp();
	App * setCurApp(App * app);
	const __FlashStringHelper * getAppInfo(byte id);
	char* getAppName(byte id);

	void print_apps();
	void start_app(byte id);
	void start_app();

	void writeIntEEPROM(byte * b, int len, int ID);
	byte readIntEEPROM(int ID);
	void writeIntEEPROM(byte b, int ID);
	byte * readIntEEPROM(int len, int ID);
	void writeExtEEPROM(byte b, unsigned int ID);
	byte readExtEEPROM(int unsigned ID);
	void writeIntIntEEPROM(unsigned b, unsigned int ID);
	unsigned int readIntIntEEPROM(unsigned int ID);
	void writeIntExtEEPROM(unsigned int b, unsigned int ID);
	unsigned int readIntExtEEPROM(unsigned int ID);

	void update();
	void drawGUI();

	void input_button(byte b);
	void input_button_press(byte b, unsigned long tm);
	
	void print_all_commands();
	void input_command(char * comm, byte * commln, int commc, int len); //comm - массив длинной len со всеми символами комманд
																		//commln - массив длинной commc где каждый байт - длинна комманды в массиве comm
	void showMessage(String msg);
	void showMenu();
	void closeMenu();

	void ResetArd();
	int freeRam();

	byte getBatteryState(); //battery state. If val less, then 101, then - percents.

	void mesure_battery();

	U8GLIB_SSD1306_128X64 * u8g;
	byte lcdON;
private:
	App * curApp;
	String GUImessage;
	byte GUImenu;
	byte selectedMenuItem;
	byte batteryState;

	void drawMessage();
	void drawHeader();
	void drawMenuBG();
	void drawMenu();
	void run_app(byte id);
};


class App
{
public:
	App(Core * cr);
	virtual ~App(){}
	virtual byte getID() = 0;
	virtual char * getName();
	virtual void update() = 0;
	virtual void drawGUI() = 0;
	virtual void input_button(byte ID) = 0;
	virtual void input_button_press(byte ID, unsigned long tm) = 0;
	virtual void print_all_commands() = 0;
	virtual void input_command(char * comm, byte * commln, int commc, int len) = 0; //comm - массив длинной len со всеми символами комманд
																			        //commln - массив длинной commc где каждый байт - длинна комманды в массиве comm
protected:
	Core * core;
};
#endif