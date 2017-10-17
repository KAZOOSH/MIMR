#!/usr/bin/python

import socket
import sys
import time
from serial import Serial
import struct

import RPi.GPIO as GPIO

print "Start foen"

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
oldValues = [0,0,0,0]

# initialize the read values
values = [0,0,0,0]

# midi values
intensity = 0
hueMin = 0
hueMax = 60

#gpio foot sensor

footPin = 21 #7 on pi1    21 on pi2
GPIO.setmode(GPIO.BCM)
GPIO.setup(footPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

lastFootChange = 0
minFootDwellTime = 5.0
isIdle = 1

# loop infinitely
while True:
# foot sensor
	if time.time() - lastFootChange > minFootDwellTime:
		tIdle = GPIO.input(footPin)
		if tIdle != isIdle:
			print ("idle " + str(tIdle) + "  " + str(isIdle))
			isIdle = tIdle
			lastFootChange = time.time()
			elements = [255,intensity,isIdle]
			print elements
			for x in elements:
				serial.write(chr(x))
	
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
		if ord(data[1]) == 74:
			intensity = min(2*ord(data[2]),254);

		elements = [255,intensity,isIdle]
		#print elements

		if isIdle == 0:
			for x in elements:
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
				#value = int( line[1:-1] )
				value = line[1:-1]
				values = value.split()

				for x in values:
					x = (int)(x)

				# read was successfull
				safe = True

			else:
				safe = False

		except Exception:
			safe = False

		# value available and different from old one?
		if safe:
			for i in range(len(values)):
				if (int)(values[i]) !=(int)(oldValues[i]):
					# limit to 0..127
					values[i] = max( min( int(values[i]), 127 ), 0 )
					
					# update cached value
					oldValues[i] = values[i]

					# on MIDI channel 4, set controller #1 to value
					bytes = struct.pack( "BBBB", 0xaa, 0xB7, i, values[i] )
					udpOut.send( bytes )
					#print ":".join(format(ord(c)) for c in bytes)	
				

		#print ":".join("{0:x}".format(ord(c)) for c in data)
