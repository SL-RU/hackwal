#include "pingpong.h"


pingpong::pingpong(Core *cr):
App(cr),
intEepromAddr(pingpongID * InternalEEPROMlen),
x(0),
y(0),
lastTm(0),
lastId(255),
w(10),
h(10)
{
}

//pingpong::~pingpong()
//{
//}

byte pingpong::getID()
{
	return pingpongID;
}


void pingpong::update()
{
}

void pingpong::drawGUI()
{
	core->u8g->drawBox(x, y+10, w, h);
}

void pingpong::input_button(byte ID)
{
}
void pingpong::input_button_press(byte ID, unsigned long tm)
{
	if(ID == 4)
	{
		if(tm == 0)
			w = h = 10;
		else
			w = h =	(int)((tm*5.0)/1000.0);
		return;
	}
	if(lastId != 255)
	{
		if(lastId == ID)
		{
			if(tm == 0)
			{
				lastId = 255;
				lastTm = 0;
			}
			else
			{
				int val = (tm - lastTm)/10;
				if(val < 1)
					return;
				else
					lastTm = tm;
				if(ID == 1)
					y -= val;
				else if(ID == 7)
					y+= val;
				else if(ID == 5)
					x+=val;
				else if(ID == 3)
					x-= val;
			}
		}
	}
	else
	{
		if(tm != 0)
		{
			lastId = ID;
			lastTm = tm;
		}
	}
}

void pingpong::print_all_commands()
{
	Serial.print("""none""");
}
void pingpong::input_command(char * comm, byte * commln, int commc, int len)
{
}
