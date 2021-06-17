import os
import sys
import serial
from subprocess import Popen

movie1 = ("/home/pi/Videos/movie1.mp4")
movie2 = ("/home/pi/Videos/movie2.mp4")
movie3 = ("/home/pi/Videos/movie3.mp4")
movie4 = ("/home/pi/Videos/movie4.mp4")
movie5 = ("/home/pi/Videos/movie5.mp4")
movie6 = ("/home/pi/Videos/movie6.mp4")

port = serial.Serial("/dev/ttyUSB0")

b0 = 'b0#'
b1 = 'b1#'
b2 = 'b2#'
b3 = 'b3#'
b4 = 'b4#'
b5 = 'b5#'
b6 = 'b6#'

while True:
    rcv = port.read(3)
    print rcv
    
    if rcv == b1:
	os.system('killall omxplayer.bin')
	rcv = b0
    	omxc = Popen(['omxplayer', '-b', movie1])
    
    if rcv == b2:
	os.system('killall omxplayer.bin')
    	omxc = Popen(['omxplayer', '-b', movie2])
    
    if rcv == b3:
	os.system('killall omxplayer.bin')
    	omxc = Popen(['omxplayer', '-b', movie3])
    
    if rcv == b4:
	os.system('killall omxplayer.bin')
    	omxc = Popen(['omxplayer', '-b', movie4])
    
    if rcv == b5:
	os.system('killall omxplayer.bin')
    	omxc = Popen(['omxplayer', '-b', movie5])
    
    if rcv == b6:
	os.system('killall omxplayer.bin')
    	omxc = Popen(['omxplayer', '-b', movie6])