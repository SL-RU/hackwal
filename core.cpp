#include "core.h"
#include "testapp.h"
#include "sysinf.h"
#include "pingpong.h"
#include "rfidreader.h"

void(* resetFunc) (void) = 0; //declare reset function @ address 0

Core::Core():
u8g(new U8GLIB_SSD1306_128X64(U8G_I2C_OPT_NO_ACK)),
extEeprom(new Eeprom24C32_64(EEPROM_ADDRESS)),
GUImessage(""),
GUImenu(0),
selectedMenuItem(0)
{
	curApp = 0;
	u8g->setFont(u8g_font_6x10);
	extEeprom->initialize();
	//EEPROM.write(1, 2);
	delay(3);
}

App * Core::getCurApp()
{
	return curApp;
}

App * Core::setCurApp(App * app)
{
	if(curApp != 0)
	{
		delete curApp;
	}
	curApp = app;
	return app;
}

char * Core::getAppInfo(byte id)
{
	if(id == 1)
		return "None";
	if(id == testappID)
		return "App for testing";
	if(id == sysinfID)
		return (char*)sysinfDesc;
	if(id == pingpongID)
		return (char*)pingpongDesc;
	if(id == rfidreaderID)
		return (char*)rfidreaderDesc;
}
char * Core::getAppName(byte id)
{
	if(id == 1)
		return ("Spoofer");
	if(id == testappID)
		return ("Test app");
	if(id == sysinfID)
		return (sysinfName);
	if(id == pingpongID)
		return (pingpongName);
	if(id == rfidreaderID)
		return (rfidreaderName);
}
void Core::print_apps()
{
	Serial.print("Apps:\n\
ID\tName\t\tDescr\n");
	for(byte i = 1; i<=AppCount; i++)
	{
		Serial.print(i);
		Serial.print("\t");
		Serial.print(getAppName(i));
		Serial.print("\t\t");
		Serial.println(getAppInfo(i));
	}
	Serial.println("--------");
}
void Core::run_app(byte id)
{
	if(id == 1)
	{
//		setCurApp
	}else if (id == testappID)
	{
		setCurApp(new testapp(this));		
	} else if (id == sysinfID)
	{
		setCurApp(new sysinf(this));
	}
	else if (id == pingpongID)
	{
		setCurApp(new pingpong(this));
	}
	else if (id == rfidreaderID)
	{
		setCurApp(new rfidreader(this));
	}
}
void Core::start_app(byte ID)
{
	EEPROM.write(1, ID);
	delay(3);
	resetFunc();
}
void Core::start_app()
{
	run_app(EEPROM.read(1));
}
void Core::writeIntEEPROM(byte * b, int len, int Pos)
{
	for(int i = 0; i<len; i++)
	{
		EEPROM.write(Pos + i, b[i]);
		delay(4);
	}
}
byte * Core::readIntEEPROM(int len, int Pos)
{
	byte * b = new byte[len];
	for(int i = 0; i<len; i++)
	{
		b[i] = EEPROM.read(Pos + i);
	}
}


void Core::writeExtEEPROM(byte b, word Pos)
{
	extEeprom->writeByte(Pos, b);
}
byte Core::readExtEEPROM(word Pos)
{
//	byte * b = new byte[len];
//	for(int i = 0; i<len; i++)
//	{
	return extEeprom->readByte(Pos);
//	}
}

void Core::update()
{
	if(curApp != 0)
	{
		curApp->update();
	}
	u8g->firstPage();
	do
	{
		if(GUImessage == "" && GUImenu == 0)
		{
			if(curApp != 0)
			{
				curApp->drawGUI();
			}
			drawHeader();
			drawGUI();
		} else if(GUImessage!="")//else
		{
			drawMessage();
		} 
		else 
		{
			drawMenu();
		}
	}
	while(u8g->nextPage());
}
void Core::input_button(byte b)
{
	if(GUImessage != "")
	{
		if(b == 4)
			GUImessage = "";
		return;
	}
	if(GUImenu != 0)
	{
		if(b == 6)
		{
			closeMenu();
		}
		if(b == 4)
		{
			if(curApp != 0)
				if(selectedMenuItem != curApp->getID())
					start_app(selectedMenuItem);
			else
				start_app(selectedMenuItem);
			closeMenu();
			}
		if(b == 1)
			if(selectedMenuItem - 1 <= 0)
				selectedMenuItem = 1;
			else
				selectedMenuItem --;
		if(b == 7)
			if(selectedMenuItem + 1 > AppCount)
				selectedMenuItem = AppCount;
			else
				selectedMenuItem ++;
		return;
	}
	if(b == 6)
	{
		showMenu();
		return;
	}
	if(curApp != 0)
	{
		curApp->input_button(b);
	}
}
void Core::input_button_press(byte b, unsigned long tm)
{
	if(curApp!=0)
	{
			curApp->input_button_press(b, tm);
	}
}



void Core::print_all_commands()
{
	Serial.print(F("System commands: \n\
!h\tPrint this help\n\
!n\tName of current app\n\
!i\tID of current app\n\
!r\tReset arduino\n\
!a\tGet all info about all apps\n\
!s $id\tStart app with requested id\n\
!m $msg\tShow message with $msg content"));
	if(curApp != 0)
	{
		Serial.print(F("Commands of app: "));
		Serial.println(curApp->getName());
		curApp->print_all_commands();
	}
	else
	{
		Serial.println(F("No application started now"));
	}
}
void Core::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] != '!')
	{
		if(curApp != 0)
		{
			curApp->input_command(comm, commln, commc, len);
		}
	}
	else
	{
		if(len>1)
		{
			if(comm[1] == 'h')
			{
				print_all_commands();
			}
			if(comm[1] == 'n')
			{
				if(curApp != 0)
				{
					Serial.println(curApp->getName());
				}
				else
				{
					Serial.println("none");
				}
			}
			if(comm[1] == 'i')
			{
				if(curApp != 0)
				{
					Serial.println(curApp->getID());
				}
				else
				{
					Serial.println("0");
				}
			}
			if(comm[1] == 'r')
			{
				resetFunc();
			}
			if(comm[1] == 's')
			{
				if(commln[1] != 0 && commln[0] == 2)
				{
					String n;
					for (int i = 2; i < 2 + commln[1]; i++)
					{
						n+=(char)comm[i];
					}
					start_app(n.toInt());
				}
			}
			if(comm[1] == 'a')
			{
				print_apps();
			}
			if(comm[1] == 'm')
			{
				if(commc >= 2 && commln[0] == 2)
				{
					String s = "";
					int p = 2;
					for(int i = 1; i < commc; i++)
					{
						for(int j = 0; j<commln[i]; j++)
						{
							s+=String(comm[p]);
							p++;
						}
						s+=" ";
					}
					showMessage(s);
				}
			}
		}
	}
}

void Core::drawGUI()
{
}
void Core::drawHeader()
{
	u8g->setPrintPos(0, 7);
	if(curApp != 0)
	{
		u8g->print(curApp->getName());
	}
	u8g->print("  ");
	u8g->print(freeRam());
	u8g->print("/2048");
	u8g->drawLine(0, 9, 128, 9);
}
void Core::drawMenuBG()
{
	u8g->setColorIndex(0);
	u8g->drawBox(16, 0, 96, 64);	
	u8g->setColorIndex(1);
	u8g->drawFrame(16, 0, 96, 64);
	u8g->drawFrame(18, 2, 92, 60);
}
void Core::drawMessage()
{
	drawMenuBG();
	byte ln = 0;
	String gm = String(GUImessage);
	while(gm.length() > 14)
	{
		u8g->setPrintPos(22, 11 + 8*ln);
		u8g->print(gm.substring(0, 14));
		gm.remove(0, 14);
		ln++;
	}
	u8g->setPrintPos(22, 11 + 8*ln);
	u8g->print(gm);
}
void Core::drawMenu()
{
	drawMenuBG();
	u8g->setPrintPos(22, 11);
	u8g->print("Select app:");
	for(byte i = 1; i<=AppCount; i++)
	{
		u8g->setPrintPos(22, 11 + 8*i);
		if(selectedMenuItem == i)
			u8g->print(">");
		u8g->print(getAppName(i));
	}
}

void Core::showMessage(String msg)
{
	GUImessage = msg;
}
void Core::showMenu()
{
	GUImenu = 1;
	if(curApp != 0)
		selectedMenuItem = curApp->getID();
	else
		selectedMenuItem = 1;
}
void Core::closeMenu()
{
	GUImenu = 0;
}
void Core::ResetArd()
{
	resetFunc();
}
int Core::freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

App::App(Core * cr):
core(cr)
{
}
char * App::getName()
{
	return core->getAppName(getID());
}
