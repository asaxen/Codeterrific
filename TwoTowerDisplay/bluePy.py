from __future__ import division
#!/usr/bin/python
#==================================================
# bluePy.py
# 2016-09-18
#==================================================
import serial
import time
import httplib
import os
import bluetooth
import sys

#==================================================
# Read configuration
#==================================================
g_debug     = 'YES';
g_server    = 'data.nabton.com'
g_path      = '/index.php'
g_delay     = 2
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

colorSchemaOne = [RED, BLUE,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  GREEN, GREEN]

colorSchemaTwo = [RED, BLUE,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  GREEN, GREEN,
                  BLUE, BLUE,
                  GREEN, GREEN,
                  BLUE, BLUE,
                  GREEN, GREEN]

previousSchemaLeftTower = [LED_OFF]*16
previousSchemaRightTower = [LED_OFF]*16


def power_schema(ledList, wattValue):
    wattLevels = [0,200,400,600,800,1000,2000,3000,10000]
    diff = 0

    i = 0
    for level in wattLevels:
        if wattValue < level:
            diff = level-wattLevels[i-1]
            break
        i += 1

    # Divide into 8 intervals over the range 0 to diff
    wattPerInterval = (diff/8) 

    remainingWatt = wattValue-wattLevels[i-1]

    odd_numbers = [15,13,11,9,7,5,3,1]
    # Should only be value between 1 and 8. (index 0-7)
    currentWattLED = int(remainingWatt/wattPerInterval)
    for j in odd_numbers:
        ledList[j] = WHITE

    print "CurrentWattLed:" + str(currentWattLED) + " RemWatt:" + str(remainingWatt) + " wattInter:" + str(wattPerInterval)

    if currentWattLED <= 8.0 and currentWattLED >= 0.0:
        if currentWattLED == 8:
            currentWattLED = 7
        index_led = odd_numbers[int(currentWattLED)]
        ledList[index_led] = BLUE
    else:
        ledList[15] = YELLOW

    if wattValue >= wattLevels[1]:
        ledList[12] = GREEN
    if wattValue >= wattLevels[2]:
        ledList[10] = GREEN
    if wattValue >= wattLevels[3]:
        ledList[8] = GREEN
    if wattValue >= wattLevels[4]:
        ledList[6] = YELLOW
    if wattValue >= wattLevels[5]:
        ledList[4] = YELLOW
    if wattValue >= wattLevels[6]:
        ledList[2] = RED
    if wattValue >= wattLevels[7]:
        ledList[0] = RED


# Color byte array is 16 long
def set_tower_colors(btSocket, towerSelection, colorByteArray):
    packagedByteArray = bytearray()
    packagedByteArray.append(b"*")
    packagedByteArray.extend(colorByteArray)
    packagedByteArray.append(towerSelection)
    btSocket.send(buffer(packagedByteArray))

def status_to_color(status):
    if status is 1:
        return MAGENTA
    if status is 2:
        return YELLOW
    if status is 3:
        return RED

def display_status_and_value(displaySchema, status, value):
    #Set status LED
    ledList = [0]*16
    ledList[14] = status_to_color(status)

    #Set value
    displaySchema(ledList, value)
    return ledList

def update_tower(towerSelection, sid, displaySchema, previousSchema):
    nb_value = 0
    req = g_path+ '?mid=2'+'&nsid=1'+'&sid1=' + str(sid)  
    conn = httplib.HTTPConnection(g_server)
    previousSchema[14] = MAGENTA
    set_tower_colors(sock, towerSelection, previousSchema)
    try:
        conn.request("GET", req)
        try:
            r1 = conn.getresponse()
            if g_debug == 'YES':
                print ("Server Response:-_- %s %s " % (r1.status, r1.reason))
            if r1.status == 200:
                status = 1
                data1 = r1.read()
                print data1
                par = data1.split(':')
                print par[1]
                noDec = par[1].split('.')
                print noDec[0]
                nb_value = noDec[0]
                ledSchema = display_status_and_value(displaySchema, status, int(nb_value))
                print previousSchema
                previousSchema[:] = ledSchema
                ledSchema[14] = LED_OFF
                set_tower_colors(sock, towerSelection, ledSchema)
        except:
            print "Unexpected error:", sys.exc_info()[0]
            print '-_- No response from nb server'
            status = 2
    except:
        print "Unexpected error:", sys.exc_info()[0]
        print '-_- Not able to connect to server '+g_server
        status = 3

    if status > 1:
        previousSchema[14] = status_to_color(status)
        set_tower_colors(sock, towerSelection, previousSchema)
    conn.close()
    

     
while 1:
    #test_list = [0]*16
    #power2_schema(test_list, 238)
    update_tower(TOWER_LEFT_ID, 5, power_schema, previousSchemaLeftTower)
    update_tower(TOWER_RIGHT_ID, 302, power_schema, previousSchemaRightTower)
    time.sleep(g_delay)
sock.close()
# End of file
