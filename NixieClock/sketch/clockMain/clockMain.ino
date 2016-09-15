/*==================
 * Nixie Clock Driver
 * V.1
 *
 * Set time using Bluetooth module + android app (BlueArduino)
 * 
 * Copywrite aSaxen 
 * Adam Saxén 2014
 ==================*/
 
#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <DHT.h>

#define DHTPIN 13 
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

//Library for multiplexing the Nixie clock
#include <NixieAS.h>

//int pinLayout[] = {PINA_IC1, PINB_IC1, PINC_IC1, PIND_IC1, PINA_IC2, PINB_IC2, PINC_IC2, PIND_IC2, PAIR1, PAIR2, PAIR3}; 
int pinLayout[] = {5, 3, 2, 4, 9, 7, 6, 8, 12, 11, 10};

NixieAS nix(&pinLayout[0]);
int nums[] = {1,3,3,7,0,0};
unsigned char time_raw[7];


//Variable handle for real time clock object
Rtc_Pcf8563 rtc;

int pre_sec = 0;
int cur_sec = 0;

/*For storeing time and date from user*/
/*unsigned char time_date_raw[13];
int pos = 0;
char byteRead;*/

void setup() {                
  Serial.begin(9600);
  
  rtc.initClock();
  rtc.setDate(1, 3, 3, 7, 10);
  rtc.setTime(1, 13, 37);
  
  dht.begin();
  
}

void updRtcTime(){
  rtc.getTime();
  byte ss = rtc.getSecond();
  byte mm = rtc.getMinute();
  byte hh = rtc.getHour();
  nums[0] = hh / 10;
  nums[1]  = hh % 10;
  nums[2]  = mm / 10;
  nums[3]  = mm % 10;
  nums[4]  = ss / 10;
  nums[5]  = ss % 10;
}

void setRTC(){
  int dateTime[3];
  
  //Check if 0 in front of single day/month/year/hh/mm/ss . Ex: 01 02 03
   for(int i=0; i<3; i++){
   
      if(time_raw[2*i] == '0'){
        dateTime[i] = time_raw[2*i+1]-'0';
      }else{
        //Convert char to int with - '0'
        dateTime[i] = 10*(time_raw[2*i]-'0')+(time_raw[2*i+1]-'0');
      }     
   } 
  
   //rtc.setDate(dateTime[0], 0, dateTime[1], 0, dateTime[2]);
   rtc.setTime(dateTime[0], dateTime[1], dateTime[2]);  
   
   for(int i=0; i < 30; i++){
      nums[0] = random(0, 10);
      nums[1] = random(0, 10);
      nums[2] = random(0, 10);
      nums[3] = random(0, 10);
      nums[4] = random(0, 10);
      nums[5] = random(0, 10);
      nix.showDigits(&nums[0],1);
   }
     
}

void displayTempHumid(){
 
   int temp = (int)dht.readTemperature();
   int humi = (int)dht.readHumidity();
   
   for(int i=0; i < 180; i++){
      nums[0] = humi/10;
      nums[1] = humi%10;
      nums[2] = nums[2] = 0;
      nums[3] = nums[2] = 0;
      nums[4] = temp/10;
      nums[5] = temp%10;
      nix.showDigits(&nums[0],2);
   }
  
}

void readBluetooth(){
  int pos = 0;
  char byteRead;
  //Listen for user commands
  if(Serial.available()) {
    /* read the most recent byte */
    byteRead = Serial.read();
    //Trigger and capture data when char '#' is found
    if(byteRead == '#'){
      while(pos < 7 && Serial.available() > 0){
        byteRead = Serial.read();
        time_raw[pos] = byteRead;
        pos++;
        time_raw[pos] = '\0'; //Null terminate string - indicates end of string
      }      
      //Set the time and date after recieving all data from user
      setRTC();  
     }   
  }
  
}

void loop() { 
  
   
  //Se if bluetooth instructions are available
  readBluetooth();
  
  //Update digits for nixie based on RTC IC
  updRtcTime();
  
  
  //Display temperature and humidity every 15 sec
  
  if(abs((nums[4]*10+nums[5])-pre_sec) > 30){
    displayTempHumid();
    updRtcTime();
    pre_sec = (nums[4]*10+nums[5]);
  }
  
  //Call showDigits that will display nums. Every digit will be on for 2 ms
  nix.showDigits(&nums[0],2);
  
}
