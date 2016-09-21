from __future__ import division
#!/usr/bin/python
#==================================================
# bluePy.py
# 2016-09-18
#==================================================
#import serial
import time
import httplib
import os
import bluetooth
import sys
import random
from random import randint

#==================================================
# Read configuration
#==================================================
bd_addr     = "30:14:08:28:05:79"
port        = 1
sid         = 5
#==================================================

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
print 'Connected'

RED        = 1
BLUE       = 4
GREEN      = 2
YELLOW     = 3
MAGENTA    = 5
CYAN       = 6
WHITE      = 7
LED_OFF    = 0

TOWER_LEFT_ID  =  'L'
TOWER_RIGHT_ID =  'R'

# Color byte array is 16 long
def set_tower_colors(btSocket, towerSelection, colorSchema):
    packagedByteArray = bytearray()
    packagedByteArray.append(b"*")
    packagedByteArray.extend(colorSchema)
    packagedByteArray.append(towerSelection)
    btSocket.send(buffer(packagedByteArray))


def rainbowSchema():
    schema = []
    for i in xrange(16):
        schema.append(randint(1,7))
    return schema

colorSchemaYellowRed = [YELLOW, YELLOW,
                        YELLOW, YELLOW,
                        YELLOW, YELLOW,
                        YELLOW, YELLOW,
                        RED, RED,
                        RED, RED,
                        RED, RED,
                        RED, RED]

colorSchemaGreenBlue = [GREEN, GREEN,
                        GREEN, GREEN,
                        GREEN, GREEN,
                        GREEN, GREEN,
                        BLUE, BLUE,
                        BLUE, BLUE,
                        BLUE, BLUE,
                        BLUE, BLUE]


while 1:
    #test_list = [0]*16
    #power2_schema(test_list, 238)
    set_tower_colors(sock, TOWER_LEFT_ID, colorSchemaYellowRed)
    set_tower_colors(sock, TOWER_RIGHT_ID, colorSchemaGreenBlue)
    time.sleep(1)
    set_tower_colors(sock, TOWER_LEFT_ID, colorSchemaYellowRed[::-1])
    set_tower_colors(sock, TOWER_RIGHT_ID, colorSchemaGreenBlue[::-1])
    time.sleep(1)

    randomSchema = [randint(1,7)]*16
    set_tower_colors(sock, TOWER_LEFT_ID, randomSchema)
    randomSchema = [randint(1,7)]*16
    set_tower_colors(sock, TOWER_RIGHT_ID, randomSchema)
    
    time.sleep(1)

    set_tower_colors(sock, TOWER_LEFT_ID, rainbowSchema())
    set_tower_colors(sock, TOWER_RIGHT_ID, rainbowSchema())
    time.sleep(1)


sock.close()
# End of file
