import serial
import serial.tools.list_ports
import time

port = serial.tools.list_ports.comports()[0].device
ser = serial.Serial(port, baudrate=115200, timeout=1)

signals = [
	[1] * 1024,
	[1] * 1024,
	[1] * 1024,
	[1] * 1024,
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