#include "LedControl.h"
#include <SoftwareSerial.h>

int bluetoothTx = 2; //Bluetooth tx pin on arduino
int bluetoothRx = 3; //Bluetooth rx pin on arduino

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

byte RED        = B00000001;  // 1
byte BLUE       = B00000100;  // 4
byte GREEN      = B00000010;  // 2
byte YELLOW     = B00000011;  // 3
byte MAGENTA    = B00000101;  // 5
byte CYAN       = B00000110;  // 6
byte WHITE      = B00000111;  // 7
byte OFF        = B00000000;  // 0


/*
 * Helper function for setting color
 */
byte LED_ROW(byte ledOneColor, byte ledTwoColor) 
{
  return ((ledTwoColor << 5) + (ledOneColor << 2));
}


/*
 - MAX7219 driver 1
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 

 - MAX7219 driver 2
 pin 7 is connected to the DataIn 
 pin 6 is connected to the CLK 
 pin 5 is connected to LOAD 

- Left Servo pin
pin 9 is connected to servo control wire
- Left Servo pin
pin 3 is connected to servo control wire

- Bluetooth adapter HC-06
pin RX0 is connected to TXD
pin TX1 is connected to RXD 
 */

 // LED tower matrix with 2x8 RGB leds
   /*
    *   -----
    *  | O O | <- Row 7
    *  | O O |
    *  | O O |
    *  | O O | <- One LED has three color pins, so two rows of leds will correspond to 2x3 (6 columns)
    *  | O O |
    *  | O O |
    *  | O O |
    *  | O O | <- Row 0
    *  |  x  |
    *   -----
    */


const int LEFT_TOWER_MAX_IC = 0;
const int RIGHT_TOWER_MAX_IC = 1;
 
LedControl lc1  = LedControl(12,11,10,1);
LedControl lc2  = LedControl(7,6,5,1);

byte schemaTowerBuffer[8] =  {0};
int tstate =0;
int numBytes;
byte nbbuff[50] = {0};


/* Functions */

void setup() 
{
  Serial.begin(9600);
  bluetooth.begin(9600);

  /* -- Setup MAX7219 LED drivers -- */
  lc1.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc1.setIntensity(0,8);
  /* and clear the display */
  lc1.clearDisplay(0);

  lc2.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc2.setIntensity(0,8);
  /* and clear the display */
  lc2.clearDisplay(0);

}

void loop() 
{
  //delay(500);
  serialManager();
  //Other tasks;
  
}

void serialManager()
{
  int i=0,j=0;
  int tower = 0;
  int istart=0, istop=0;
  byte inByte = 0;

    if(bluetooth.available() > 0)
    {
      inByte = bluetooth.read();
      //Serial.print(inByte);Serial.println("-");
    }
    else
      return;

    if(inByte == '*')
    {
      tstate = 1;
      numBytes = 0;
      //Serial.println("State=1");
      //Serial.print("1numBytes=");Serial.println(numBytes);
    }
    if(inByte == 'L' && tstate == 1)
    {
      tstate = 2;
      tower  = LEFT_TOWER_MAX_IC; // Left
      //Serial.println("L State=2");
      //Serial.print("2numBytes=");Serial.println(numBytes);
    }
    if(inByte == 'R' && tstate == 1)
    {
      tstate = 2;
      tower  = RIGHT_TOWER_MAX_IC; // Right
      //Serial.println("R State=2");
      //Serial.print("2numBytes=");Serial.println(numBytes);
    }
    if(tstate == 1 && inByte != '*' && inByte != 'L'  && inByte != 'R')
    {
      nbbuff[numBytes] = (byte)inByte;
      numBytes++;
      //Serial.print(">numBytes=");Serial.println(numBytes);
    }
    
    if (tstate == 2)
    {
       tstate = 0;
       //Serial.print("numBytes=");Serial.println(numBytes);
       //Serial.print("tower=");Serial.println(tower);
       //if(numBytes >= 3 && numBytes <=18) 
       clearBuffer(schemaTowerBuffer);
       i = 0;
       byte odd = 0;
       byte even = 0;
       for(j=0; j<numBytes; j++ ) {
          //Serial.print("j=");Serial.print(j);Serial.print(" "); 
          if (j % 2)
          {
              //Serial.println(nbbuff[j]); 
              odd = nbbuff[j];
              //Serial.print("odd=");Serial.println(odd,DEC); 
              //Serial.println(even,HEX);Serial.println(odd,HEX); 
              schemaTowerBuffer[i] = LED_ROW(even, odd);
              i++;
          }
          else
          {
              Serial.println(nbbuff[j]); 
              even = nbbuff[j];
              //Serial.print("even=");Serial.println(even,DEC);
          }
       }
       numBytes = 0;
       setSchema(tower ,schemaTowerBuffer);
    }
  
}

void clearBuffer(byte * schema)
{
   for (int i=0; i < 8; i++)
   {
      schema[i] = 0;
   }
}



/*
 *  Function for converting schema to LED output
 */
void setSchema(int maxChipSelect, byte* schema)
{
   for(int i=0; i < 8; i++)
   {
      if (maxChipSelect == LEFT_TOWER_MAX_IC)
        lc1.setRow(0,7-i,schema[i]); 
      else
        lc2.setRow(0,7-i,schema[i]); 
   }
}


