#!/usr/bin/python

import socket
import sys
from time import sleep
from serial import Serial
import struct

print "Start Theremin"

'''init serial and network'''
# open serial port to Arduino
serial = Serial( "/dev/ttyUSB0", 57600, bytesize=8, parity='N', timeout=0 )


# open UDP socket to listen raveloxmidi
udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
udpIn.bind( ('', 5010 ) )
udpIn.settimeout(0.1)

# open UDP socket to send raveloxmidi
udpOut = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpOut.connect( ( "localhost", 5006 ) )


# initialize old value for change detection
oldvalue = 0

# midi values
midiMode = 0
midiHue = 0
midiSaturation = 0
midiBrightness = 0

# loop infinitely
while True:
	try:
		data, addr = udpIn.recvfrom(1024)
		print ":".join(format(ord(c)) for c in data)
		#print ":".join("{0:x}".format(ord(c)) for c in data)

		elements = [255,midiMode,midiHueMin,midiHueMax,midiBrightness]
		# control command
		if ord(data[0]) == 176:
			#set mode
			if ord(data[1]) == 34:
				#print ord(data[2])
				midiMode = max(ord(data[2]),1)
			#set hue
			elif ord(data[1]) == 35:
				midiHue = min(2*ord(data[2]),254)
			#set saturation
			elif ord(data[1]) == 36:
				midiSaturation = min(2*ord(data[2]),254)
			#set saturation
			elif ord(data[1]) == 37:
				midiBrightness = min(2*ord(data[2]),254)

			elements = [255,midiMode,midiHue,midiSaturation,midiBrightness]
			print elements

			for x in elements:
				#sys.stdout.write(chr(x))
				#sys.stdout.flush()
				serial.write(chr(x))


	except Exception:
		pass

	# try to read value from Arduino
	safe = True

	while safe:

		try:
			value = serial.readline().strip()

			value = int(value)
			safe = True
		except Exception:
			safe = False

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
			bytes = struct.pack( "BBBB", 0xaa, 0xB2, 0, value )
			udpOut.send( bytes )
		#print serial.readline()


		#print ":".join("{0:x}".format(ord(c)) for c in data)
