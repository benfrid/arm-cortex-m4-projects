// Runs on LM4F120/TM4C123
// Uses simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  

//   Author: Ben Fridolin
//   Date: 23/8/2016

// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

// Global variables
unsigned long In;  // input from PF4
unsigned long Out; // output to PF2 (blue LED)


// Function prototype
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void Delay(void);
void EnableInterrupts(void);  // Enable interrupts

int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
  // initialization goes here
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_AMSEL_R &= ~0x14;        // 3) disable analog on PF4 and PF2 bits
	GPIO_PORTF_PCTL_R &= ~0x00010100;   // 4) Clear PCTL GPIO on PF4 and PF2 bits
	GPIO_PORTF_DIR_R |= 0x04;          // 5) PF4 in, PF2 out
	GPIO_PORTF_AFSEL_R &= ~0x14;        // 6) disable alt funct on PF4 and PF2
	GPIO_PORTF_DEN_R |= 0x14;          // 7) enable digital I/O on PF4 and PF2
  GPIO_PORTF_PUR_R |= 0x10;          // enable pull-up on PF4 and PF2
	GPIO_PORTF_DATA_R |= 0x04;         // Set the PF2 bit in Port F DATA so the LED is initially ON
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    // body goes here
		Delay();
		In = GPIO_PORTF_DATA_R&0x10;       // Read the switch and test if the switch is pressed
		if (In==0x00)  {                      // If switch is pressed PF4=1
			Out = GPIO_PORTF_DATA_R;  //  toggle PF2 (flip bit from 0 to 1, or from 1 to 0)
			Out = Out^0x04;
			GPIO_PORTF_DATA_R = Out;
			}
		if (In!=0x00)  {                        // If switch is not pressed PF4=0
			GPIO_PORTF_DATA_R |= 0x04;            // set PF2, so LED is ON
			}
	}
}	
	
void Delay(void){unsigned long volatile time;
  time = 666666;  // 0.1sec (100ms/12.5ns/11)
  while(time){
		time--;
  }
}