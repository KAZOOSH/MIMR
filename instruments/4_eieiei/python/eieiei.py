#!/usr/bin/python

import socket
import sys
import time
from time import sleep
from serial import Serial
import struct
import RPi.GPIO as GPIO


print "Starting Eieiei"

'''init serial and network'''
# open serial port to Arduino
#serial = Serial( "/dev/ttyACM0", 38400, bytesize=8, parity='N', timeout=0.01 )


# open UDP socket to listen raveloxmidi
udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
udpIn.bind( ('', 5010 ) )
udpIn.settimeout(0.1)

# open UDP socket to send raveloxmidi
udpOut = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpOut.connect( ( "localhost", 5006 ) )

#open socket to send to visualisation
udpVis = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpVis.connect( ( "localhost", 5013 ) )
sendFps = 4 #fps of vis
lastSendVis = 0;

# initialize old value for change detection
oldvalue = 0

# midi values
midiMode = 0
midiHue = 0
midiSat = 0
midiVal = 0

#GPIO
#GPIO Modus (BOARD / BCM)
GPIO.setmode(GPIO.BCM)
 
#GPIO Pins zuweisen
GPIO_TRIGGER = 18
GPIO_ECHO = 24
 
#Richtung der GPIO-Pins festlegen (IN / OUT)
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)



def distance():
    # setze Trigger auf HIGH
    GPIO.output(GPIO_TRIGGER, True)
 
    # setze Trigger nach 0.01ms aus LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
 
    StartZeit = time.time()
    StopZeit = time.time()
 
    # speichere Startzeit
    while GPIO.input(GPIO_ECHO) == 0:
        StartZeit = time.time()
 
    # speichere Ankunftszeit
    while GPIO.input(GPIO_ECHO) == 1:
        StopZeit = time.time()
 
    # Zeit Differenz zwischen Start und Ankunft
    TimeElapsed = StopZeit - StartZeit
    # mit der Schallgeschwindigkeit (34300 cm/s) multiplizieren
    # und durch 2 teilen, da hin und zurueck
    distanz = (TimeElapsed * 34300) / 2
 
    return distanz

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
	'''try:
		data, addr = udpIn.recvfrom(1024)
		#print ":".join(format(ord(c)) for c in data)
		#print ":".join("{0:x}".format(ord(c)) for c in data)

		elements = [255,midiMode,midiHue,midiSat,midiVal]
		# control command
		if ord(data[0]) == 176:
			#set mode
			if ord(data[1]) == 14:
				if ord(data[2]) <= 2:
					midiMode = ord(data[2])
				else:
					midiMode = 0
			#set hue
			elif ord(data[1]) == 15:
				midiHue = min(2*ord(data[2]),254)
			#set saturation
			elif ord(data[1]) == 16:
				midiSat = min(2*ord(data[2]),254)
			#set brightness
			elif ord(data[1]) == 17:
				midiVal = min(2*ord(data[2]),254)

			elements = [255,midiMode,midiHue,midiSat,midiVal]
			#print elements

			#for x in elements:
				#sys.stdout.write(chr(x))
				#sys.stdout.flush()
				#serial.write(chr(x))
		
	except Exception:
		pass

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

			# limit to 0..127
			value = max( min( int(value), 127 ), 0 )

			# update cached value
			oldvalue = value

			# log current value
			print value
			#sys.stdout.write( "%d   \r" % value )
			#sys.stdout.flush()

			# on MIDI channel 1, set controller #1 to value
			bytes = struct.pack( "BBBB", 0xaa, 0xB1, 0, value )
			udpOut.send( bytes )
		'''


		#print ":".join("{0:x}".format(ord(c)) for c in data)
	dist = distance()
	sleep(0.001)
	bytes = struct.pack( "BBBB", 0xaa, 0xB5, 0, mapValue(dist,0,30,0,127))
	udpOut.send( bytes )

	#send distance to visualisation
	if time.time() - lastSendVis > 1.0/sendFps:
		print dist
		udpVis.send( str(dist) )
		lastSendVis = time.time();

