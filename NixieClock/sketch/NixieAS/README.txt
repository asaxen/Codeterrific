===== NixieAS library =====
Author: Adam Saxén
Date: 2014-09-21
Contact: adam.saxen@gmail.com

This library is to be used when multiplexing 2x3 nixie tube systems, hence 6 nixie tube in total.
The pin_layout can be modified, but requires that you define 3 digital pins for the 3 pair switches + 8 pins as input for the two K155ID1 
drivers.

The library contains two function: showDigits(), setDigit().

