#!/usr/bin/python

import logging
import time
from serial import Serial
from osc4py3.as_eventloop import *
from osc4py3 import oscbuildparse
from osc4py3.oscmethod import *



class InstrumentConfig:
    def __init__(self):
        self.name = "none"
        self.serialPort = "/dev/ttyACM0"
        self.oscServer = "192.168.1.253"
        self.oscSendPort = 9002
        self.oscReceivePort = 9002
        self.sendBytes = [255]

class Instrument:
    def __init__(self,config):
        self.name = config.name

        # open serial port to Arduino
        self.serial = Serial( config.serialPort, 115200, bytesize=8, parity='N', timeout=0.01 )
        self.sendBytes = config.sendBytes

        # open OSC communication
        osc_startup()
        osc_udp_client(config.oscServer, config.oscSendPort, self.name)
        #osc_udp_server("0.0.0.0", config.oscReceivePort, self.name)
        osc_method("/mimr", self.readOscInput, argscheme=OSCARG_MESSAGE)

        #give the instrument some time to init ....
        time.sleep(2)
        self.sendSerial()

    def update(self):
        osc_process()
        data = self.readSerialInput()
        if len(data) > 0:
            self.sendOscMessage(data)
 
    def readOscInput(self,msg):
        pass

    def readSerialInput(self):
        serialReadValues = []
        try:
            # try to read line from Arduino
            line = self.serial.readline()
            # got complete line with expected start and end character?
            if line[:1] == b":" and line[-1:] == b"\n":
                # get numeric values
                value = line[1:-1]
                #create array of values
                serialReadValues = list([int(x) for x in value.split()])
                logging.debug("got serial msg: " + ",".join([str(x) for x in serialReadValues]))
                #convert to int numbers and clamp
                #for x in serialReadValues:
                #    x = int(x)

                return serialReadValues
        except Exception:
            pass

        return []

    def sendOscMessage(self,data):
        packet = [self.name]
        packet.extend(data)
        msg = oscbuildparse.OSCMessage("/mimr/instrument", None, packet)
        osc_send(msg, self.name)
        logging.debug("send osc msg: " + ",".join([str(x) for x in packet]))

    def sendSerial(self):
        # first add sync byte ...
        packet = [255]

        #add data here

        packet.extend(self.sendBytes)

        #set instrument to active ....
        packet.append(0) #tmp activate - todo remove later ...
        logging.debug("send serial msg: " + ",".join([str(x) for x in packet]))
        self.serial.write(bytes(packet))