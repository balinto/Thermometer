/*
**************************************************************************************************************
*                 Temperature reader
*
* filename:	temp_reader.c
* created:	Agnes Gellen, Balint Erdohegyi, 2013. 
*
* prerequisites:
*			- WinAVR 20071221 is installed in the root of drive C (C:\WinAVR-20071221)
*
* function:	- 7 segment display shows the temperature while pressing a button, read every ~1 sec in IT routine
*			- btn1: minimum temperature
*			- btn2: actual temperature
*			- btn3: maximum temperature
*
**************************************************************************************************************
*/


#include "mcu_avr_atmega128_api.h" 	// MCU API   
#include "dpy_trm_s01.h"			// DPY API

unsigned char	led_counter=0;
unsigned char	err;
float			temp_sensor;
float 			min=99;
float 			max=0;
unsigned char	but1, but2, but3;
int				cntr=0;


/********  function prototypes  ***************************/
void Timer0_Init(void);
ISR(SIG_OVERFLOW0);
int main(void);

/********  Timer0 overflow IT Service Routine  ***************************/
ISR(SIG_OVERFLOW0) // Timer0 overflow
{
   led_counter++; 
   if (led_counter<63) DPY_TRM_S01__LED_4_OFF();
   else if (led_counter<126) DPY_TRM_S01__LED_4_ON();
   else {
		err=dpy_trm_s01__Temp_ReadTEMP(&temp_sensor,TMP75_JUMPER_OFF,TMP75_JUMPER_OFF,TMP75_JUMPER_OFF);	// Reads the temperature sensor
		if (temp_sensor<min) min = temp_sensor;		// Checks if new temperature is lower than min
		else if (temp_sensor>max) max = temp_sensor;		// Checks if new temperature is higher than max
		
		led_counter=0;
		}
}

/********  main program  ***************************/
int main (void)
{
   dpy_trm_s01__Init();		// Initialize the DPY dysplay card
   Timer0_Init();			// Initialize timer0
   dpy_trm_s01__Temp_Init(TMP75_JUMPER_OFF,TMP75_JUMPER_OFF,TMP75_JUMPER_OFF); //Initialisation of temp. sensor
   SYS_LED_DIR_OUTPUT();	// Set the pin driving the system led to output
   SYS_LED_ON();			// Switch on system led
   sei();					// enable interrupts
   temp_sensor=24.3;
   while(1)
   {
	but1=DPY_TRM_S01__BUTTON_1_GET_STATE();
	but2=DPY_TRM_S01__BUTTON_2_GET_STATE();
	but3=DPY_TRM_S01__BUTTON_3_GET_STATE();
	if(cntr >= 1000){
		if (!but3) dpy_trm_s01__7seq_write_number(min,1);				// Displaying the min temperature on the 7seg display
		else if (!but2) dpy_trm_s01__7seq_write_number(temp_sensor,1);	// Displaying the actual temperature on the 7seg display
		else if (!but1) dpy_trm_s01__7seq_write_number(max,1);			// Displaying the max temperature on the 7seg display
		else dpy_trm_s01__7seq_clear_dpy();						// Clearing the 7seg display
		cntr=0;
		}
	cntr++;
   }
}


/********  Timer0 initialisation  ***************************/
void Timer0_Init(void)
{
   TCCR0=0x06;			// Set TIMER0 prescaler to CLK/256 -> 32.768kHz/256=128Hz
   TCNT0=0;				// Set the counter initial value                    
   TIMSK=_BV(TOIE0);	// Enable TIMER0 overflow interrupt                 
}

