#!/usr/bin/python

import socket
import sys
from time import sleep
from serial import Serial
import struct

print "Start fön"

'''init serial and network'''
# open serial port to Arduino
serial = Serial( "/dev/ttyACM0", 115200, bytesize=8, parity='N', timeout=0.01 )


# open UDP socket to listen raveloxmidi
udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
udpIn.bind( ('', 5010 ) )
udpIn.settimeout(0.01)

# decrease receive buffer size
udpIn.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF,128)

# open UDP socket to send raveloxmidi
udpOut = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpOut.connect( ( "localhost", 5006 ) )


# initialize old values for change detection
oldFoehn = 0
oldButton1 = 0
oldButton2 = 0
oldButton3 = 0

# midi values
intensity = 0
hueMin = 0
hueMax = 60

# loop infinitely
while True:
	# incoming UDP packets in buffer?
	bufferClear = False

	# UDP data as string
	data = chr(0)

	# as long as packets in buffer...
	# (empty buffer, only forward latest packet)
	if not bufferClear:
		try:
			# read UDP packet from socket
			data, addr = udpIn.recvfrom(256)
			#print ":".join(format(ord(c)) for c in data)
			#print ":".join("{0:x}".format(ord(c)) for c in data)
		except Exception:
			# no more packets to read
			bufferClear = True
			pass


	# control command
	if ord(data[0]) == 176:
		#set intensity
		if ord(data[1]) == 44:
			intensity = min(2*ord(data[2]),254);
		#set hueMin
		elif ord(data[1]) == 45:
			hueMin = min(2*ord(data[2]),254)
		#set hueMax
		elif ord(data[1]) == 46:
			hueMax = min(2*ord(data[2]),254)

		elements = [255,intensity,hueMin,hueMax];
		#print elements

		for x in elements:
			#sys.stdout.write(chr(x))
			#sys.stdout.flush()
			serial.write(chr(x))

	# try to read value from Arduino
	safe = True

	while safe:

		try:
			# try to read line from Arduino
			line = serial.readline()

			# got complete line with expected start and end character?
			if line[:1] == ":" and line[-1:] == "\n":

				# get numeric value
				value = int( line[1:-1] )
				
				# read was successfull
				safe = True

			else:
				safe = False

		except Exception:
			safe = False

		'''# value available and different from old one?
		if safe and value != oldvalue:

			# limit to 0..127
			value = max( min( int(value), 127 ), 0 )

			# update cached value
			oldvalue = value

			# log current value
			print value
			#sys.stdout.write( "%d   \r" % value )
			#sys.stdout.flush()

			# on MIDI channel 4, set controller #1 to value
			bytes = struct.pack( "BBBB", 0xaa, 0xB7, 0, value )
			udpOut.send( bytes )'''


		#print ":".join("{0:x}".format(ord(c)) for c in data)
