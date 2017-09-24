# A single LED that can be controlled
import time
import RPi.GPIO as GPIO

class Led:

    def __init__(self, pinRed, pinGreen, pinBlue):
        self.pinRed = pinRed
        self.pinGreen = pinGreen
        self.pinBlue = pinBlue

        self.r = 0
        self.g = 0
        self.b = 0

        GPIO.setmode(GPIO.BOARD)
        GPIO.setwarnings(False)

        GPIO.setup(pinRed, GPIO.OUT)
        GPIO.setup(pinGreen, GPIO.OUT)
        GPIO.setup(pinBlue, GPIO.OUT)

        # Set up colors using PWM so we can control individual brightness.
        self.RED = GPIO.PWM(pinRed, 100)
        self.GREEN = GPIO.PWM(pinGreen, 100)
        self.BLUE = GPIO.PWM(pinBlue, 100)
        self.RED.start(0)
        self.GREEN.start(0)
        self.BLUE.start(0)

    def setColor(self, r, g, b):
        self.r = r
        self.g = g
        self.b = b

        self.r = r/255.0 * 100
        self.g = g/255.0 * 100
        self.b = b/255.0 * 100

        print r, g,b

        self.RED.ChangeDutyCycle(r)
        self.GREEN.ChangeDutyCycle(g)
        self.BLUE.ChangeDutyCycle(b)
