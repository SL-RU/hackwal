#include <EEPROM.h>
#include <U8glib.h>
#include <Wire.h>
#include "core.h"
#include "spoofer.h"
#include <Eeprom24C32_64.h>
#include "leOS2.h"
// Define the Arduino PIN 
#define coil_pin 9

#define keypin1 7
#define keypin2 6
#define buttons_levels_count 6
static short buttons_levels[] = {150, 300, 420, 550, 700, 850};
unsigned long buttons2_stateST = 0, buttons1_stateST = 0;
byte buttons1_state = 255, buttons2_state = 255;

leOS2 os;
Core * core;

void setup() { 
  Serial.begin(9600);
  while(!Serial);
  

  os.begin();

  core = new Core(); 
  core->start_app(3);

  os.addTask(buttons_update, os.convertMs(10));
  os.addTask(readSerial, os.convertMs(50));

  Serial.println("Started");
}

char *_comm;
byte *_comlen;
byte _comn, _curc, curn;
int _ln;
void readSerial()
{
	while (Serial.available() > 0) 
	{
		char inChar = Serial.read();
		if(_comn != 0)
		{
			if(_ln <= 0)
			{
				_comlen[curn] = byte(inChar);
				_ln -= _comlen[curn];
				curn ++;
				if(curn == _comn)
				{
					_ln *= -1;
					curn = 0;
					_comm = new char[_ln];
				}
			}
			else
			{
				_comm[curn] = inChar;
				curn++;
				if(curn == _ln)
				{
					//send comm
					core->input_command(_comm, _comlen, _comn, _ln);

					delete _comm;
					delete _comlen;
					_ln = 0;
					_comn = 0;
				}
			}
		}
		else
		{
			_comn = (byte(inChar));
			_comlen = new byte[_comn];
			_curc = 0;
			curn = 0;
			_ln = 0;
		}
	}
}

void loop() 
{
  //readSerial();
  core->update();
  //buttons_update();
  delay(50);
  //core->update();
}

byte what_diap(short val)
{
	for(byte i = 0; i< buttons_levels_count; i++)
	{
		if(val > buttons_levels[i] - 50 && val < buttons_levels[i] + 50)
		{
			return i;
		}
	}
	return 255;
}
void buttons_update()
{
	unsigned long mil = millis();
	short v = analogRead(keypin1);
	if(v > 50)
	{
		byte z = what_diap(v);
		if(z != 255)
		{
			if(buttons1_state != z)
			{
				buttons1_state = z;
				buttons1_stateST = mil;
			}
		}
		else
		{
			buttons1_state = 255;
			buttons1_stateST = 0;
		}
	}
	else
	{
		if(buttons1_stateST != 0 &&  buttons1_state != 255)
		{
			if(mil - buttons1_stateST > 25)
			{
				core->input_button(buttons1_state*3);
			}
		}
		buttons1_state = 255;
		buttons1_stateST = 0;
	}

	v = analogRead(keypin2);
	if(v > 50)
	{
		byte z = what_diap(v);
		if(z != 255)
		{
			if(buttons2_state != z)
			{
				buttons2_state = z;
				buttons2_stateST = mil;
			}
		}
		else
		{
			buttons2_state = 255;
			buttons2_stateST = 0;
		}
	}
	else
	{
		if(buttons2_stateST != 0 &&  buttons2_state != 255)
		{
			if(mil - buttons2_stateST > 25)
			{
				if(buttons2_state < 3)
					core->input_button(buttons2_state*3 + 1);
				else
					core->input_button((5 - buttons2_state) * 3 + 2);
			}
		}
		buttons2_state = 255;
		buttons2_stateST = 0;
	}
}
void core_update()
{
	core->update();
}