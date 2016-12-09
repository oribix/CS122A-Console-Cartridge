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

#include "bit.h"
#include "usart_ATmega1284.h"

#define CTRLREQ 1
#define MTRXREQ 2

unsigned short matrixG[8], matrixR[8];

//opens a request for the controller vectors
//also signals ds3 to rumble or not
void requestControllers(){
    //create request bit vector
    unsigned char rumble = getButton(PIND, 7) << 7;
    unsigned char request = CTRLREQ | rumble;
    
    //send request for controller vectors
    USART_Send(request, 1);
    while(!USART_HasTransmitted(1));
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
            requestControllers();
            //receive response
            PORTC = USART_Receive(1);
            
            ////construct 8 byte ds3Vector
            //unsigned char i;
            //for(i = 0; i < 8 ; i++){
                //ds3Vector <<= 8;
                //ds3Vector |= USART_Receive(1);
            //}
            //
            //PORTC = ds3Vector & 0xFF;
            
            //unsigned char snesHigh = USART_Receive(1);
            //unsigned char snesLow = USART_Receive(1);
            //snesVector = (snesHigh << 8) | snesLow;
            
            //unsigned short buttonVector = ds3Vector & 0xFFFF;
            //for(i = 0; i < 8; i++){
            //    matrixR[i] = buttonVector;
            //    matrixG[i] = buttonVector;
            //}
            //
            ////send LED Matrix push request
            //USART_Send(2, 1);
            //while(!USART_HasTransmitted(1));
            //
            ////wait for the console to be ready
            //unsigned char dummy = USART_Receive(1);
            //
            ////push red LEDMatrix
            //for(i = 0; i < 8; i++){
            //    USART_Send(matrixR[i], 1);
            //}
            //
            ////push green LEDMatrix
            //for(i = 0; i < 8; i++){
            //    USART_Send(matrixG[i], 1);
            //}
            //while(!USART_HasTransmitted(1));
            
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

void reqTask()
{
	reqInit();
   for(;;) 
   { 	
	reqTick();
	vTaskDelay(50); 
   } 
}

void StartReq(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(reqTask, (signed portCHAR *)"reqTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void)
{
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0x00; PORTD = 0xFF;
    
    initUSART(1);
    
    //Start Tasks
    StartReq(1);
    //RunSchedular
    vTaskStartScheduler();
    
    return 0;
}