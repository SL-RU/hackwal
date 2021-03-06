#include <Wire.h>
#include <EEPROM.h>
#include <U8glib.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include "core.h"
#include "leOS2.h"
// Define the Arduino PIN 
/*
A0 - input voltage(before boost)
A1 - Vcc
A2 - keyboard line 1
A3 - keyboard line 2
A4,A5 - I2C
A6 - 
A7 - 
D1,D2 - rx,tx
D2,D3 software serial for RFID reader
D3
D4
D5
D6
D7
D8
D9
D10 - RFID spoofer
D11
D12
D13 - LED
*/

/*
Buttons:

0     1     2

3     4     5

6     7     8


System keys:
6 - app list

Normal use:
2 - show info about element
0 - edit element
8 - current app menu(help)
*/


#define coil_pin 9

#define keypin1 2
#define keypin2 3
#define buttons_levels_count 6
const short buttons_levels[] PROGMEM = {190, 370, 550, 745, 970, 1010};
unsigned long buttons2_stateST = 0, buttons1_stateST = 0;
byte buttons1_state = 255, buttons2_state = 255;

leOS2 os;
Core * core;

void core_update();
void buttons_update();
void readSerial();

void setup() { 
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  while(!Serial);

  os.begin();

  core = new Core(); 
  core->start_app();

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
  delay(10);
  //core->update();
}

byte what_diap(short val)
{
	for(byte i = 0; i< buttons_levels_count; i++)
	{
		if(val > pgm_read_word_near(buttons_levels + i) - 50 && val < pgm_read_word_near(buttons_levels + i) + 50)
		{
			return i;
		}
	}
	return 255;
}
unsigned long mil;
short v;
byte z;
void buttons_update()
{
//	if(lcdON)

	mil += 11;
	v = analogRead(keypin1);

	if(((unsigned long)(mil/2000))%2 == 0)
		core->mesure_battery();
	core->mesure_battery();
	if(v > 50)
	{
		digitalWrite(13, 1);
		z = what_diap(v);
		if(z != 255)
		{
			if(buttons1_state != z)
			{
				core->input_button_press(buttons1_state*3 + 2, 0);
				buttons1_state = z;
				buttons1_stateST = mil;
			}
			else
			{
				core->input_button_press(z*3 + 2, mil - buttons1_stateST);
			}
		}
		else
		{
			buttons1_state = 255;
			buttons1_stateST = 0;
			if(buttons1_state != 255)
				core->input_button_press(buttons1_state*3 + 2, 0);
		}
		return;
	}
	else
	{
		if(buttons1_stateST != 0 &&  buttons1_state != 255)
		{
			digitalWrite(13, 0);
			if(mil - buttons1_stateST > 25)
			{
				core->input_button(buttons1_state*3 + 2);
				core->input_button_press(buttons1_state*3 + 2, 0);
			}
		}
		buttons1_state = 255;
		buttons1_stateST = 0;
	}

	v = analogRead(keypin2);
	if(v > 50)
	{
		digitalWrite(13, 1);
		z = what_diap(v);
		if(z != 255)
		{
			if(buttons2_state != z)
			{
				if(buttons2_state < 3)
					core->input_button_press(buttons2_state*3 + 1, 0);
				else
					core->input_button_press((5 - buttons2_state) * 3, 0);
				buttons2_state = z;
				buttons2_stateST = mil;
			}
			else
			{
				if(buttons2_state < 3)
					core->input_button_press(buttons2_state*3 + 1, mil - buttons2_stateST);
				else
					core->input_button_press((5 - buttons2_state) * 3, mil - buttons2_stateST);
			}
		}
		else
		{
			if(buttons2_state != 255)
			{
				if(buttons2_state < 3)
					core->input_button_press(buttons2_state*3 + 1, 0);
				else
					core->input_button_press((5 - buttons2_state) * 3, 0);
			}
			buttons2_state = 255;
			buttons2_stateST = 0;
		}
	}
	else
	{
		if(buttons2_stateST != 0 &&  buttons2_state != 255)
		{
			Serial.println(mil);
			digitalWrite(13, 0);
			if(mil - buttons2_stateST > 25)
			{
				if(buttons2_state < 3)
				{
					core->input_button(buttons2_state*3 + 1);
					core->input_button_press(buttons2_state*3 + 1, 0);
				}
				else
				{
					core->input_button((5 - buttons2_state) * 3);
					core->input_button_press((5 - buttons2_state) * 3 , 0);
				}
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