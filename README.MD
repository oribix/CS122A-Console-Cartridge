#Console Cartridge
This is an API to create cartridges for the console.
This is built on [FreeRTOS](http://www.freertos.org/).

See the [console](https://github.com/oribix/CS122A-Console)
and [Google Doc]() for more details on how this works.

##How do I use this?
###To Start
You only need 2 lines of code to start:

`#include "cartridge.h"` and `StartCartridge(<priority>)`

See main.c for an example.

###API Usage
The API has 5 variables for you to consider:
- `unsigned short matrixG[8]`
- `unsigned short matrixR[8]`
- `unsigned long long ds3Vector`
- `unsigned short snesVector`
- `bool rumble`

####Controlling the LED Matrix
`matrixG` and `matrixR` are output variables.
Simply set this to whatever you want the matrix to output.
For example:

`matrixG[3] = 0xFFFF;`

This turns on all of the fourth row's green LEDs.
`matrixR` can be used in a similar fashion.

See for main.c an example.

####Getting Controller inputs
The API automatically fethes controller inputs and stores them in `ds3Vector` and `snesVector`.
Simply read these whenever you want inputs.

```
unsigned long long player1Input = ds3Vector;
unsigned short player2Input = snesVector;
```

For information on how to parse the ds3Vector input,
see the [DUALSHOCK3 clone project page](https://github.com/oribix/DualShock3-Clone-ATMEGA1284).

The snes input has a similar input vector.
The difference is that the SNES controller only has 12 buttons and nothing else.
The bit vector from Most Significant Bit to Least Significant Bit is:

`B Y Sl St Up Dn <- -> A X L R - - - -`

The bits that have dashes(-) have no use in the snes bit vector.

####Make the DS3 Clone vibrate
`rumble` is a bool.
Set it to `true` when you want the DS3 to vibrate.
The DS3 will vibrate until you set to to `false`

`rumble = true;//starts vibration`

`rumble  = false;//ends vibration`

See main.c for an example.
