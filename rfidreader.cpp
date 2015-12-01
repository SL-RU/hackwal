#include "rfidreader.h"

rfidreader::rfidreader(Core *cr):
App(cr),
intEepromAddr(rfidreaderID * InternalEEPROMlen),
extEepromAddr(rfidreaderExtrenalEEPROMID * ExternalEEPROMlen),
ser(3,2),
inp("")
{
	ser.begin(9600);
}


byte rfidreader::getID()
{
	return rfidreaderID;
}


void rfidreader::update()
{
	while(ser.available())
	{
		inp += String(ser.read());
	}
}

void rfidreader::drawGUI()
{
	core->u8g->setPrintPos(0,20);
	core->u8g->print("r");
	core->u8g->print(inp);
}

void rfidreader::input_button(byte ID)
{
}

void rfidreader::print_all_commands()
{
	Serial.print(F("""h\t\tHello"""));
}
void rfidreader::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] == 'd')
	{
		Serial.println(F("Hello"));
		core->showMessage(F("Hello"));
	}
}
