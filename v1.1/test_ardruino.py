from serial import Serial
from time import sleep
import binascii
import RPi.GPIO as GPIO


serial = Serial( "/dev/ttyACM0", 115200, bytesize=8, parity='N', timeout=0.01 )

footSensorPin = 21 #7 for pi1
GPIO.setmode( GPIO.BCM )
GPIO.setup( footSensorPin, GPIO.IN, pull_up_down=GPIO.PUD_UP )

counter=0

while True:
    print("\033[2J\033[1;1H")#clear screen & home

    counter = counter + 1
    print("COUNTER: %d" % (counter))

    footActive = not GPIO.input(footSensorPin)
    if footActive:
        print("FOOT: ACTIVE\n")
    else:
        print("FOOT: NOT ACTIVE\n")

    send = bytes([255,1,128,128,128])
    print("TX: ")
    print(send.hex())
    print("\n ")
    serial.write(send)

    line = serial.readline()
    print("RX: ")
    print(line.hex())
    print("\n")


    sleep(0.05)




