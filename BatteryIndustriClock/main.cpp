#ifndef F_CPU
#define F_CPU 8000000UL // Internal 8MHz clock
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define TASK1         1    // id number
#define TASK1_BEGIN   10   // start time
#define TASK1_END     22   // end time

#define TASK2         2
#define TASK2_BEGIN   33
#define TASK2_END     44

#define TASK3         3
#define TASK3_BEGIN   50
#define TASK3_END     59

#define RUN           1
#define FINISHED      0

#define MOTOR_PULSE_DURATION_MS  100

void rtc_init(void);
void gnight(void);

// Second counter
unsigned long seconds;

// Polarity
uint8_t direction = 0;

int main(void) {
    // Configure PORTB-pins
    DDRD = 0x07;

    rtc_init();  //initialise the timer
    sei();
    set_sleep_mode(SLEEP_MODE_PWR_SAVE); 

    unsigned int lastsecond = 0;

    // Loop forever
    while(1) {
    
    if ( seconds != lastsecond )
    {
        PORTD ^= 0b00000100;
        lastsecond = seconds;
    }
  
    if ( seconds == 0 )
    {
        // If one minute passed, pulse 2ms signal
        // We must also alternate polarity of the
        // pulse sent to the clock, so every other
        // time we pulse the other way of the H-bridge.
        //
        // 0bxxxxxx10 -> 0bxxxxxx01 -> 0bxxxxxx10 -> ...(contd.)
        //
        if ( direction == 0 ){
            PORTD |= 0x02;
            _delay_ms( MOTOR_PULSE_DURATION_MS );
            PORTD &= 0xFD;
            // Increment direction to alternate next time
            direction = 1;
        } else {
            PORTD |= 0x01;
            _delay_ms( MOTOR_PULSE_DURATION_MS );
            PORTD &= 0xFE;
            // Increment direction to alternate next time
            direction = 0;
        }
    }
    gnight();
    }
}

void rtc_init(void)
{  
    TCCR2A = 0x00;  //overflow
    TCCR2B = 0x05;  //5 gives 1 sec. prescale 
    TIMSK2 = 0x01;  //enable timer2A overflow interrupt
    ASSR   = 0x20;   //enable asynchronous mode
}

void gnight(void){
    sleep_enable();

    /**
     * There is a bug with the TIMER2 overflow interrupt and
     * going to sleep that causes the interrupt to fire twice,
     * thus making stuff not work properly. The solution is to
     * wait a bit (some clock cycles) until the timer counter
     * is no longer 0 (here less than 2).
     */
    if (TCNT2 == (uint8_t)0xff || TCNT2 == 0 || TCNT2 == 1)
    {
        while (TCNT2 < 2);
    }
    sleep_mode();
    sleep_disable();
}

ISR(TIMER2_OVF_vect)
{
    if(++seconds == 60){
      seconds = 0;
    }   
}