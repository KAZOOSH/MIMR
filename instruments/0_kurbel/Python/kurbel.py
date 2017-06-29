#!/usr/bin/python

import socket
import sys
from time import sleep
from serial import Serial

print "Kurbel Control"

'''init serial and network'''
# open serial port to Arduino
serial = Serial( "/dev/ttyUSB0", 9600, bytesize=8, parity='N', timeout=1 )


# open UDP socket to local raveloxmidi
socket = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
socket.bind( ('', 5010 ) )
socket.settimeout(0.1)


# initialize old value for change detection
oldvalue = 0
	


# loop infinitely
while True:
	try:
		data, addr = socket.recvfrom(1024)
		#print ":".join(format(ord(c)) for c in data)
		#print ":".join("{0:x}".format(ord(c)) for c in data)
		
		if ord(data[0]) == 176 and ord(data[1]) == 21:
			elements = [255,1,min(2*ord(data[2]),254),254,0]
			print elements[2]
			for x in elements:
				#sys.stdout.write(chr(x))
				#sys.stdout.flush()
				serial.write(chr(x))
		

	except Exception:
		pass

	# try to read value from Arduino
	try:
		value = serial.readline().strip()
		value = int(value)
		safe = True
	except Exception:
		safe = False


'''TODO FIX THIS
	# value available and different from old one?
	if safe and value != oldvalue:

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
	#print serial.readline()

	
	#print ":".join("{0:x}".format(ord(c)) for c in data)
'''

	


	
	
