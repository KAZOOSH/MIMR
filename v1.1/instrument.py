#!/usr/bin/python

import logging
import socket
import sys
from time import time
from serial import Serial
import struct

import RPi.GPIO as GPIO

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    )


class Instrument:
    def __init__(self,midiInputStartChannel,midiOutputChannel,nMidiInputValues = 1, nMidiOutpuValues = 2, isPi1 = False):

        # MIDI configuration
        self.midiOutputChannel = midiInputStartChannel
        self.midiInputStartChannel = midiOutputChannel

        #beat channel
        self.beatChannel = 3

        #init input values [intensity/beat, ... , value n]
        self.inputValues = []

        for i in range(nMidiInputValues):
            self.inputValues.append(0)

        #init output values [isIdle,value1, ... , value n]
        self.outputValues = []

        for i in range(nMidiOutpuValues):
            self.outputValues.append(0)

        #init foot sensor
        GPIO.setmode( GPIO.BCM )
        GPIO.setup( footSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP )

        self.footSensorPin = 21
        if (isPi1): 
            self.footSensorPin = 7

        self.releasing = False
        self.releaseStart = 0
        self.releaseTime = 2.0

        # open serial port to Arduino
        self.serial = Serial( "/dev/ttyACM0", 115200, bytesize=8, parity='N', timeout=0.01 )

        # open UDP socket to listen raveloxmidi
        self.udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
        self.udpIn.bind( ('', 5010 ) )
        self.udpIn.settimeout(0.01)
        # decrease receive buffer size
        self.udpIn.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF,128)

        # open UDP socket to send raveloxmidi
        self.udpOut = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
        self.udpOut.connect( ( "localhost", 5006 ) )
        
    def update(self):
        updateFootState()
        readMidiInput()
        readSerialInput()  

    def updateFootState(self):

        #outputValues[0] is the active state 0-idle 127-active

        # query sensor state
        footState = GPIO.input(footSensorPin)

        sendIdleUpdate = False

        # idle to active?
        if not footState:
            # accept immediately
            releasing = False
            if not outputValues[0]:
                sendIdleUpdate = True
                outputValues[0] = 127
                logging.info("Instrument active")
        # about to go from active to idle?
        if not outputValues[0] and footState:
            # not yet releasing?
            if not releasing:
                # remember start
                releaseStart = time()
                releasing = True
                logging.info("Instrument inactive")
            # already releasing, time elapsed?
            elif time()-releaseStart > releaseTime:
                # finally, turn idle!
                sendIdleUpdate = True
                outputValues[0] = 0
                releasing = False

        # state changed? then send midi active message
        if sendIdleUpdate:
            sendMidiMessage(0)
            sendSerial()
    
    def readMidiInput(self):
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
            # check if message is beat signal
            if ord(data[1]) == 3:
                inputValues[0] = min(2*ord(data[2]),254)
            # get value from defined channel
            for x in xrange(0,len(midiInputChannel)):
                if ord(data[1]) == x:
                    inputValues[x] = min(2*ord(data[2]),254)

            #send new state to serial
            sendSerial()
    
    def readSerialInput(self):
        safe = True

        serialReadValues = []
        while safe:
            try:
                # try to read line from Arduino
                line = serial.readline()

                # got complete line with expected start and end character?
                if line[:1] == ":" and line[-1:] == "\n":

                    # get numeric values
                    value = line[1:-1]
                    #create array of values
                    serialReadValues = value.split()
                    #convert to int numbers and clamp
                    for x in serialReadValues:
                        x = max( min( int(x), 127 ), 0 )

                    # read was successfull
                    safe = True


            except Exception:
                safe = False

            if safe:
                for i in range(len(serialReadValues)):
                    #compare read values with output values
                    #using i+1 because index 0 is active state
                    if serialReadValues[i] != self.outputValues[i+1]:
                        # update cached value
                        outputValues[i+1] = serialReadValues[i]

                        
                        logging.info("new value on channel " + str(i+1) + " : " + str(outputValues[i+1]))
                        
                        #send value
                        sendMidiMessage(i+1)
                        
            safe = False

    def sendMidiMessage(self,attribute):
        # send MIDI update
        bytes = struct.pack( "BBBB", 0xaa, midiOutputChannel, attribute, outputValues[attribute])
        udpOut.send( bytes )    

    def sendSerial(self):
        # update Arduino, first add sync byte
        elements = [255]

        # add idle state
        elements.append(outputValues[0])

        # add all input values
        for i in range(len(inputValues)):
            elements.append(min(2*ord(self.inputValues[i]),254))

        #write values to serial port
        for x in elements:
            serial.write(chr(x))

    
