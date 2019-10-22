#!/usr/bin/python

import logging
import socket
import sys
from time import time
from serial import Serial
import struct
from osc4py3.as_eventloop import *
from osc4py3 import oscbuildparse
from osc4py3.oscmethod import *

import RPi.GPIO as GPIO

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    )


class InstrumentConfig:
    def __init__(self):
        self.name = "none"
        self.midiInputStartChannel = 0
        self.midiOutputChannel = 0
        self.nMidiInputValues = 1
        self.nMidiOutputValues = 2
        self.isPi1 = False
        self.inputNValues= [254]
        self.serialPort = "/dev/ttyACM0"
        self.oscServer = "192.168.1.133"
        self.oscSendPort = 9002
        self.oscReceivePort = 9002
        self.enableMidi = False
        self.enableOsc = True


class Instrument:
    def __init__(self,config):

        self.name = config.name

        # MIDI configuration
        # starting with 0xB0 -> 178  
        self.midiOutputChannel = 178 + config.midiOutputChannel
        self.midiInputStartChannel = config.midiInputStartChannel
        self.enableMidi = config.enableMidi

        # osc
        self.enableOsc = config.enableOsc

        #beat channel
        self.beatChannel = 3

        #init input values [intensity/beat, ... , value n]
        self.inputValues = []
        self.inputNValues = []

        for i in range(config.nMidiInputValues):
            self.inputValues.append(0)
            self.inputNValues.append(config.inputNValues[i])

        #init output values [isIdle,value1, ... , value n]
        self.outputValues = []

        for i in range(config.nMidiOutputValues):
            self.outputValues.append(0)

        #init foot sensor
        self.footSensorPin = 21
        if (config.isPi1): 
            self.footSensorPin = 7

        GPIO.setmode( GPIO.BCM )
        GPIO.setup( self.footSensorPin, GPIO.IN  )

        self.releasing = False
        self.releaseStart = 0
        self.releaseTime = 0.01

        # open serial port to Arduino
        self.serial = Serial( config.serialPort, 115200, bytesize=8, parity='N', timeout=0.01 )

        # open UDP socket to listen raveloxmidi
        self.udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
        self.udpIn.bind( ('', 5010 ) )
        self.udpIn.settimeout(0.01)
        # decrease receive buffer size
        self.udpIn.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF,128)

        # open UDP socket to send raveloxmidi
        if config.enableMidi:
            self.udpOut = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
            self.udpOut.connect( ( "localhost", 5006 ) )

        # open OSC communication
        if config.enableOsc:
            osc_startup()
            osc_udp_client(config.oscServer, config.oscSendPort, self.name)
            #osc_udp_server("0.0.0.0", config.oscReceivePort, self.name)
            osc_method("/mimr", self.readOscInput, argscheme=OSCARG_MESSAGE)

    def update(self):
        self.updateFootState()
        if self.enableMidi:
            self.readMidiInput()
        if self.enableOsc:
            osc_process()
        self.readSerialInput()  

    def updateFootState(self):

        #outputValues[0] is the active state 0-idle 127-active

        # query sensor state
        footActive = GPIO.input(self.footSensorPin)

        sendIdleUpdate = False

        # idle to active?
        if footActive:
            # accept immediately
            self.releasing = False
            if self.outputValues[0] != 127:
                sendIdleUpdate = True
                self.outputValues[0] = 127
                logging.info("Instrument active")
        # about to go from active to idle?
        if self.outputValues[0] == 127 and not footActive:
            # not yet releasing?
            if not self.releasing:
                # remember start
                self.releaseStart = time()
                self.releasing = True
            # already releasing, time elapsed?
            if self.releasing and time()-self.releaseStart > self.releaseTime:
                # finally, turn idle!
                sendIdleUpdate = True
                self.outputValues[0] = 0
                self.releasing = False
                logging.info("Instrument inactive")

        # state changed? then send midi active message
        if sendIdleUpdate:
            if self.enableMidi:
                self.sendMidiMessage(0)
            if self.enableOsc:
                self.sendOscMessage(0)
            self.sendSerial()
    
    def readOscInput(self,msg):
        pass


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
                data, addr = self.udpIn.recvfrom(256)
                print (":".join(format(ord(c)) for c in data))
            except Exception:
                # no more packets to read
                bufferClear = True
                pass

        # control command
        if data[0] == 176:
            isUpdated = False
            # check if message is beat signal
            if data[1] == 3:
                self.inputValues[0] = min(2*ord(data[2]),254)
                isUpdated = True
            elif data[1]- self.midiInputStartChannel < len(self.inputValues) and data[1] - self.midiInputStartChannel >= 0:
                # get value from defined channel
                index = data[1] - self.midiInputStartChannel
                self.inputValues[index] = int(min(2*data[2]*self.inputNValues[index]/254,254))
                logging.info(str(index) + "  " + str(self.inputValues[index]))
                isUpdated = True
            
            #send new state to serial
            if isUpdated:
                self.sendSerial()
    
    def readSerialInput(self):
        safe = True

        serialReadValues = []
        while safe:
            try:
                # try to read line from Arduino
                line = self.serial.readline()

                # got complete line with expected start and end character?
                if line[:1] == b":" and line[-1:] == b"\n":
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
                    if serialReadValues[i] != self.outputValues[i+1] and self.outputValues[0] == 127:
                        # update cached value
                        self.outputValues[i+1] = serialReadValues[i]
                        logging.info("new value on channel " + str(i+1) + " : " + str(self.outputValues[i+1]))
                        
                        #send value
                        if self.enableMidi:
                            self.sendMidiMessage(i+1)
                        if self.enableOsc:
                            self.sendOscMessage(i+1)
                        
            safe = False

    def sendMidiMessage(self,attribute):
        if attribute == 0 or self.outputValues[0] == 127:
            bytes = struct.pack( "BBBB", 0xaa, self.midiOutputChannel, attribute, int(self.outputValues[attribute]))
            self.udpOut.send( bytes )  

    def sendOscMessage(self,attribute):
        if attribute == 0 or self.outputValues[0] == 127:
            msg = oscbuildparse.OSCMessage("/mimr/instrument", None, [self.name, self.midiOutputChannel, attribute,int(self.outputValues[attribute])])
            osc_send(msg, self.name)

    def sendSerial(self):
        # update Arduino, first add sync byte
        elements = [255]

        # add all input values
        for i in range(len(self.inputValues)):
            #elements.append(min(2*ord(self.inputValues[i]),254))
            elements.append(min(2*self.inputValues[i],254))

        # add idle state, arduino uses inverse value
        if self.outputValues[0] == 0:
            elements.append(1)
        else: 
            elements.append(0)

        #write values to serial port
        print(elements)

        for x in elements:
            self.serial.write(bytes([x]))

    
