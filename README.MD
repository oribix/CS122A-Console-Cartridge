# Console Cartridge
This is an API to create cartridges for the console.
This is built on [FreeRTOS](http://www.freertos.org/).

The cartridge is for the [DS3 clone console](https://github.com/oribix/CS122A-Console).
See the [Google Doc](https://drive.google.com/open?id=0B5K5OUHJGcLmLTlsRlRZWVctNUk) for more details on how this works.

[Video Demonstration](https://www.youtube.com/watch?v=FelWu7re3BA)

## How do I use this?
### To Start
You only need 2 lines of code to start:

`#include "cartridge.h"` and `StartCartridge(<priority>)`

Note that the priority must be set to the lowest out of all the tasks in your FreeRTOS system.
Failure to to do this will cause output bugs.

See main.c for an example.

### API Usage
The API has 5 variables for you to consider:
- `unsigned short matrixG[8]`
- `unsigned short matrixR[8]`
- `unsigned long long ds3Vector`
- `unsigned short snesVector`
- `bool rumble`

#### Controlling the LED Matrix
`matrixG` and `matrixR` are output variables.
Simply set this to whatever you want the matrix to output.
For example:

`matrixG[3] = 0xFFFF;`

This turns on all of the fourth row's green LEDs.
`matrixR` can be used in a similar fashion.

See main.c for an example.

#### Getting Controller inputs
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

#### Make the DS3 Clone vibrate
`rumble` is a bool.
Set it to `true` when you want the DS3 to vibrate.
The DS3 will vibrate until you set to to `false`

`rumble = true;//starts vibration`

`rumble  = false;//ends vibration`

See main.c for an example.

## File Descriptions

### cartridge.h
This holds all the code for the cartridge API.
See the "How do I use this" section for details on its use.

### main.c
This is an example of the cartridge API being used.

### bit.h
Binary manipulation helper functions.

### usart_ATmega1284.h
This is a driver for the ATMEGA1284's USART communication.

### FreeRTOS Files
- croutine.c
- heap_1.c
- list.c
- port.c
- queue.c
- tasks.c
- timer.h
- timers.c
