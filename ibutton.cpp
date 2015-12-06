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
curState(0),
addck(0),
startPreviewID(0),
selectedKey(0),
lstselectedKey(0),
curMenuShow(0),
writeResult(0)
{
	digitalWrite(13, 0);


	Data = new byte[8];
	for(byte i = 0; i<8; i++)
	{
		Data[i] = 0xFF;
	}

	//core->writeIntIntEEPROM(2, intEepromAddr);
	//delay(5);

	KeyCount = core->readIntIntEEPROM(intEepromAddr);

	previewCache = new char*[5];
	for (byte i = 0; i < 5; ++i)
	{
		previewCache[i] = new char[17];
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
	if(addck)
	{
		addck = false;
		add_cur_key();
		generate_cache();
	}
	if(lstselectedKey != selectedKey)
	{
		generate_cache();
		lstselectedKey = selectedKey;
	}
	if(curState == 1)
	{
		digitalWrite(13, 1);
  		byte i;
  		byte data[8];
  
  		ds.reset();
  		delay(50);
  		ds.write(0x33); // "READ" command
  
  		ds.read_bytes(data, 8);

  		if(!(data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF))
  		{
  			for(i = 0; i < 8; i++) {
  			Data[i] = data[i];
  			}
  			curState = 0;
  		}
  		digitalWrite(13, 0);
	}
	else if(curState == 2)
	{
		digitalWrite(13, 1);
  		byte i;
  		byte data[8];
  
  		ds.reset();
  		delay(50);
  		ds.write(0x33); // "READ" command
  
  		ds.read_bytes(data, 8);

  		if((Data[0] & Data[1] & Data[2] & Data[3] & Data[4] & Data[5] & Data[6] & Data[7] == 0xFF))
  		{
  			curState = 0;
  			writeResult = 2;
  			return;
  		}

  		if((data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF))
  		{
  			//for(byte i = 0; i<8; i++)
  			//{
  			//	if(data[i] != Data[i])
  			//		break;
  			//	else
  			//		if(i == 7)
  			//		{
  			//			writeResult = 1;
			//            curState = 0;
			//            return;
  			//		}
  			//}

  			for(byte i = 0; i<8; i++)
  			{
  				ds.reset();
  				data[0] = 0x3C; // "WRITE" command
   				data[1] = i; // programming i-th byte
    			data[2] = 0;
    			data[3] = Data[i];
    			ds.write_bytes(data, 4);

    			uint8_t crc = ds.read();
				if (OneWire::crc8(data, 4) != crc) 
				{
					if(i == 0)
						writeResult = 3;
					else
						writeResult = 4;
					digitalWrite(13, 0);
					return;
				}
  				pinMode(10, OUTPUT);
            	digitalWrite(10, HIGH); 
            	delay(60);
            	digitalWrite(10, LOW); 
            	delay(5);
            	digitalWrite(10, HIGH); 
            	delay(50); 
  			}
  			writeResult = 1;
  			curState = 0;
        }
  		digitalWrite(13, 0);
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
int ibutton::isCurInDB()
{
	byte eq = 0;
	for(int i = 0; i < KeyCount; i++)
	{
		while(eq < 8)
		{
			if(core->readExtEEPROM(extEepromAddr + i*32 + 1 + eq) == Data[eq])
				eq++;
			else
				break;
		}
		if(eq == 8)
			return i;
	}
	return -1;
}
void  ibutton::generate_cache()
{
	clear_cache();
	if(KeyCount <= 0)
		return;
	int e = selectedKey;
	byte cnt = 1, side = 0;
	startPreviewID = selectedKey;

	while(cnt < 5 && cnt < KeyCount && (startPreviewID > 0 || e < KeyCount - 1))
	{
		if(startPreviewID - 1 >= 0 && side)
		{
			startPreviewID--;
			//if(core->readExtEEPROM(extEepromAddr + startPreviewID*32 + 9))
			//{
				cnt++;
				side = 0;
			//}
		}
		else
			side = 0;

		if(e + 1 < KeyCount && !side)
		{
			e++;
			//if(core->readExtEEPROM(extEepromAddr + e*32 + 9))
			//{
				cnt ++;
				side = 1;
			//}
		}
		else
			side = 1;
	}

	cnt = 0;
	for(int i = startPreviewID; i<=e; i++)
	{
		//if(core->readExtEEPROM(extEepromAddr + i*32 + 9))
		//{
			for(side = 0; side < 17; side ++)
				previewCache[cnt][side] = (char)core->readExtEEPROM(extEepromAddr + i*32 + 10+side);
			cnt++;
		//}
	}
}
void  ibutton::clear_cache()
{
	for (byte i = 0; i < 5; ++i)
		for(byte j = 0; i<17;i++)
			previewCache[i][j] = ' ';
}

void ibutton::drawGUI()
{
	core->u8g->setPrintPos(50, 8);
	if(curState == 1)
		core->u8g->print("r");
	else if(curState == 2)
		core->u8g->print("w");

	if(curMenuShow == 1)
	{
		drawreadMenu();
		return;
	}else if(curMenuShow == 2)
	{
		drawwriteMenu();
		return;
	}

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
			core->u8g->print(previewCache[i]);
	}
	else
	{
		core->u8g->setPrintPos(10, 18);
		core->u8g->print("empty...");
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

void ibutton::drawreadMenu()
{
	core->u8g->setColorIndex(0);
	core->u8g->drawBox(8, 10, 112, 53);	
	core->u8g->setColorIndex(1);
	core->u8g->drawFrame(8, 10, 112, 53);

	if(curState == 1)
	{
		core->u8g->setPrintPos(10, 20);
		core->u8g->print("rding...");
		core->u8g->setPrintPos(10, 61);
		core->u8g->print("3 - cancel");
	}
	else
	{
		core->u8g->setPrintPos(10, 20);
		for(byte i = 0; i < 8; i++) 
		{
			if(Data[i] < 0x10)
				core->u8g->print('0');
    		core->u8g->print(Data[i], HEX);
    		if (i==3) core->u8g->print(":");
  		}
  		core->u8g->setPrintPos(10, 30);
  		int k = isCurInDB();
  		if(k >= 0)
  		{
			core->u8g->print(F("in db: ID="));
			core->u8g->print(k);
			core->u8g->setPrintPos(10, 39);
			for(byte b = 0; b < 18; b++)
			{
				core->u8g->print((char)core->readExtEEPROM(extEepromAddr + k*32 + 10 + b));
			}
  		}
  		else
  		{
  			core->u8g->print(F("not fnded"));
  		}

		core->u8g->setPrintPos(10, 50);
		core->u8g->print(F("4 - add as new"));
		core->u8g->setPrintPos(10, 61);
		core->u8g->print("3 - close");
	}
}

void ibutton::drawwriteMenu()
{
	core->u8g->setColorIndex(0);
	core->u8g->drawBox(8, 10, 112, 53);	
	core->u8g->setColorIndex(1);
	core->u8g->drawFrame(8, 10, 112, 53);

	core->u8g->setPrintPos(10, 20);
	if(curState == 2)
	{
		core->u8g->print(F("wrting..."));
		core->u8g->setPrintPos(10, 61);
		core->u8g->print("3 - cancel");
	}
	core->u8g->setPrintPos(10, 30);
		if(writeResult == 1)
			core->u8g->print(F("DONE!"));
		else if (writeResult == 2)
			core->u8g->print(F("Dn't wrt FF key!11"));
		else if (writeResult == 3)
			core->u8g->print(F("key dsn't rewritable"));
		else if (writeResult == 4)
			core->u8g->print(F("error while writing"));

}
void ibutton::input_button(byte ID)
{
	if(curMenuShow == 1)
	{
		if(ID == 3)
		{
			curState = 0;
			curMenuShow = 0;
		}
		if(ID == 4)
		{
			addck = true;
			curState = 0;
			curMenuShow = 0;

		}
		return;
	}
	else if (curMenuShow == 2)
	{
		curState = 0;
		curMenuShow = 0;
		writeResult = 0;
		return;
	}

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
	{
		curState = 1;
		curMenuShow = 1;
	}

	if(ID == 5)
	{
		curState = 2;
		curMenuShow = 2;
	}

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
	//Serial.print("""h\t\tHello""");
}
void ibutton::input_command(char * comm, byte * commln, int commc, int len)
{
	if(comm[0] == 'r')
	{
		core->writeIntIntEEPROM(1, intEepromAddr);
		KeyCount = 1;
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
			if(p > 1+commln[1] + 22)
				return;
			s+=" ";
		}
		Serial.print(s);
		Serial.print(" -> ");
		Serial.println(id);
		set_key_note(id, s);
		generate_cache();
	}
}

