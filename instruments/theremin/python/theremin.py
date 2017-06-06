#!/usr/bin/python

import sys
from time import sleep
from serial import Serial
import socket
import struct

print "Serial to MIDI"

# open serial port to Arduino
serial = Serial( "/dev/ttyUSB0", 115200, bytesize=8, parity='N', timeout=1 )

# open UDP socket to local raveloxmidi
socket = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
socket.connect( ( "localhost", 5006 ) )


# initialize old value for change detection
oldvalue = 0

#connect the led
#led = Led(19,21,23);

# loop infinitely
while True:

	# try to read value from Arduino
	value = serial.readline().strip()

	if oldvalue == '*' and value != '*':
		bytes = struct.pack( "BBBB", 0xaa, 0xB1, 6, 0 )
		socket.send( bytes )
	if value == '*':
		# on MIDI channel 1, set controller #1 to value
		bytes = struct.pack( "BBBB", 0xaa, 0xB1, 6, 127 )
		socket.send( bytes )
		oldvalue = value

	# value available and different from old one?
	elif value and value != oldvalue:
		# limit to 0..127
		try:
			value = max( min( int(value), 127 ), 0 )

			# log current value
			sys.stdout.write( "%d   \r" % value )
			sys.stdout.flush()

			# on MIDI channel 1, set controller #1 to value
			bytes = struct.pack( "BBBB", 0xaa, 0xB1, 5, value )
			socket.send( bytes )

			# update cached value
			oldvalue = value
		except:
			continue
