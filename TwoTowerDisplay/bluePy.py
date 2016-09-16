import bluetooth
import sys
bd_addr = "30:14:08:28:05:79" #itade address

port = 1
sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
print 'Connected'
sock.send("r")
print 'Sent data'
mode = raw_input('Enter your input:')
while mode is not 'q':
	if mode == 'r':
		sock.send("r")
	if mode == 's':
		sock.send("s")
	mode = raw_input('Enter your input:')

sock.close()
