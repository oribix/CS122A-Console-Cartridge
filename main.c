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

void requestControlVectors(bool rumble){
    
}

//requests Controller vectors
enum ReqState {REQ_INIT, REQ_WAIT} reqState;

void reqInit(){
	reqState = REQ_INIT;
}

void reqTick(){
	//Actions
	switch(reqState){
		case REQ_INIT:
		
		break;
        
        case REQ_WAIT:
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
   
   initUSART(1);
   
   //wait for bluetooth to synch
   while(!USART_HasReceived(1));
   USART_Flush(1);
   
   //Start Tasks  
   StartReq(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}