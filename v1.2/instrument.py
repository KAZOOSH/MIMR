#!/usr/bin/python

import logging
from time import time
from serial import Serial
from osc4py3.as_eventloop import *
from osc4py3 import oscbuildparse
from osc4py3.oscmethod import *

logging.basicConfig(level=logging.DEBUG,format='%(asctime)s %(levelname)s %(message)s',)

class InstrumentConfig:
    def __init__(self):
        self.name = "none"
        self.inputNValues= [254]
        self.serialPort = "/dev/ttyACM0"
        self.oscServer = "192.168.1.133"
        self.oscSendPort = 9002
        self.oscReceivePort = 9002

class Instrument:
    def __init__(self,config):

        self.name = config.name

        # osc
        self.enableOsc = config.enableOsc

        #init input values [intensity/beat, ... , value n]
        self.inputValues = []
        self.inputNValues = []

        #init output values [isIdle,value1, ... , value n]
        self.outputValues = []

        # open serial port to Arduino
        self.serial = Serial( config.serialPort, 115200, bytesize=8, parity='N', timeout=0.01 )

        # open OSC communication
        if config.enableOsc:
            osc_startup()
            osc_udp_client(config.oscServer, config.oscSendPort, self.name)
            #osc_udp_server("0.0.0.0", config.oscReceivePort, self.name)
            osc_method("/mimr", self.readOscInput, argscheme=OSCARG_MESSAGE)

    def update(self):
        if self.enableOsc:
            osc_process()
        self.readSerialInput()
 
    def readOscInput(self,msg):
        pass

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
                else:
                    logging.debug("readline buffer invalid.")


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
                        if self.enableOsc:
                            self.sendOscMessage(i+1)
                        
            safe = False

    def sendOscMessage(self,attribute):
        if attribute == 0 or self.outputValues[0] == 127:
            msg = oscbuildparse.OSCMessage("/mimr/instrument", None, [self.name, attribute,int(self.outputValues[attribute])])
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

        logging.info("serial write: " + str(elements))

        #TODO: why this ?
        #for x in elements:
        #    self.serial.write(bytes([x]))
        #TODO: this should be fine ...
        self.serial.write(bytes(elements))