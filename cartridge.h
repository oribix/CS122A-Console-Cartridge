/*
 * cartridge.h
 *
 * Created: 12/9/2016 12:09:48 AM
 *  Author: Marco Morelos
 */ 


#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

//#include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <stdbool.h>
//#include <string.h>
//#include <math.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/eeprom.h>
//#include <avr/portpins.h>
//#include <avr/pgmspace.h>
//
////FreeRTOS include files
//#include "FreeRTOS.h"
//#include "task.h"
//#include "croutine.h"

#include "bit.h"
#include "usart_ATmega1284.h"

#define CTRLREQ 1
#define MTRXREQ 2

#define CARTRIDGE_TICK_DELAY 50

unsigned short matrixG[8], matrixR[8];

//opens a request for the controller vectors
//also signals ds3 to rumble or not
void requestControllers(unsigned char rumble){
    //create request bit vector
    unsigned char r = rumble << 7;
    unsigned char request = CTRLREQ | r;
    
    //send request for controller vectors
    USART_Send(request, 1);
    while(!USART_HasTransmitted(1));
}

void sendMatrix(unsigned short *matrix, unsigned char size){
    unsigned char i;
    for(i = 0; i < size; i++){
        unsigned char highByte = matrix[i] >> 8;
        unsigned char  lowByte = matrix[i] & 0xFF;
        USART_Send(highByte, 1);
        USART_Send(lowByte, 1);
    }
}

//requests Controller vectors
enum ReqState {REQ_INIT, REQ_WAIT} reqState;
unsigned long long ds3Vector;
unsigned short snesVector;
unsigned char rumble;

void reqInit(){
    reqState = REQ_INIT;
}

void reqTick(){
    //Actions
    switch(reqState){
        case REQ_INIT:
        ds3Vector = 0;
        snesVector = 0;
        rumble = 0;
        break;
        
        case REQ_WAIT:
        rumble = getButton(PIND, 7);
        requestControllers(rumble);
        
        //construct 8 byte ds3Vector
        ds3Vector = 0;
        unsigned char i;
        for(i = 0; i < 8 ; i++){
            ds3Vector <<= 8;
            ds3Vector |= USART_Receive(1);
        }
        
        //get SNES bit vector
        unsigned char snesHigh = USART_Receive(1);
        unsigned char snesLow = USART_Receive(1);
        snesVector = (snesHigh << 8) | snesLow;
        
        //sets the matrix to the last 16 bits of ds3Vector
        unsigned short buttonVector = ds3Vector & 0xFFFF;
        for(i = 0; i < 8; i++){
            matrixR[i] = buttonVector;
            matrixG[i] = buttonVector;
        }
        
        //send LED Matrix push request
        USART_Send(MTRXREQ, 1);
        while(!USART_HasTransmitted(1));
        
        //wait for the console to be ready
        USART_Receive(1);
        
        //push red LEDMatrix
        sendMatrix(matrixR, 8);
        
        ////push green LEDMatrix
        sendMatrix(matrixG, 8);
        
        while(!USART_HasTransmitted(1));
        break;
        
        default:
        
        break;
    }
    //Transitions
    switch(reqState){
        case REQ_INIT:
        reqState = REQ_WAIT;
        break;
        
        case REQ_WAIT:
        break;
        
        default:
        reqState = REQ_INIT;
        break;
    }
}

void cartridgeTask()
{
    reqInit();
    for(;;)
    {
        reqTick();
        vTaskDelay(CARTRIDGE_TICK_DELAY);
    }
}

void StartCartridge(unsigned portBASE_TYPE Priority)
{
    initUSART(1);
    xTaskCreate(cartridgeTask, (signed portCHAR *)"cartridgeTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

#endif /* CARTRIDGE_H_ */