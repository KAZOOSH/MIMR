#!/usr/bin/python

import socket
import sys
from time import time
from serial import Serial
import struct

import RPi.GPIO as GPIO

print "Start Kuehler				"

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
oldValues = [0,0,0,0,0]

# initialize the read values
values = [0,0,0,0,0]

# midi values
intensity = 0
hue = 0
saturation = 255

# MIDI configuration
midiOutputChannel = 0xB1


# set up GPIO pin with pull-up for foot sensor
footSensorPin = 21
GPIO.setmode( GPIO.BCM )
GPIO.setup( footSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP )

releasing = False
releaseStart = 0
releaseTime = 2.0

isIdle = 1

# loop infinitely
while True:

	# query sensor state
	footState = GPIO.input(footSensorPin)

	sendIdleUpdate = False

	# idle to active?
	if not footState:
		# accept immediately
		releasing = False
		if isIdle:
			isIdle = 0
			sendIdleUpdate = True

	# about to go from active to idle?
	if not isIdle and footState:
		# not yet releasing?
		if not releasing:
			# remember start
			releaseStart = time()
			releasing = True
			print "Foot released..."
		# already releasing, time elapsed?
		elif time()-releaseStart > releaseTime:
			# finally, turn idle!
			isIdle = 1
			sendIdleUpdate = True
			releasing = False

	# state changed?
	if sendIdleUpdate:

		print "Idle state:", isIdle

		# send MIDI update
		bytes = struct.pack( "BBBB", 0xaa, midiOutputChannel, 0, 0 if isIdle else 127 )
		udpOut.send( bytes )

		# update Arduino
		elements = [255,intensity,hue,saturation,isIdle]
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
		if ord(data[1]) == 24 or ord(data[1]) == 3:
			intensity = min(2*ord(data[2]),254)
		#set hue
		elif ord(data[1]) == 25:
			hue = min(2*ord(data[2]),254)
		#set saturation
		elif ord(data[1]) == 26:
			saturation = min(2*ord(data[2]),254)
		elements = [255,intensity,hue,saturation,isIdle]
		#print elements

		if isIdle == 0:
			for x in elements:
				serial.write(chr(x))

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


		except Exception:
			safe = False

		if safe:
			for i in range(len(values)):
				if (int)(values[i]) !=(int)(oldValues[i]):
					# limit to 0..127
					values[i] = max( min( int(values[i]), 127 ), 0 )
					if values[i] == 1:
						values[i] = 127;
					# update cached value
					oldValues[i] = values[i]

					# on MIDI channel 4, set controller #1 to value
					print values
					bytes = struct.pack( "BBBB", 0xaa, midiOutputChannel, i+1, values[i] )
					udpOut.send( bytes )
					#print ":".join(format(ord(c)) for c in bytes)
		safe = False

		#print ":".join("{0:x}".format(ord(c)) for c in data)
