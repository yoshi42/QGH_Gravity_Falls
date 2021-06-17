import serial

while True:
    ser = serial.Serial("/dev/ttyUSB0")
    read = ser.read(3)
    print read 