#include "testapp.h"


testapp::testapp(Core *cr):
App(cr),
intEepromAddr(testappID * 100),
extEepromAddr(testappExtrenalEEPROMID * 1024)
{

	var = core->readExtEEPROM(extEepromAddr);
}

testapp::~testapp()
{
}

byte testapp::getID()
{
	return testappID;
}


void testapp::update()
{

}

void testapp::drawGUI()
{
	core->u8g->setPrintPos(0, 20);
	core->u8g->print(var);
	core->u8g->setPrintPos(0, 30);
	core->u8g->print(analogRead(6));
	core->u8g->setPrintPos(0, 40);
	core->u8g->print(analogRead(7));
	core->u8g->setPrintPos(0, 50);
	core->u8g->print(millis());
}

void testapp::input_button(int ID)
{
	var++;
	writeVar();
}
void testapp::writeVar()
{
	core->writeExtEEPROM(var, extEepromAddr);
}
void testapp::print_all_commands()
{
	Serial.print("""d\tReset var\n\
r\tRead var\n\
s *v\tSet var as v. Where 0 >= v <= 255""");
}
void testapp::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] == 'd')
	{
		var = 0;
		writeVar();
	}
	if(comm[0] == 'r')
	{
		Serial.println(var);
	}
	else if(comm[0] == 's')
	{
		if(commc == 2)
		{
			if(commln[0] == 1)
			{
				String n;
				for (int i = 1; i < 1 + commln[1]; i++)
				{
					n+=(char)comm[i];
				}
				var = n.toInt();
				writeVar();
			}
		}
	}
}
