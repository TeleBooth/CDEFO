import serial
from time import sleep

ser = serial.Serial(
    port='COM3',
    baudrate=9600,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
)

ser.isOpen()

while 1 :
    out = ''
    # let's wait one second before reading output (let's give device time to answer)

    sleep(1)

    while ser.inWaiting() > 0:
        out += str(ser.readline().decode("UTF-8"))

    if out != '':
        print(out)