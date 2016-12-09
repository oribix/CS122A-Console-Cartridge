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

void setRowBothColor(unsigned short data, unsigned char row){
    if(row >= 8) return;
    
    matrixR[row] = data;
    matrixG[row] = data;
}

enum DS3EchoState{DS3ECHO_INIT, DS3ECHO_WAIT} ds3EchoState;
unsigned short buttonVector;

void ds3EchoInit(){
    ds3EchoState = DS3ECHO_INIT;
}

void ds3EchoTick(){
    //State actions
    switch(ds3EchoState){
        case DS3ECHO_INIT:
            buttonVector = 0;
        break;
        
        case DS3ECHO_WAIT:
        //rumble request. Rumbles as long as rumble == true
        rumble = getButton(PIND, 7);
        
        //sets the matrix to the last 16 bits of ds3Vector
        buttonVector = ds3Vector & 0xFFFF;
        
        //set matrix row 0 to ds3ButtonVector
        unsigned char row = 0;
        setRowBothColor(buttonVector, row);
        
        //set matrix rows to values of thumb sticks
        for(row = 1; row < 5; row++){
            unsigned char shiftNum = 16 + (6 * (row - 1));
            unsigned short data = (ds3Vector >> shiftNum) & 0x3F;
            setRowBothColor(data, row);
        }
        
        //set matrix rows to values of accelerometer
        for(; row < 8; row++){
            unsigned char shiftNum = 8 * row;
            unsigned short data = (ds3Vector >> shiftNum) & 0xFF;
            setRowBothColor(data, row);
        }
        
        break;
        
        default:
        break;
    }
    
    //Transitions
    switch(ds3EchoState){
        case DS3ECHO_INIT:
        ds3EchoState = DS3ECHO_WAIT;
        break;
        
        case DS3ECHO_WAIT:
        break;
        
        default:
        ds3EchoState = DS3ECHO_INIT;
        break;
    }
}

void ds3EchoTask()
{
    ds3EchoInit();
    for(;;)
    {
        ds3EchoTick();
        vTaskDelay(50);
    }
}

void StartDS3Echo(unsigned portBASE_TYPE Priority)
{
    xTaskCreate(ds3EchoTask, (signed portCHAR *)"ds3EchoTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
    DDRC = 0xFF; PORTC = 0x00; //used for debugging
    DDRD = 0x00; PORTD = 0xFF; //used for rumble button
    
    //Start Tasks
    StartDS3Echo(1);
    StartCartridge(2); //MUST BE LOWEST PRIORITY
    //RunSchedular
    vTaskStartScheduler();
    
    return 0;
}