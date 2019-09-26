#!/usr/bin/python

import logging
import socket
import sys
from time import time
import struct

import RPi.GPIO as GPIO

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    )



class Bank:
    def __init__(self):

        # MIDI configuration
        # starting with 0xB0 -> 178  
        self.midiOutputChannel = 178 + 8
        self.midiInputStartChannel = 84

        #beat channel
        self.beatChannel = 3

        #init input values [intensity/beat, ... , value n]
        #self.inputValues = []
        #self.inputNValues = []

        #for i in range(config.nMidiInputValues):
        #    self.inputValues.append(0)
        #    self.inputNValues.append(config.inputNValues[i])

        #init output values [isIdle,value1, ... , value n]
        self.outputValues = []

        for i in range(3):
            self.outputValues.append(0)

        #init foot sensor
        self.sensorPin = [7,8,25]

        GPIO.setmode( GPIO.BCM )
        for s in self.sensorPin:
            GPIO.setup( s, GPIO.IN, pull_up_down=GPIO.PUD_UP )

        self.releasing = [False,False,False]
        self.releaseStart = [0,0,0]
        self.releaseTime = 0.01

        # open serial port to Arduino
        #self.serial = Serial( config.serialPort, 115200, bytesize=8, parity='N', timeout=0.01 )

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
        self.updateFootState()
        #self.readMidiInput()
        #self.readSerialInput()  

    def updateFootState(self):

        # idle to active?
        for i in range(3):
            sendIdleUpdate = False
            footActive = not GPIO.input(self.sensorPin[i])

            if footActive:
                # accept immediately
                self.releasing[i] = False
                if self.outputValues[i] != 127:
                    sendIdleUpdate = True
                    self.outputValues[i] = 127
                    logging.info("Seat" + str(i) + " active")
            # about to go from active to idle?
            if self.outputValues[i] == 127 and not footActive:
                # not yet releasing?
                if not self.releasing[i]:
                    # remember start
                    self.releaseStart[i] = time()
                    self.releasing[i] = True
                # already releasing, time elapsed?
                if self.releasing[i] and time()-self.releaseStart[i] > self.releaseTime:
                    # finally, turn idle!
                    sendIdleUpdate = True
                    self.outputValues[i] = 0
                    self.releasing[i] = False
                    logging.info("Seat" + str(i) + " inactive")

            # state changed? then send midi active message
            if sendIdleUpdate:
                self.sendMidiMessage(i)
      

    def sendMidiMessage(self,attribute):
        bytes = struct.pack( "BBBB", 0xaa, self.midiOutputChannel, attribute, int(self.outputValues[attribute]))
        self.udpOut.send( bytes )    


    
