#!/usr/bin/python

import socket
from singleLed import Led
from time import sleep

print "MIDI to LED"

# open UDP socket to local raveloxmidi
socket = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
socket.bind( ('', 5010 ) )
socket.settimeout(0.1)

led = Led(19,21,23);

ison = False

# loop infinitely
while True:
	try:
		data, addr = socket.recvfrom(1024)
		print ":".join(format(ord(c)) for c in data)

		if ison:
			led.setColor(100,100,100)
		else:
			led.setColor(0,0,0)

		ison = not ison

	except Exception:
		pass

	#print ":".join("{0:x}".format(ord(c)) for c in data)
