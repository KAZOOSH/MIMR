#!/usr/bin/python

import sys
from time import sleep
from serial import Serial
import socket
import struct

def is_set(x, n):
    return x & 2**n != 0

def checkSendValue(valueIn,valueSaved,id):
	if valueSaved != is_set(int(valueIn),id):
		# on MIDI channel 1, set controller #2 to value
		valueSaved = is_set(int(valueIn),id);
		send = 0 if valueSaved == False else 127
		bytes = struct.pack( "BBBB", 0xaa, 0xB1, id+2, send )
		socket.send( bytes )
		return valueSaved

print "Der Kuehler"

# open serial port to Arduino
serial = Serial( "/dev/ttyACM0", 115200, bytesize=8, parity='N', timeout=0.2 )

# open UDP socket to local raveloxmidi
socket = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
socket.connect( ( "localhost", 5006 ) )


# initialize old value for change detection
oldvalue = 0
value1 = False;
value2 = False;
value3 = False;

# loop infinitely
while True:
	# try to read value from Arduino
	value = serial.readline().strip()

	# value available and different from old one?
	if value and value != oldvalue:

		value1 = checkSendValue(value,value1,0)
		value2 = checkSendValue(value,value2,1)
		value3 = checkSendValue(value,value3,2)

		# update cached value
		oldvalue = value
