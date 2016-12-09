#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

#include "cartridge.h"

int main(void)
{
    DDRC = 0xFF; PORTC = 0x00; //used for debugging
    DDRD = 0x00; PORTD = 0xFF; //used for rumble button
    
    //Start Tasks
    StartCartridge(1);
    
    //RunSchedular
    vTaskStartScheduler();
    
    return 0;
}