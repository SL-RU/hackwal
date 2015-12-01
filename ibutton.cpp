#include "ibutton.h"



ibutton::ibutton(Core *cr):
App(cr),
intEepromAddr(ibuttonID * InternalEEPROMlen),
extEepromAddr(ibuttonExtrenalEEPROMID * ExternalEEPROMlen),
ds(10),
read(0),
write(0)
{
	pinMode(13, OUTPUT);
	digitalWrite(13, 1);
}

ibutton::~ibutton()
{
}

byte ibutton::getID()
{
	return ibuttonID;
}


void ibutton::update()
{
	if(read)
	{
		digitalWrite(13, 1);
  		byte i;
  		byte data[8];
  
  		ds.reset();
  		delay(50);
  		ds.write(0x33); // "READ" command
  
  		ds.read_bytes(data, 8);

  		if((data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF))
  		{}else{
  			for(i = 0; i < 8; i++) {
  			Data[i] = data[i];
  			}
  			read = 0;
  		}
  		digitalWrite(13, 0);
	}
}

void ibutton::drawGUI()
{
	core->u8g->setPrintPos(0, 20);
	if(read)
		core->u8g->print("r");
	if(write)
		core->u8g->print("w");
	core->u8g->setPrintPos(0, 40);
	for(byte i = 0; i < 8; i++) {
    core->u8g->print(Data[i], HEX);
    if(i == 3) core->u8g->setPrintPos(0, 50);
    if (i != 7 && i != 3) core->u8g->print(":");
  	}

}

void ibutton::input_button(byte ID)
{
	if(ID == 4)
		read = 1;
}

void ibutton::print_all_commands()
{
	Serial.print("""h\t\tHello""");
}
void ibutton::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] == 'd')
	{
		Serial.println("Hello");
		core->showMessage("Hello");
	}
}
