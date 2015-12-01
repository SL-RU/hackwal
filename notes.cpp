#include "notes.h"


/*
Data in EEPROM structure:
Blocks start counting from zero position.
Blocks count is in first 4 bytes of internal EEPROM


Each block have lenght (n+5):
[0,3] - n
4     - if == 1, then block visible, if 0 => hidden
[5...n+4] - string data of note

*/

notesapp::notesapp(Core *cr):
App(cr),
intEepromAddr(notesID * InternalEEPROMlen),
extEepromAddr(notesExtrenalEEPROMID * ExternalEEPROMlen)
{
}

notesapp::~notesapp()
{
}

byte notesapp::getID()
{
	return notesID;
}


void notesapp::update()
{
}

void notesapp::drawGUI()
{
}

void notesapp::input_button(byte ID)
{
}

void notesapp::print_all_commands()
{
	Serial.print("""h\t\tHello""");
}
void notesapp::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] == 'd')
	{
		Serial.println("Hello");
		core->showMessage("Hello");
	}
}
