#!/usr/bin/python

import socket
import sys
from time import time
from serial import Serial
import RPi.GPIO as GPIO
import struct

print "Start Bassfahrer"

'''init serial and network'''
# open serial port to Arduino
serial = Serial( "/dev/ttyACM0", 115200, bytesize=8, parity='N', timeout=0.01 )


# open UDP socket to listen raveloxmidi
udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
udpIn.bind( ('', 5010 ) )
udpIn.settimeout(0.01)

# decrease receive buffer size
udpIn.setsockopt( socket.SOL_SOCKET, socket.SO_RCVBUF, 128 )

# open UDP socket to send raveloxmidi
udpOut = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpOut.connect( ( "localhost", 5006 ) )


# MIDI configuration
midiInputChannel = 64
midiOutputChannel = 0xB6


# set up GPIO pin with pull-up for foot sensor
footSensorPin = 21
GPIO.setmode( GPIO.BCM )
GPIO.setup( footSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP )

releasing = False
releaseStart = 0
releaseTime = 2.0

isIdle = 1


# initialize old value for change detection
oldvalue = 0

# MIDI values
midiVal = 0


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
		elements = [255,isIdle,midiVal]
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
		except Exception:
			# no more packets to read
			bufferClear = True
			pass

	# control command
	if ord(data[0]) == 176:
		# get value from defined channel
		if ord(data[1]) == midiInputChannel  or ord(data[1]) == 3:
			midiVal = min(2*ord(data[2]),254)

		elements = [255,isIdle,midiVal]
		print elements
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
				value = int( line[1:-1] )

				# read was successfull
				safe = True

			else:
				safe = False

		except Exception:
			safe = False

		# value available and different from old one?
		if safe and value != oldvalue:

			# scale value from 0..215 to 0..127
			value = value * 127 / 215

			# limit to 0..127
			value = max( min( int(value), 127 ), 0 )

			# update cached value
			oldvalue = value

			# log current value
			print value

			# send via MIDI
			bytes = struct.pack( "BBBB", 0xaa, midiOutputChannel, 1, 0 if isIdle else value )
			udpOut.send( bytes )
