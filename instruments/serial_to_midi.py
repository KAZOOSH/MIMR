#!/usr/bin/python

import sys
from time import sleep
from serial import Serial
import socket
import struct

print "Serial to MIDI"

# open serial port to Arduino
serial = Serial( "/dev/ttyACM0", 115200, bytesize=8, parity='N', timeout=1 )

# open UDP socket to local raveloxmidi
socket = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
socket.connect( ( "localhost", 5006 ) )


# initialize old value for change detection
oldvalue = 0

#connect the led
led = Led(19,21,23);

# loop infinitely
while True:

	# try to read value from Arduino
	value = serial.readline().strip()

	# value available and different from old one?
	if value and value != oldvalue:

		# limit to 0..127
		value = max( min( int(value), 127 ), 0 )

		# update cached value
		oldvalue = value

		# log current value
		sys.stdout.write( "%d   \r" % value )
		sys.stdout.flush()

		# on MIDI channel 1, set controller #1 to value
		bytes = struct.pack( "BBBB", 0xaa, 0xB1, 1, value )
		socket.send( bytes )
