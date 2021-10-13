// ***** 0. Documentation Section *****
// SwitchLEDInterface.c
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.

//   Author: Ben Fridolin
//   Date: 28 August, 2016

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
unsigned long In;
unsigned long Out;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void Delay(void);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
int main(void)
{
  //**********************************************************************
  // The following version tests input on PE0 and output on PE1
  //**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1); // activate grader and set system clock to 80 MHz

  PortE_Init();
  EnableInterrupts(); // enable interrupts for the grader
  while (1)
  {
    Delay();
    In = GPIO_PORTE_DATA_R & 0x01; // Read and test if the switch pressed
    if (In == 0x00)
    {                            // If switch not pressed
      GPIO_PORTE_DATA_R |= 0x02; // LED ON
    }
    else
    {                          // If switch pressed
      Out = GPIO_PORTE_DATA_R; // toggle LED
      Out = Out ^ 0x02;
      GPIO_PORTE_DATA_R = Out;
    }
  }
}
void PortE_Init(void)
{
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;    // 1) E clock
  delay = SYSCTL_RCGC2_R;          // delay
  GPIO_PORTE_AMSEL_R &= 0x00;      // 3) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000; // 4) GPIO clear bit PCTL
  GPIO_PORTE_DIR_R &= ~0x11;       // 5.1) PE0 input,
  GPIO_PORTE_DIR_R |= 0x02;        // 5.2) PE1 output
  GPIO_PORTE_AFSEL_R &= 0x00;      // 6) no alternate function
  GPIO_PORTE_DEN_R |= 0x03;        // 7) enable digital pins PE0 and PE1
  GPIO_PORTE_DATA_R |= 0x02;       //   LED is initially ON
}

void Delay(void)
{
  unsigned long volatile time;
  time = 600000; // 0.1sec approx (83ms/12.5ns/11)
  while (time)
  {
    time--;
  }
}
