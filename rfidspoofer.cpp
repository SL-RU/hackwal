#include "rfidspoofer.h"

inline byte wrtbit(byte bt, byte pos, bool val) {if(val) bt |= _BV(pos); else bt &= ~_BV(pos); return bt;}
inline byte readbit(byte bt, byte pos){ return (bt & _BV(pos)) >> pos; }

rfidspoofer::rfidspoofer(Core * cr):
App(cr),
cache(new byte[32]),
pin(9), //Set pin
intEepromAddr(rfidspooferID * InternalEEPROMlen),
extEepromAddr(rfidspooferExtrenalEEPROMID * ExternalEEPROMlen),
status(0)
{
	digitalWrite(pin, OUTPUT);
	cache[1] = 0x56;
	cache[2] = 0x5A;
	cache[3] = 0x05;
	cache[4] = 0x72;
	cache[5] = 0xD6;
	generate_cache();
}

byte rfidspoofer::getID()
{
	return rfidspooferID;
}

void rfidspoofer::drawGUI()
{
	core->u8g->setPrintPos(0, 20);
	for (int i = 1; i < 2; i++)
	{
		core->u8g->print(F("123456789012345678901234567890"));
	}
}
void rfidspoofer::input_button(byte ID)
{

}

void rfidspoofer::spoofnow()
{
	//Serial.println("Data: ");
	for(int i = 0; i < 8; i++) 
	{
		for (int j = 0; j < 8; j++)
		{
			send_manchester(0, readbit(cache[6+i], j)); 
			delayMicroseconds(256);
			send_manchester(1, readbit(cache[6+i], j)); 
			delayMicroseconds(256);     
			//Serial.print(readbit(cache[6+i], j),DEC);
		}
	}
}
void rfidspoofer::send_manchester(int clock_half, int signal)
{
  // XOR the clock and the signal
	if(clock_half ^ signal) { 
		digitalWrite(pin, HIGH);
	} else {
		digitalWrite(pin, LOW);
	}
}
void rfidspoofer::update()
{
	//Serial.println("dsf");
	if(status == 2)
	{
	}
	spoofnow();
}
void rfidspoofer::generate_cache()
{
	byte i, j, k, crc_row = 0;

	byte crc_col[4] = {0 , 0, 0, 0};
    // the 9 first bit are 1 logic (a+9) 
	cache[6] = 0xFF; 
	cache[7] = 0xFF;
	j = 1;
	k = 1;

	for (i = 1; i <= 5; ++i)
	{
	  	for (int z = 7; z >= 4; z--) //first 4 bits
	  	{
	  		if(readbit(cache[i], z))
	  		{
	  			crc_row++;
	  			crc_col[7-z]++;
			}
	  		bitWrite(cache[6+k], j, readbit(cache[i], z));
	  		if (j == 7){j = 0; k ++; } else j++;
	  	}
	  	
	  	bitWrite(cache[6+k], j, crc_row % 2); //crc_row row (hash)
	  	crc_row = 0;
	  	if (j == 7){j = 0; k ++; } else j++;

	  	for (int z = 3; z >= 0; z--) //second 4 bits
	  	{
	  		if(readbit(cache[i], z))
	  		{
	  			crc_row++;
	  			crc_col[3 - z] ++;
	  		}
	  		bitWrite(cache[6+k], j, readbit(cache[i], z));
	  		if (j == 7){j = 0; k ++; } else j++;
	  	}

	  	bitWrite(cache[6+k], j, crc_row % 2); //crc_row row (hash)
	  	crc_row = 0;
	  	if (j == 7){j = 0; k ++; } else j++;
  }
  for (i = 0; i < 4; ++i)
  {
  	bitWrite(cache[6+k], j, crc_col[i] % 2);
  	if (j == 7){j = 0; k ++; } else j++;
  }
  bitWrite(cache[6+k], j, 0);
}