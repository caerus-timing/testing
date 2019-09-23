import serial
import serial.tools.list_ports
import time

for comport in list(serial.tools.list_ports.comports()):
    if "FTDI" in comport[2]:
        #Assumption: this is the only FTDI device
        port = comport[0]
ser = serial.Serial(port, baudrate=115200, timeout=1)

signals = [
	[0, 0, 1, 0, 1, 1, 0, 1] * 128,
	[0, 1, 0, 1, 1, 0, 1, 0] * 128,
	[0, 0, 0, 0, 0, 0, 0, 1] * 128,
	[0, 1, 1, 0, 0, 0, 0, 1] * 128,
]

for ch, _ in enumerate(signals):
	ser.write('set_rec_mode {}\n'.format(ch).encode('utf-8'))

time.sleep(1)

for ch, ch_signal in enumerate(signals):
	print('CH:', ch)
	ser.write('en_wram {}\n'.format(ch).encode('utf-8'))
	time.sleep(1)

	for addr, val in enumerate(ch_signal):
		print('CH {} ADDR {}, {}'.format(ch, addr, val))
		ser.write('wram {} {} {}\n'.format(ch, addr, val).encode('utf-8'))

		time.sleep(0.01)

	ser.write('set_addr {} 0\n'.format(ch).encode('utf-8'))
	ser.write('dis_wram {}\n'.format(ch).encode('utf-8'))

	ser.write('set_freq {} 100000\n'.format(ch).encode('utf-8'))

for ch, _ in enumerate(signals):
	ser.write('en_ch {}\n'.format(ch).encode('utf-8'))

for ch, _ in enumerate(signals):
	ser.write('set_play_mode {}\n'.format(ch).encode('utf-8'))
