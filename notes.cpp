#include "notes.h"


/*
Data in EEPROM structure:
Blocks start counting from third position.
Blocks count is in first 2 bytes of internal EEPROM


Each block have lenght (n+5):
0 - value 1, if there is block here
[1,2] - n
3     - if == 1, then block visible, if 0 => hidden
[4...n+2] - string data of note

*/

notesapp::notesapp(Core *cr):
App(cr),
intEepromAddr(notesID * InternalEEPROMlen),
extEepromAddr(notesExtrenalEEPROMID * ExternalEEPROMlen)
{
	NotesCount = core->readIntIntEEPROM(intEepromAddr);
	//core->writeIntIntEEPROM(0, intEepromAddr);
	Serial.println(NotesCount);

	notesCache = new char *[notePreviewCount];
	for(byte i = 0; i<notePreviewCount; i++)
		notesCache[i] = new char[notePreviewLen];
	buildCache();
	//clearCache();
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
	delay(1000);
}


void notesapp::clearCache()
{
	for(byte i = 0; i<notePreviewCount; i++)
	{
		for(byte j = 0; j<notePreviewLen; j++)
		notesCache[i][j] = ' ';
	}
}
void notesapp::buildCache()
{
	clearCache();
	if(NotesCount <= 0)
		return;
	if(selectedNote < 0)
		selectedNote = 0;
	if(selectedNote >= NotesCount)
		selectedNote = NotesCount - 1;

	Serial.println(selectedNote);
	unsigned int s = selectedNote, e = selectedNote;
	byte cnt = 1;
	bool side = false;
	selectedPreview = 0;
	while(cnt < notePreviewCount && cnt < NotesCount)
	{
		if(s > 0 && !side)
		{
			s--;
			Serial.print("s: "); Serial.println(s);
			if(core->readExtEEPROM(get_note_pos(s) + 3) == 1) //only if visible
			{
				cnt++;
				Serial.print("s as: "); Serial.println(s);
				selectedPreview ++;
				side = true;
			}
		}
		else
			side = true;
		if(e < NotesCount - 1 && cnt < notePreviewCount && cnt < NotesCount && side)
		{
			e++;
			Serial.print("e: "); Serial.println(e);
			if(core->readExtEEPROM(get_note_pos(e) + 3) == 1) //only if visible
			{
				cnt++;
				Serial.print("e as: "); Serial.println(e);
				side = true;
			}
		}
		else
			side = false;
	}
	unsigned int pos, len;
	cnt = 0; //current cache string
	for (unsigned int i = s; i <= e; ++i)
	{
		pos = get_note_pos(i);
		if(core->readExtEEPROM(pos + 3) == 1)
		{
			len = core->readIntExtEEPROM(pos + 1);
			//utoa(i, notesCache[cnt], 10);
			byte sr = 0;
			for(byte j = 0; j < notePreviewLen && j < len; j++)
			{
				//if(notesCache[cnt][j] == ' ')
					notesCache[cnt][j] = core->readExtEEPROM(pos + 4 + j);
				//else
				//{
				///	if(notesCache[cnt][j] == '\0')
				//		notesCache[cnt][j] = '.';
				//	sr ++;
				//}
			}
		}
	}
}

void notesapp::drawGUI()
{
	if(NotesCount == 0)
		return;
	for(byte i = 0; i < notePreviewCount; i++)
	{
		core->u8g->setPrintPos(0, 20 +i*8);
		Serial.println(notesCache[i]);
		core->u8g->print(notesCache[i]);
	}
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
	if(comm[0] == 'a')
	{
		if(commc >= 2 && commln[0] == 1)
		{
			String s = "";
			int p = 1;
			for(int i = 1; i < commc; i++)
			{
				for(int j = 0; j<commln[i]; j++)
				{
					s+=String(comm[p]);
					p++;
				}
				s+=" ";
			}
			add_new_note(s);
		}
	}
}
String notesapp::get_note(unsigned int ID)
{

}
unsigned int notesapp::add_new_note(String s)
{
	Serial.println("Adding string " + s);
	unsigned int pos = 3 + extEepromAddr;
	NotesCount++;
	if(NotesCount > 1)
	{
		pos = get_note_pos(NotesCount-2);
		pos += core->readIntExtEEPROM(pos + 1) + 3;
	}
	core->writeExtEEPROM(1, pos);
	core->writeIntExtEEPROM(s.length(), pos + 1);
	core->writeExtEEPROM(1, pos + 3);
	for(int i = 0; i < s.length(); i++)
	{
		core->writeExtEEPROM(s[i], i + pos + 4);
	}
	core->writeIntIntEEPROM(NotesCount, intEepromAddr);
	return 1;
}
unsigned int notesapp::get_note_pos(unsigned int ID)
{
	int cur_pos = extEepromAddr + 3;
	int id = 0;

	if(ID >= NotesCount)
	{
		return 0;
	}

	while(1)
	{
		if(core->readExtEEPROM(cur_pos) == 1)
		{
			if(id == ID)
				return cur_pos;
			else
			{
				cur_pos += core->readIntExtEEPROM(cur_pos + 1) + 3;
			}
			id++;
		}
		else
		{
			return 0;
		}
	}
}