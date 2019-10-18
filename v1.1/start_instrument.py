from instrument import InstrumentConfig
from instrument import Instrument
from bank import Bank
import argparse
import socket

def createConfig(name):
    config = InstrumentConfig()

    if name == 'kurbel':
        config.midiInputStartChannel = 14
        config.midiOutputChannel = 0
        config.nMidiInputValues = 2
        config.inputNValues = [254,8]
    elif name == 'kuehler':
        config.midiInputStartChannel = 24
        config.midiOutputChannel = 1
        config.nMidiInputValues = 2
        config.inputNValues = [254,8]
        config.nMidiOutputValues = 6
    elif name == 'theremin':
        config.midiInputStartChannel = 34
        config.midiOutputChannel = 2
        config.nMidiInputValues = 2
        config.inputNValues = [254,254]
        config.serialPort = "/dev/ttyUSB0"
    elif name == 'trichter':
        config.midiInputStartChannel = 44
        config.midiOutputChannel = 3
        config.nMidiInputValues = 3
        config.inputNValues = [254,254,254]
    elif name == 'eieiei':
        config.midiInputStartChannel = 54
        config.midiOutputChannel = 4
        config.nMidiInputValues = 1
        config.inputNValues = [254,254]
        config.nMidiOutputValues = 3
    elif name == 'goldenbox':
        config.midiInputStartChannel = 64
        config.midiOutputChannel = 5
        config.nMidiInputValues = 2
        config.inputNValues = [254,254]
        config.isPi1 = True
    elif name == 'bassfahrer':
        config.midiInputStartChannel = 64
        config.midiOutputChannel = 6
        config.nMidiInputValues = 0
        config.inputNValues = []
    elif name == 'foen':
        config.midiInputStartChannel = 74
        config.midiOutputChannel = 7
        config.nMidiInputValues = 1
        config.inputNValues = [254]
        config.nMidiOutputValues = 5
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
    device = Bank()
else:
    config = createConfig(instrumentName)
    device = Instrument(config)


# update instrument
while True:
	device.update()