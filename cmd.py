#!/usr/bin/python3

from tkinter import *
import serial
from serial.tools import list_ports
import time
import sys
from threading import Thread
from tkinter.scrolledtext import ScrolledText

root=Tk()

prt = list()
prts = list_ports.comports()
for i in prts:
	if(i[0].startswith("/dev/ttyUSB")):
		prt.append(i)

if(len(prt) == 0):
	print("There isn't any arduino connected")
	sys.exit()

if(len(prt) == 1):
	prt = prt[0]
else:
	print("Choose required port:")
	n = 0
	for i in prt:
		print(str(n) + ")\t" + i[0] + " name: " + i[1])
		n+=1
	print("Enter number: ")
	while True:
		n = input()
		if n.isdigit():
			n = int(n)
		else:
			n = -1
		if((n >= 0 and n < len(prt))):
			break
		else:
			print("Wrong number. Print another:")

	prt = prt[n]

print(prt)
root.title(prt[0])

ser = serial.Serial(prt[0], baudrate=9600)


def parse_com(s):
	c = s.split(" ")
	o = chr(len(c))
	for i in c:
		o+= chr(len(i))
	for i in c:
		o+= i
	return o

inp = StringVar()
cons = StringVar()

def on_enter(ev):
	#print("ent")
	i = inp.get()
	if len(i) == 0:
		return 0
	s = parse_com(i)

	ser.write(str.encode(s))
	txt.insert(END, "\n> " + s + "\n")
	inp.set("")
	txt.see(END)


#inp.trace('w', on_input)
root.bind('<Return>', on_enter)

txt = ScrolledText(root)
txt.pack(expand=1, fill='both')
ent = Entry(root, textvariable=inp)
ent.pack(expand=1, fill='both', side='bottom')


def read_ser():
	while True:
		i = ser.read()
		#print(i)
		txt.insert(END, i)
		txt.see(END)


rd = Thread(target=read_ser)
rd.setDaemon(True)
rd.start()
ent.focus()
root.mainloop()