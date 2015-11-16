#include "templateapp.h"


templateapp::templateapp(Core *cr):
App(cr),
intEepromAddr(templateappID * 20),
extEepromAddr(templateappExtrenalEEPROMID * 1024)
{
}

templateapp::~templateapp()
{
}

byte templateapp::getID()
{
	return templateappID;
}


void templateapp::update()
{
}

void templateapp::drawGUI()
{
}

void templateapp::input_button(byte ID)
{
}

void templateapp::print_all_commands()
{
	Serial.print("""h\t\tHello""");
}
void templateapp::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] == 'd')
	{
		Serial.println("Hello");
		core->showMessage("Hello");
	}
}
