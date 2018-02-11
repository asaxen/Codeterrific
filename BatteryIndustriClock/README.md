# Interrupt RTC timer code
Code for ATMEGA328 that will do the following:
- Use a 32.768 kHz crystal to trigger a ISR with 1 Hz
- Every minute will toggle two GPIO


## Build and load
```powershell
$file='main';avr-gcc -g -Os -mmcu=atmega328 "$file.cpp" -o "$file.out"; avr-objcopy -j .text -j .data -O ihex ".\$file.out" "$file.hex";avrdude.exe -c stk500v2 -p atmega328 -P COM3 -U flash:w:$file.hex:i
```
