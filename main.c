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

unsigned short matrixG[8], matrixR[8];

//requests Controller vectors
enum ReqState {REQ_INIT, REQ_WAIT} reqState;
unsigned long long ds3Vector;
unsigned short snesVector;
void reqInit(){
	reqState = REQ_INIT;
}

void reqTick(){
	//Actions
	switch(reqState){
		case REQ_INIT:
		    ds3Vector = 0;
            snesVector = 0;
		break;
        
        case REQ_WAIT:
            ;
            //send request for controller vectors
            unsigned char rumble = GetBit(PIND, 7) << 7;
            USART_Send(1 | rumble, 1);
            while(!USART_HasTransmitted(1));
            PORTA = 0xFF;
            //construct ds3Vector
            unsigned char i;
            for(i = 0; i < 8 ; i++){
                ds3Vector <<= 8;
                ds3Vector |= USART_Receive(1);
            }
            
            unsigned char snesHigh = USART_Receive(1);
            unsigned char snesLow = USART_Receive(1);
            snesVector = (snesHigh << 8) | snesLow;
            
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
   DDRA = 0xFF; PORTA = 0x00;
   DDRD = 0x00; PORTD = 0xFF;
   
   initUSART(1);
   
   //wait for bluetooth to synch
   unsigned char dummy = USART_Receive(1);

   //Start Tasks  
   StartReq(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}