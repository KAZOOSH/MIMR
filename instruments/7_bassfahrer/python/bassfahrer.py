#!/usr/bin/python

import socket
import sys
from time import sleep
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


# set up GPIO pin with pull-up for foot sensor
footSensorPin = 21
GPIO.setmode( GPIO.BCM )
GPIO.setup( footSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP )
lastSensorChangeTime = time()
sensorDebounceTime = 4.0


# initialize old value for change detection
oldvalue = 0

# MIDI and status values
midiVal = 0
idle = 0


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
		except Exception:
			# no more packets to read
			bufferClear = True
			pass

	# query foot sensor
	if time() - lastSensorChangeTime > sensorDebounceTime:
		sensorValue = GPIO.input( footSensorPin )
		if sensorValue != idle:
			lastSensorChangeTime = time()
			idle = sensorValue
			print "Idle state change:", idle
			elements = [255,idle,midiVal]
			for x in elements:
				serial.write(chr(x))

	# control command
	if ord(data[0]) == 176:
		# get value from defined channel
		if ord(data[1]) == midiInputChannel:
			midiVal = min(2*ord(data[2]),254)

		elements = [255,idle,midiVal]
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

			# on MIDI channel 1, set controller #1 to value
			bytes = struct.pack( "BBBB", 0xaa, 0xB1, 0, 0 if idle else value )
			udpOut.send( bytes )
