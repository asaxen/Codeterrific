#include "Arduino.h"
#include "NixieAS.h"



NixieAS::NixieAS(int* pinLayout)
{
    _pinLayout = pinLayout;

    for(int i = 0; i < NUMBEROFPINS; i++){
        pinMode(_pinLayout[i], OUTPUT); 
    }

    //Set all anod switches to LOW (off) 
    digitalWrite(_pinLayout[8],LOW);
    digitalWrite(_pinLayout[9],LOW);
    digitalWrite(_pinLayout[10],LOW);
    
}

/* setNumber function
 * Paramters: num, chan(0,1)
 * Description: Sends binary combination to K155ID1 IC based on the
 * variable num. Variable chan (channel) determines if IC1 or IC2 should recieve the data.
*/

void NixieAS::setDigit(int num, int chan){
     switch(num){
      case 0:
        digitalWrite(_pinLayout[chan*4],LOW);   //PIN A
        digitalWrite(_pinLayout[chan*4+1],LOW); //PIN B
        digitalWrite(_pinLayout[chan*4+2],LOW); //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW); //PIN D 
      break;
      case 1:
        digitalWrite(_pinLayout[chan*4], HIGH); //PIN A
        digitalWrite(_pinLayout[chan*4+1],LOW); //PIN B
        digitalWrite(_pinLayout[chan*4+2],LOW); //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW); //PIN D 
      break;
      case 2:
        digitalWrite(_pinLayout[chan*4],  LOW);  //PIN A
        digitalWrite(_pinLayout[chan*4+1],HIGH); //PIN B
        digitalWrite(_pinLayout[chan*4+2],LOW);  //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW);  //PIN D 
      break;
      case 3:
        digitalWrite(_pinLayout[chan*4], HIGH);  //PIN A
        digitalWrite(_pinLayout[chan*4+1],HIGH); //PIN B
        digitalWrite(_pinLayout[chan*4+2],LOW);  //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW);  //PIN D 
      break;
      case 4:
        digitalWrite(_pinLayout[chan*4],  LOW);   //PIN A
        digitalWrite(_pinLayout[chan*4+1],LOW);   //PIN B
        digitalWrite(_pinLayout[chan*4+2],HIGH);  //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW);   //PIN D 
      break;
      case 5:
        digitalWrite(_pinLayout[chan*4],  HIGH);  //PIN A
        digitalWrite(_pinLayout[chan*4+1],LOW);   //PIN B
        digitalWrite(_pinLayout[chan*4+2],HIGH);  //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW);   //PIN D 
      break;
      case 6:
        digitalWrite(_pinLayout[chan*4],  LOW);   //PIN A
        digitalWrite(_pinLayout[chan*4+1],HIGH);  //PIN B
        digitalWrite(_pinLayout[chan*4+2],HIGH);  //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW);   //PIN D 
      break;      
      case 7:
        digitalWrite(_pinLayout[chan*4],  HIGH);   //PIN A
        digitalWrite(_pinLayout[chan*4+1],HIGH);   //PIN B
        digitalWrite(_pinLayout[chan*4+2],HIGH);   //PIN C
        digitalWrite(_pinLayout[chan*4+3],LOW);    //PIN D 
      
      break;
      case 8:
        digitalWrite(_pinLayout[chan*4],  LOW);   //PIN A
        digitalWrite(_pinLayout[chan*4+1],LOW);   //PIN B
        digitalWrite(_pinLayout[chan*4+2],LOW);   //PIN C
        digitalWrite(_pinLayout[chan*4+3],HIGH);  //PIN D 
      
      break;
      case 9:
        digitalWrite(_pinLayout[chan*4],  HIGH);   //PIN A
        digitalWrite(_pinLayout[chan*4+1],LOW);    //PIN B
        digitalWrite(_pinLayout[chan*4+2],LOW);    //PIN C
        digitalWrite(_pinLayout[chan*4+3],HIGH);   //PIN D 
      break;      
    }
    
}

/* showdigits - Function for multiplexing
 * Parameters: int* digits (pointer to array of digits), int ms_d (display time / digit in milliseconds)
 * Description: Multiplexes by turning digits on and of in three pairs.
 *
 * Position of the digits on the Nixie clock (hh,mm,ss)
 * =============================================
 * =   ---   ---     ---   ---     ---   ---   =                  
 * =  | A | | B |   | C | | D |   | E | | F |  =                
 * =   ---   ---     ---   ---     ---   ---   =                 
 * =============================================
 *
 * Pairs(1,2,3) for multiplexing. One pair at a time on 
 * =============================================
 * =   ---   ---     ---   ---     ---   ---   =                  
 * =  | 1 | | 2 |   | 3 | | 1 |   | 2 | | 3 |  =                
 * =   ---   ---     ---   ---     ---   ---   =                 
 * =============================================
*/
void NixieAS::showDigits(int* digits, int ms_d){
	
	int iterations = 50/(ms_d*3+5);
	
	for(int i=0; i<iterations; i++){
		
		/*== Digit Pair 1 ==*/
		setDigit(digits[0],0); //Digit A
		setDigit(digits[3],1); //Digit D
		
		//Switch for pair 1
		digitalWrite(_pinLayout[8],HIGH); 
			delay(ms_d);
		digitalWrite(_pinLayout[8],LOW);

		/*== Digit Pair 2 ==*/
		setDigit(digits[1],0); //Digit B
		setDigit(digits[4],1); //Digit E
		
		//Switch for pair 1
		digitalWrite(_pinLayout[9],HIGH); 
			delay(ms_d);
		digitalWrite(_pinLayout[9],LOW);
		
		/*== Digit Pair 3 ==*/
		setDigit(digits[2],0); //Digit C
		setDigit(digits[5],1); //Digit F
		
		//Switch for pair 1
		digitalWrite(_pinLayout[10],HIGH); 
			delay(ms_d);
		digitalWrite(_pinLayout[10],LOW);
	}

}