#include "ibutton.h"


/*
Data in EEPROM structure:
Blocks start counting from zero position.
Blocks count is in first 2 bytes of internal EEPROM


Each block have lenght 32:
0 - value 1, if there is block here
[1, 8] - key
9      - if == 1, then block visible, if 0 => hidden !!!!!!! DO NOT HIDE KEYS!11 NOT IMPLEMENTED YET!11 
[10...30] - string data of note

*/


ibutton::ibutton(Core *cr):
App(cr),
intEepromAddr(ibuttonID * InternalEEPROMlen),
extEepromAddr(ibuttonExtrenalEEPROMID * ExternalEEPROMlen),
ds(10),
read(0),
write(0),
addck(0),
startPreviewID(0),
selectedKey(0),
lstselectedKey(0)
{
	pinMode(13, OUTPUT);
	digitalWrite(13, 0);


	Data = new byte[8];
	for(byte i = 0; i<8; i++)
	{
		Data[i] = 0xFF;
	}

	//core->writeIntIntEEPROM(0, intEepromAddr);
	//delay(5);

	KeyCount = core->readIntIntEEPROM(intEepromAddr);

	previewCache = new char*[5];
	for (byte i = 0; i < 5; ++i)
	{
		previewCache[i] = new char[21];
	}
	generate_cache();
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
	if(addck)
	{
		add_cur_key();
		generate_cache();
		addck = false;
	}
	if(lstselectedKey != selectedKey)
	{
		generate_cache();
		lstselectedKey = selectedKey;
	}
}

int   ibutton::add_cur_key()
{
	core->writeExtEEPROM(1, extEepromAddr + KeyCount*32);
	for(byte i = 0; i<8; i++)
	{
		core->writeExtEEPROM(Data[i], extEepromAddr + KeyCount*32 + 1 + i);
	}
	core->writeExtEEPROM(1, extEepromAddr + KeyCount*32 + 9);

	for(byte i = 10; i<32; i++)
	{
		core->writeExtEEPROM((byte)' ', extEepromAddr + KeyCount*32 + i);
	}

	KeyCount ++;
	core->writeIntIntEEPROM(KeyCount, intEepromAddr);
}
byte* ibutton::get_key(int id)
{
	if(id < 0 || id >= KeyCount)
		return 0;

	byte* k = new byte[8];
	for(byte i = 0; i<8; i++)
	{
		k[i] = core->readExtEEPROM(extEepromAddr + id*32 + 1 + i);
	}
	return k;
}
char* ibutton::get_key_note(int id)
{
	if(id < 0 || id >= KeyCount)
		return 0;

	char* ch = new char[21];
	for(byte i = 0; i<21; i++)
	{
		ch[i] = core->readExtEEPROM(extEepromAddr + id*32 + 10 + i);
	}
	return ch;
}
void  ibutton::set_key_note(int id, String note)
{
	if(id < 0 || id >= KeyCount)
		return;

	for(byte i = 0; i<21; i++)
	{
		if(i < note.length())
			core->writeExtEEPROM((byte)note[i], extEepromAddr + id*32 + 10 + i);
		else
			core->writeExtEEPROM((byte)' ', extEepromAddr + id*32 + 10 + i);
	}
}
void  ibutton::generate_cache()
{
	clear_cache();
	if(KeyCount <= 0)
		return;
	int e = selectedKey;
	byte cnt = 1, side = 0;
	startPreviewID = selectedKey;

	while(cnt < 5 && cnt < KeyCount && (startPreviewID > 0 || e < KeyCount))
	{
		if(startPreviewID - 1 >= 0 && side)
		{
			startPreviewID--;
			if(core->readExtEEPROM(extEepromAddr + startPreviewID*32 + 9))
			{
				cnt++;
				side = 0;
			}
		}
		else
			side = 0;

		if(e + 1 < KeyCount && !side)
		{
			e++;
			if(core->readExtEEPROM(extEepromAddr + e*32 + 9))
			{
				cnt ++;
				side = 1;
			}
		}
		else
			side = 1;
	}

	cnt = 0;
	for(int i = startPreviewID; i<=e; i++)
	{
		if(core->readExtEEPROM(extEepromAddr + i*32 + 9))
		{
			for(side = 0; side < 21; side ++)
				previewCache[cnt][side] = (char)core->readExtEEPROM(extEepromAddr + i*32 + 10+side);
			cnt++;
		}
	}
}
void  ibutton::clear_cache()
{
	for (byte i = 0; i < 5; ++i)
		for(byte j = 0; i<21;i++)
			previewCache[i][j] = ' ';
}

void ibutton::drawGUI()
{
	core->u8g->setPrintPos(50, 10);
	if(read)
		core->u8g->print("r");
	if(write)
		core->u8g->print("w");

	if(KeyCount > 0)
	for(byte i = 0; i<5; i++)
	{
		if(i+startPreviewID >= KeyCount)
			break;
		core->u8g->setPrintPos(0, 20 + 8*i);
		if(i+startPreviewID == selectedKey)
		{
			core->u8g->drawLine(0, 12 + 8*i, 0, 20+ 8*i);
		}
		core->u8g->print(i+startPreviewID);
		core->u8g->print('.');
		for(byte j = 0; j < 21; j++)
		{
			core->u8g->print(previewCache[i][j]);
		}
	}

	core->u8g->setPrintPos(0, 63);
	for(byte i = 0; i < 8; i++) 
	{
		if(Data[i] < 0x10)
			core->u8g->print('0');
    	core->u8g->print(Data[i], HEX);
    	if (i==3) core->u8g->print(":");
  	}

}

void ibutton::input_button(byte ID)
{
	if(ID == 4)
	{
		if(KeyCount <= 0)
			return;

		for(byte i = 0; i<8; i++)
		{
			Data[i] = core->readExtEEPROM(extEepromAddr + selectedKey*32 + 1 + i);
		}
	}

	if(ID == 3)
		read = true;

	if(ID == 5)
		addck = true;

	if(ID == 1)
	{
		selectedKey --;
		if(selectedKey < 0)
			selectedKey = 0;
	}
	if(ID == 7)
	{
		selectedKey ++;
		if(selectedKey >= KeyCount)
			selectedKey = KeyCount - 1;
	}
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
	if(comm[0] == 'r')
	{
		core->writeIntIntEEPROM(0, intEepromAddr);
		KeyCount = 0;
	}
	if(comm[0] == 's' && commc >= 3 && commln[0] == 1)
	{
		String s = "";
		int id = 0;
		String n;
		for (int i = 1; i < 1 + commln[1]; i++)
		{
			s+=(char)comm[i];
		}
		id = s.toInt();

		s = "";
		int p = 1+commln[1];
		for(int i = 1+commln[1]; i < commc; i++)
		{
			for(int j = 0; j<commln[i]; j++)
			{
				s+=String(comm[p]);
				p++;
			}
			s+=" ";
		}
		Serial.print(s);
		Serial.print(" -> ");
		Serial.println(id);
		set_key_note(id, s);
		generate_cache();
	}
}

