// ***** 0. Documentation Section *****
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Ben Fridolin
// September 2, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// Initialize SysTick with busy wait running at bus clock.
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
	void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 10000us equals 10ms
void SysTick_Waitms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(8000000);  // wait 100ms board
  }
}

// ***** 2. Global Declarations Section *****
unsigned long S;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortB_Init(void);
void PortE_Init(void);
void PortF_Init (void);


// Linked data structure
struct State {
  unsigned long Out;
  unsigned long OutF;	
  unsigned long Time;  
  unsigned long Next[9];}; 
typedef const struct State STyp;

	#define goW       0
	#define waitW     1
	#define goS       2
  #define waitS     3
  #define walk      4
  #define dwalk     5
  #define woff      6
  #define dwalk2    7
  #define woff2     8

STyp FSM[9]={                                           // transition states
 {0x0C,0x02,10,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}}, 
 {0x14,0x02,10,{goS,goS,goS,goS,walk,walk,goS,goS}},
 {0x21,0x02,10,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS,}},
 {0x22,0x02,10,{goW,goW,goW,goW,walk,goW,walk,walk}},
 {0x24,0x08,10,{walk,dwalk,dwalk,dwalk,walk,dwalk,dwalk,dwalk}},
 {0x24,0x02,10,{dwalk,woff,woff,woff,woff,woff,woff,woff}},
 {0x24,0x00,10,{woff,dwalk2,dwalk2,dwalk2,dwalk2,dwalk2,dwalk2,dwalk2}},
 {0x24,0x02,10,{dwalk2,woff2,woff2,woff2,woff2,woff2,woff2,woff2}},
 {0x24,0x02,10,{woff2,goW,goS,goW,walk,goW,goS,goW}}};

	
// ***** 3. Subroutines Section *****

int main(void){ volatile unsigned long delay;
	unsigned long Input;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
  
	SysTick_Init();
	SYSCTL_RCGC2_R |= 0x32;      // 1) B E F
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  PortB_Init();                // 3) init PortB
	PortE_Init();                // 4) init PortE
	PortF_Init();                // 5) init PortF
  EnableInterrupts();
	S = goW;                    // initial state is goW
  while(1){
		GPIO_PORTB_DATA_R = FSM[S].Out;  // traffic lights
		GPIO_PORTF_DATA_R = FSM[S].OutF;   // pedestrian lights
    SysTick_Waitms(FSM[S].Time);        // wait
    Input = GPIO_PORTE_DATA_R&0x07;     // read sensors
    S = FSM[S].Next[Input]; 
     
  }
}
void PortE_Init(void) {
	GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) outputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
}

void PortB_Init(void) {
	GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
}

void PortF_Init(void) {
	GPIO_PORTF_AMSEL_R &= ~0x0A;        // 3) disable analog on PF3 and PF1 bits
	GPIO_PORTF_PCTL_R &= ~0x00001010;   // 4) Clear PCTL GPIO on PF3 and PF1 bits
	GPIO_PORTF_DIR_R |= 0x0A;          // 5) PF3 and PF1 out
	GPIO_PORTF_AFSEL_R &= ~0x0A;        // 6) disable alt funct on PF3 and PF1
	GPIO_PORTF_DEN_R |= 0x0A;          // 7) enable digital I/O on PF3 and PF1
	//GPIO_PORTF_DATA_R |= 0x0A;         // Set the PF3 and PF1 ON
}



