import serial
import serial.tools.list_ports
import time

for comport in list(serial.tools.list_ports.comports()):
    if "FTDI" in comport[2]:
        #Assumption: this is the only FTDI device
        port = comport[0]
print("Using serial port", port)
ser = serial.Serial(port, baudrate=115200, timeout=1)

ch = 0
freq = 1000000 #output frequency

ch_signal = ([0] * 521) + [1, 0, 1] + ([1] * 521)
nsamples = len(ch_signal) #Number of samples

#Configure the sample rate on the selected channel
ser.write('set_freq {} {}\n'.format(ch, freq).encode('utf-8'))

time.sleep(1)

#Set the stop address for the channel: number of samples - 1
ser.write('set_stop_addr {} {}\n'.format(ch,nsamples-1).encode('utf-8'))
time.sleep(1)

#Get ready to load raw values
ch_mask = (1 << ch) #Channel mask
cmd='load {:X} 0 {}\n'.format(ch_mask, nsamples).encode('utf-8')
ser.write(cmd)
for addr, val in enumerate(ch_signal):
    #Now write the raw bits
    print('CH {:X} ADDR {}, {}'.format(ch, addr, val))
    #Shift by channel number since the position of the bit determines which
    #channel this bit is for it is
    ser.write(bytes(val<<ch)) 

#Debugging - force clear the input buffer?
ser.write(bytes(0)) 
ser.write("\n".encode('utf-8'))

print("Number of samples: ", nsamples)

input('Press enter to play signal...')

ser.write('play {}\n'.format(ch).encode('utf-8'))

rsp=''
while not rsp.endswith("OK"):
    c = ser.read(1).decode('utf-8')
    print(c, end='')
    rsp += c
