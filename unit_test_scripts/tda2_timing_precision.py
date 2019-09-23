import serial
import serial.tools.list_ports
import time

for comport in list(serial.tools.list_ports.comports()):
    if "FTDI" in comport[2]:
        #Assumption: this is the only FTDI device
        port = comport[0]
ser = serial.Serial(port, baudrate=115200, timeout=1)

ch_signal = ([0] * 521) + [1, 0, 1] + ([1] * 521)

for ch in range(4):
	ser.write('set_rec_mode {}\n'.format(ch).encode('utf-8'))

time.sleep(1)

ch = 0
ser.write('en_wram {}\n'.format(ch).encode('utf-8'))
time.sleep(1)

for addr, val in enumerate(ch_signal):
	print('CH {} ADDR {}, {}'.format(ch, addr, val))
	ser.write('wram {} {} {}\n'.format(ch, addr, val).encode('utf-8'))

	time.sleep(0.01)

ser.write('set_addr {} 0\n'.format(ch).encode('utf-8'))
ser.write('dis_wram {}\n'.format(ch).encode('utf-8'))

ser.write('set_freq {} 1000000\n'.format(ch).encode('utf-8'))

ser.write('en_ch {}\n'.format(ch).encode('utf-8'))

input('Press enter to play signal...')

ser.write('set_play_mode {}\n'.format(ch).encode('utf-8'))
