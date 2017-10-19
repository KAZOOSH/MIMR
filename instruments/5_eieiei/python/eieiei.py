#!/usr/bin/python

import socket
import sys
import time
from time import sleep
from serial import Serial
import struct
import RPi.GPIO as GPIO
from numpy import median
from numpy import mean

print "Start Eieiei"

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

#open socket to send to visualisation
udpVis = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpVis.connect( ( "localhost", 5013 ) )
sendFps = 10 #fps of vis
lastSendVis = 0;

# initialize old values for change detection
oldValues = [0,0]

# initialize the read values
values = [0,0]

# spinValues
spinValues = []
spinIndex = 0

for i in range(15):
	spinValues.append(0)

spinValuesMedian = []
spinIndexMedian = 0;

for i in range(15):
	spinValuesMedian.append(0)


# midi values
intensity = 0

footPin = 21 #7 on pi1    21 on pi2
GPIO.setmode(GPIO.BCM)
GPIO.setup(footPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

lastFootChange = 0
minFootDwellTime = 5.0
isIdle = 1

def mapValue(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    value = min(value, leftMax)

    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

# loop infinitely
while True:

	# foot sensor
	if time.time() - lastFootChange > minFootDwellTime:
		tIdle = GPIO.input(footPin)
		if tIdle != isIdle:
			isIdle = tIdle
			lastFootChange = time.time()
			elements = [255,isIdle]

			#print elements
			for x in elements:
				serial.write(chr(x))

			bytes = struct.pack( "BBBB", 0xaa, 0xB4, 0, 0 if isIdle else int(values[0]) )
			bytes = struct.pack( "BBBB", 0xaa, 0xB4, 1, 0 if isIdle else values[1] )
			udpOut.send( bytes )

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
		if ord(data[1]) == 54:
			intensity = min(2*ord(data[2]),254)
		
		'''
		#elements = [255,intensity,hue,saturation]
		#print elements

		for x in elements:
			#sys.stdout.write(chr(x))
			#sys.stdout.flush()
			serial.write(chr(x))
		'''
		pass

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
			#membrane
			if len(values) >= 1:
				if (int)(values[0]) !=(int)(oldValues[0]):
					# limit to 0..127
					values[0] = max( min( int(values[0]), 127 ), 0 )
					# update cached value
					oldValues[0] = values[0]


					# on MIDI channel 4, set controller #1 to value
					bytes = struct.pack( "BBBB", 0xaa, 0xB4, 0, values[0] )
					udpOut.send( bytes )
			#spin
			if len(values) >= 2:
				#median filter spin values
				spinValues[spinIndex] = (int)(values[1])
				spinIndex = (spinIndex+1)%len(spinValues)
				spinValuesMedian[spinIndexMedian] = median(spinValues)
				spinIndexMedian = (spinIndexMedian+1)%len(spinValuesMedian)
				#smooth median values
				values[1] = mean(spinValuesMedian)

				if (int)(values[1]) !=(int)(oldValues[1]):
					# limit to 0..127
					values[1] = max( min( int(values[1]), 127 ), 0 )
					# update cached value
					oldValues[1] = values[1]

					# on MIDI channel 4, set controller #1 to value
					bytes = struct.pack( "BBBB", 0xaa, 0xB4, 1, values[1] )
					udpOut.send( bytes )	
			
			#print values

		safe = False

		

	#send distance to visualisation
	if time.time() - lastSendVis > 1.0/sendFps:
		#print isIdle
		#print float(values[0])*0.9 + intensity*0.1
		#print str(float(values[0])*0.80 + intensity*0.1) + ":" + str(isIdle) 
		d = str(float(values[0])*0.80 + intensity*0.1);
		if (isIdle == 1): d = "127"
		udpVis.send( d + ":" + str(isIdle) )
		lastSendVis = time.time();
