import RPi.GPIO as GPIO
import os
import sys
from subprocess import Popen

GPIO.setmode(GPIO.BCM)
GPIO.setup(27, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(22, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(24, GPIO.IN, pull_up_down=GPIO.PUD_UP)

movie1 = ("/home/pi/Videos/movie1.mp4")
movie2 = ("/home/pi/Videos/movie2.mp4")
movie3 = ("/home/pi/Videos/movie3.mp4")
movie4 = ("/home/pi/Videos/movie4.mp4")
movie5 = ("/home/pi/Videos/movie5.mp4")
movie6 = ("/home/pi/Videos/movie6.mp4")

last_state1 = True
last_state2 = True
last_state3 = True
last_state4 = True
last_state5 = True
last_state6 = True


input_state1 = True
input_state2 = True
input_state3 = True
input_state4 = True
input_state5 = True
input_state6 = True

quit_video = True

player = False

while True:
    #Read states of inputs
    input_state1 = GPIO.input(17)
    input_state2 = GPIO.input(18)
    input_state3 = GPIO.input(27)
    input_state4 = GPIO.input(22)
    input_state5 = GPIO.input(23)
    input_state6 = GPIO.input(24)
    quit_video = GPIO.input(4)

    #If GPIO(17) is shorted to Ground
    if input_state1 != last_state1:
        if (player and not input_state1):
            os.system('killall omxplayer.bin')
            omxc = Popen(['omxplayer', '-b', movie1])
            player = True
        elif not input_state1:
            omxc = Popen(['omxplayer', '-b', movie1])
            player = True

    #If GPIO(18) is shorted to Ground
    elif input_state2 != last_state2:
        if (player and not input_state2):
            os.system('killall omxplayer.bin')
            omxc = Popen(['omxplayer', '-b', movie2])
            player = True
        elif not input_state2:
            omxc = Popen(['omxplayer', '-b', movie2])
            player = True

    #If GPIO(27) is shorted to Ground
    if input_state3 != last_state3:
        if (player and not input_state3):
            os.system('killall omxplayer.bin')
            omxc = Popen(['omxplayer', '-b', movie3])
            player = True
        elif not input_state3:
            omxc = Popen(['omxplayer', '-b', movie3])
            player = True

    #If GPIO(22) is shorted to Ground
    if input_state4 != last_state4:
        if (player and not input_state4):
            os.system('killall omxplayer.bin')
            omxc = Popen(['omxplayer', '-b', movie4])
            player = True
        elif not input_state4:
            omxc = Popen(['omxplayer', '-b', movie4])
            player = True

    #If GPIO(23) is shorted to Ground
    if input_state5 != last_state5:
        if (player and not input_state5):
            os.system('killall omxplayer.bin')
            omxc = Popen(['omxplayer', '-b', movie5])
            player = True
        elif not input_state5:
            omxc = Popen(['omxplayer', '-b', movie5])
            player = True

    #If GPIO(24) is shorted to Ground
    if input_state6 != last_state6:
        if (player and not input_state6):
            os.system('killall omxplayer.bin')
            omxc = Popen(['omxplayer', '-b', movie6])
            player = True
        elif not input_state6:
            omxc = Popen(['omxplayer', '-b', movie6])
            player = True

    #If omxplayer is running and GPIO are not shorted to Ground
    elif (player and input_state1 and input_state2 and input_state3 and input_state4 and input_state5 and input_state6):
        os.system('killall omxplayer.bin')
        player = False

    #GPIO(4) to close omxplayer manually - used during debug
    if quit_video == False:
        os.system('killall omxplayer.bin')
        player = False

    #Set last_input states
    last_state1 = input_state1
    last_state2 = input_state2
    last_state3 = input_state3
    last_state4 = input_state4
