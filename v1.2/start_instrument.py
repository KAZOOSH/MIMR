from instrument import InstrumentConfig
from instrument import Instrument
#from bank import Bank
import argparse
import logging
import socket

def createConfig(name):
    config = InstrumentConfig()
    config.name = name

    if name == 'kurbel':
        #config.inputNValues = [254,8] #leave this for refrence
        pass

    elif name == 'kuehler':
        #config.inputNValues = [254,8] #leave this for refrence
        pass

    elif name == 'theremin':
        #config.inputNValues = [254,254] #leave this for refrence
        config.serialPort = "/dev/ttyUSB0"

    elif name == 'trichter':
        #config.inputNValues = [254,254,254] #leave this for refrence
        config.sendBytes = [40,40,40]

    elif name == 'eieiei':
        #config.inputNValues = [254,254] #leave this for refrence
        pass

    elif name == 'goldenbox':
        #config.inputNValues = [254,254] #leave this for refrence
        config.sendBytes = [40]

    elif name == 'bassfahrer':
        #config.inputNValues = [] #leave this for refrence
        pass

    elif name == 'foen':
        #config.inputNValues = [254] #leave this for refrence
        pass
        
    return config


# parse the instrument input
parser = argparse.ArgumentParser(description='Optional app description')
parser.add_argument('instrument', type=str,
                    help='the instrument, either : kurbel, kuehler, theremin, trichter, eieiei, golden_box, bassfahrer, foen OR auto - for automatic')
parser.add_argument('--debug', type=bool,dest='loglevel',help='set loglevel to debug',default=False)
args = parser.parse_args()

loglevel = {
    True: logging.DEBUG,
    False: logging.INFO
} 
logging.basicConfig(level=loglevel[args.loglevel],format='%(asctime)s %(levelname)s %(message)s',)

# create the instrument
device = {}
instrumentName = args.instrument
if instrumentName == "auto":
    print("detected hostname: %s" % socket.gethostname())
    instrumentName = socket.gethostname()

if instrumentName == "bank":
    #device = Bank()
    print("currently not implementet")
    exit(1)
else:
    config = createConfig(instrumentName)
    config.serialPort = "/dev/ttyACM4"
    #config.oscServer = "192.168.1.253"
    device = Instrument(config)


# update instrument
while True:
	device.update()