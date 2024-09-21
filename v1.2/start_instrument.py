from instrument import InstrumentConfig
from instrument import Instrument
#from bank import Bank
import argparse
import socket

def createConfig(name):
    config = InstrumentConfig()
    config.name = name

    if name == 'kurbel':
        config.inputNValues = [254,8]

    elif name == 'kuehler':
        config.inputNValues = [254,8]
        
    elif name == 'theremin':
        config.inputNValues = [254,254]
        config.serialPort = "/dev/ttyUSB0"

    elif name == 'trichter':
        config.inputNValues = [254,254,254]

    elif name == 'eieiei':
        config.inputNValues = [254,254]

    elif name == 'goldenbox':
        config.inputNValues = [254,254]

    elif name == 'bassfahrer':
        config.inputNValues = []

    elif name == 'foen':
        config.inputNValues = [254]
        
    return config


# parse the instrument input
parser = argparse.ArgumentParser(description='Optional app description')
parser.add_argument('instrument', type=str,
                    help='the instrument, either : kurbel, kuehler, theremin, trichter, eieiei, golden_box, bassfahrer, foen OR auto - for automatic')

args = parser.parse_args()

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
    #config.serialPort = "/dev/ttyACM4"
    #config.oscServer = "192.168.1.253"
    device = Instrument(config)


# update instrument
while True:
	device.update()