#include "LedControl.h"
#include <Servo.h>

byte RED        = B00000001;
byte BLUE       = B00000100;
byte GREEN      = B00000010;
byte YELLOW     = B00000011;
byte MAGENTA    = B00000101;
byte CYAN       = B00000110;
byte WHITE      = B00000111;
byte OFF        = B00000000;

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

// schedules
const int MAX_ALLOWED_SCHEDULE_LENGTH = 5;
byte* scheduleLeftTower[MAX_ALLOWED_SCHEDULE_LENGTH] = {0};
byte* scheduleRightTower[MAX_ALLOWED_SCHEDULE_LENGTH] = {0};

void addSchemaToSchedule(int tower, int position, byte * schema)
{
  if(position < MAX_ALLOWED_SCHEDULE_LENGTH && position >= 0)
  {
    if(tower == LEFT_TOWER_MAX_IC)
      scheduleLeftTower[position] = schema;
    else
      scheduleRightTower[position] = schema;
  } 
}

//Schema display
byte schemaAllColorsTest[8] = {LED_ROW(RED, RED), 
                               LED_ROW(GREEN, GREEN), 
                               LED_ROW(BLUE, BLUE),
                               LED_ROW(YELLOW, YELLOW), 
                               LED_ROW(MAGENTA, MAGENTA), 
                               LED_ROW(CYAN, CYAN), 
                               LED_ROW(WHITE, WHITE), 
                               LED_ROW(OFF, OFF)}; 

byte schemaMixedRedHalfBlue[8] = {LED_ROW(RED, BLUE), 
                                 LED_ROW(BLUE, RED), 
                                 LED_ROW(RED, BLUE), 
                                 LED_ROW(BLUE, RED), 
                                 LED_ROW(RED, BLUE), 
                                 LED_ROW(BLUE, RED),
                                 LED_ROW(RED, BLUE), 
                                 LED_ROW(BLUE, RED)}; 

byte schemaHalfRedHalfBlue[8] = {LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(BLUE, BLUE), 
                                 LED_ROW(BLUE, BLUE),
                                 LED_ROW(BLUE, BLUE), 
                                 LED_ROW(BLUE, BLUE)};

byte schemaHalfBlueHalfRed[8] = {LED_ROW(BLUE, BLUE), 
                                 LED_ROW(BLUE, BLUE),
                                 LED_ROW(BLUE, BLUE), 
                                 LED_ROW(BLUE, BLUE),
                                 LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(YELLOW, YELLOW), 
                                 LED_ROW(YELLOW, YELLOW)};




Servo leftServo;
Servo rightServo;

/* global variables */
unsigned long delaytime=20;

int leftServoControlPin  = 9;
int rightServoControlPin = 3;

int initialPositionLeftServo  = 75;
int initialPositionRightServo = 75;

int currentPositionLeftServo   = initialPositionLeftServo;
int currrentPositionRightServo = initialPositionRightServo;

/* Functions */

void setup() 
{
  Serial.begin(9600);

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

  /* -- Setup Servo motors -- */
  //leftServo.attach(leftServoControlPin);
  
  //rightServo.attach(rightServoControlPin);s


  //setSchema(LEFT_TOWER_MAX_IC,schemaAllColorsTest);
  //setSchema(RIGHT_TOWER_MAX_IC,schemaHalfRedHalfBlue);

  addSchemaToSchedule(LEFT_TOWER_MAX_IC,0,schemaHalfBlueHalfRed);
  addSchemaToSchedule(LEFT_TOWER_MAX_IC,1,schemaHalfRedHalfBlue);

  addSchemaToSchedule(RIGHT_TOWER_MAX_IC,1,schemaHalfBlueHalfRed);
  addSchemaToSchedule(RIGHT_TOWER_MAX_IC,0,schemaHalfRedHalfBlue);
}

void loop() 
{
  serialManager();
  //Other tasks;
  
}

void serialManager()
{
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    //Set schema. s[byte1][byte2][byte3][byte4][byte5][byte6][byte7][byte8]
    if (inChar == 's')
    {
      runSchedule(500);
    }
    else if (inChar == 's')
    {
      runSchedule(1000);
    }
  }
}

byte ** getSchedule()
{
   while (Serial.available())
   {
      
   }
}

void runSchedule(int delayBetweenSchemas)
{
    for(int i=0; i < MAX_ALLOWED_SCHEDULE_LENGTH; i++)
    {
       int noMoreToShow = 0;
      
       if (scheduleLeftTower[i] != 0)
          setSchema(LEFT_TOWER_MAX_IC,scheduleLeftTower[i]);
       else 
         noMoreToShow++;
       if (scheduleRightTower[i] != 0)
          setSchema(RIGHT_TOWER_MAX_IC,scheduleRightTower[i]);
       else
          noMoreToShow++;

       // No more schedules defined for higher i. Break
       if (noMoreToShow == 2)
          break;
          
       delay(delayBetweenSchemas);
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


void rainbow()
{
  for (int i = 0; i < 8; i++)
  {
     for (int j = 1; j < 6; j++)
     {
        lc1.setLed(0,i,j,random(2));
        lc2.setLed(0,i,j,random(2));
        delay(delaytime);
     }
  }
}


void runMotor(int pos, int milliseconds, int servo) // Left servo = 0, right servo = 1
{
  int direction = 0;
  int currentPosition = 0;

   if (servo == 0)
   {
     currentPosition = currentPositionLeftServo;
   }
   else if (servo == 1)
   {
    currentPosition = currrentPositionRightServo;
   }

   if (pos < currentPosition)
  {
     direction = -1;
  }
  else if (pos > currentPosition)
  {
    direction = 1;
  }
  else
  {
    direction = 0;
  }

  while (pos != currentPosition)
  {
    currentPosition += direction;
    
   if (servo == 0)
   {
      leftServo.write(currentPosition);
      currentPositionLeftServo = currentPosition;
   }
   else if (servo == 1)
   {
      rightServo.write(currentPosition);
      currrentPositionRightServo = currentPosition;
   }

   delay(milliseconds);
   
  } 

}

