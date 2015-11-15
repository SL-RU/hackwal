#include "sysinf.h"


sysinf::sysinf(Core *cr):
App(cr),
intEepromAddr(sysinfID * 100),
buttonsbuf("                     "),
selButton(0)
{
}

sysinf::~sysinf()
{
}

byte sysinf::getID()
{
	return sysinfID;
}

void sysinf::update()
{
}

void sysinf::drawGUI()
{
	core->u8g->setPrintPos(0, 20);
	core->u8g->print("RAM: ");
	core->u8g->print(core->freeRam());
	core->u8g->print("/2048");
	core->u8g->setPrintPos(0, 30);

	core->u8g->print("UPTIME: ");
	String s = String(millis());
	if(s.length() > 3)
	{
		String l = s.substring(s.length() - 3, s.length() - 1);
		s.remove(s.length() - 3, 3);
		s += "." + l;
	}
	core->u8g->print(s);
	core->u8g->print("s");
	core->u8g->setPrintPos(0, 64);
	core->u8g->print(buttonsbuf);
	drawButtons();
}
void sysinf::drawButtons()
{
	String nm[] = {
		"Reboot",
		"button",
		"run 2"
	};
	for(int i = 0; i<3; i++)
	{
		core->u8g->setPrintPos(2 + 42*i, 45);
		if(i == selButton)
		{
			core->u8g->setColorIndex(1);
			core->u8g->drawBox(42*i, 32, 42, 18);	
			core->u8g->setColorIndex(0);
			core->u8g->print(nm[i]);
		}
		else
		{
			core->u8g->setColorIndex(1);
			core->u8g->print(nm[i]);
			core->u8g->drawFrame(42*i, 32, 42, 18);	
		}
	}
	core->u8g->setColorIndex(1);
}

void sysinf::input_button(int ID)
{
	buttonsbuf = String(ID) + buttonsbuf;
	buttonsbuf.remove(buttonsbuf.length() - 1);

	if(ID == 3)
	{
		selButton--;
		if(selButton < 0)
			selButton = 0;
	}
	if(ID == 5)
	{
		selButton++;
		if(selButton >= 3)
			selButton = 2;
	}
	if(ID == 4)
	{
		if(selButton == 0)
		{
			core->ResetArd();
		}
		if(selButton == 1)
		{
			core->showMessage("HELLOOOOO!1111 =D");
		}
		if(selButton == 2)
		{
			core->start_app(2);
		}
	}
}

void sysinf::print_all_commands()
{
	Serial.print("none");
}
void sysinf::input_command(char * comm, byte * commln, int commc, int len)
{
}
