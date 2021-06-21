#include "TM4C123GH6PM.h"
#include <stdio.h>

//prototypes
void SystemInit(void);
void Delay_ms(int n);
void portDriveSetup();
void forward();
void backward();
void right();
void left();
uint32_t Measure_distanceLeft(void);//left sensor calc
uint32_t Measure_distanceRight(void);//right sensor calc
void Timer0ACapture_init(void);
void Timer3ACapture_init(void);
void Delay_MicroSecond(int time);
void calculations();

/* global variables to store and display distance in cm */
uint32_t time; //left sensor time 
uint32_t timeTwo;//right sensor time
uint32_t distance;//left sensor distance calculated
uint32_t distanceTwo;//right sensor distance calculated

int main(void)
{
  portDriveSetup();
  Timer0ACapture_init();
  Timer3ACapture_init();
  while(1){
    forward();
    calculations(); 
    if(distance < 18 && distance < distanceTwo){
      for(int i =0; i<125; i++){
        right();
      }
    }
    else if(distanceTwo < 18 && distance > distanceTwo){
      for(int j =0 ;j<125; j++){
        left();
      }
    }
    else{
    for(int j =0 ;j<75; j++){
        forward();
      }
      
    }
  }	
}

void calculations(){
  time = Measure_distanceLeft(); //left sensor time
   timeTwo = Measure_distanceRight(); //right sensor time  
   distance = (time * 10625)/10000000; //left sensor distance
   distanceTwo = (timeTwo * 10625)/10000000;//right sensor distance, distance in cm
}

uint32_t Measure_distanceLeft(void)
{
    int lastEdge, thisEdge;
    GPIOA->DATA &= ~(1<<4); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIOA->DATA |= (1<<4); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIOA->DATA &= ~(1<<4); /* make trigger  pin low */
    
    while(1){
    TIMER0->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER0->RIS & 4) == 0) ;    /* wait till captured */
    if(GPIOB->DATA & (1<<6)){
    lastEdge = TIMER0->TAR;
    TIMER0->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER0->RIS & 4) == 0) ;    /* wait till captured */
    thisEdge = TIMER0->TAR;     /* save the timestamp */
    return (thisEdge - lastEdge); /* return the time difference */
      }
    }
}

uint32_t Measure_distanceRight(void)
{
    int lastEdge, thisEdge;
    GPIOA->DATA &= ~(1<<5); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIOA->DATA |= (1<<5); /* make trigger  pin high */
    Delay_MicroSecond(10); /*10 seconds delay */
    GPIOA->DATA &= ~(1<<5); /* make trigger  pin low */
    
    while(1){
    TIMER3->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER3->RIS & 4) == 0) ;    /* wait till captured */
    if(GPIOB->DATA & (1<<2)){
    lastEdge = TIMER3->TAR;
    TIMER3->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER3->RIS & 4) == 0) ;    /* wait till captured */
    thisEdge = TIMER3->TAR;     /* save the timestamp */
    return (thisEdge - lastEdge); /* return the time difference */
      }
    }
}


/* Timer0A initialization function */
/* Initialize Timer0A in input-edge time mode with up-count mode */
void Timer0ACapture_init(void)
{
    SYSCTL->RCGCTIMER |= 1;     /* enable clock to Timer Block 0 */
    SYSCTL->RCGCGPIO |= 2;      /* enable clock to PORTB */
    
    GPIOB->DIR &= ~0x40;        /* make PB6 an input pin */
    GPIOB->DEN |= 0x40;         /* make PB6 as digital pin */
    GPIOB->AFSEL |= 0x40;       /* use PB6 alternate function */
    GPIOB->PCTL &= ~0x0F000000;  /* configure PB6 for T0CCP0 */
    GPIOB->PCTL |= 0x07000000;
    
    SYSCTL->RCGCGPIO |= 1;      /* enable clock to PORTA */
    GPIOA->DIR |=(1<<4);         
    GPIOA->DEN |=(1<<4);         

    TIMER0->CTL &= ~1;          /* disable timer0A during setup */
    TIMER0->CFG = 4;            /* 16-bit timer mode */
    TIMER0->TAMR = 0x17;        /* up-count, edge-time, capture mode */
    TIMER0->CTL |=0x0C;        /* capture the rising edge */
    TIMER0->CTL |= (1<<0);           /* enable timer0A */
}

void Timer3ACapture_init(void){
    SYSCTL->RCGCTIMER |= (1<<3);  //PB2 is T3CCP0 timer 3     
    SYSCTL->RCGCGPIO |= 2; //port B      
    
    GPIOB->DIR &= ~0x04;        
    GPIOB->DEN |= 0x04;         
    GPIOB->AFSEL |= 0x04;      
    GPIOB->PCTL &= ~0x00000F00;  
    GPIOB->PCTL |= 0x00000700;
    
    SYSCTL->RCGCGPIO |= 1;     
    GPIOA->DIR |=(1<<5); //pa5         
    GPIOA->DEN |=(1<<5);         

    TIMER3->CTL &= ~1;        
    TIMER3->CFG = 4;           
    TIMER3->TAMR = 0x17;  
    TIMER3->CTL |=0x0C;        
    TIMER3->CTL |= (1<<0);        
}


void Delay_MicroSecond(int time)
{
    int i;
    SYSCTL->RCGCTIMER |= 2;     /* enable clock to Timer Block 1 */
    TIMER1->CTL = 0;            /* disable Timer before initialization */
    TIMER1->CFG = 0x04;         /* 16-bit option */ 
    TIMER1->TAMR = 0x02;        /* periodic mode and down-counter */
    TIMER1->TAILR = 16 - 1;  /* TimerA interval load value reg */
    TIMER1->ICR = 0x1;          /* clear the TimerA timeout flag */
    TIMER1->CTL |= 0x01;        /* enable Timer A after initialization */

    for(i = 0; i < time; i++)
    {
        while ((TIMER1->RIS & 0x1) == 0) ;      /* wait for TimerA timeout flag */
        TIMER1->ICR = 0x1;      /* clear the TimerA timeout flag */
    }
}

/* This function is called by the startup assembly code to perform system specific initialization tasks. */
void SystemInit(void)
{
    __disable_irq();    
    SCB->CPACR |= 0x00F00000;
}

void backward(){
    GPIOD->DATA = 0x08;
    GPIOC->DATA = 0x10;
    Delay_ms(3);
    GPIOD->DATA = 0x04;
    GPIOC->DATA = 0x20;
    Delay_ms(3);
    GPIOD->DATA = 0x02;
    GPIOC->DATA = 0x40;
    Delay_ms(3);
    GPIOD->DATA = 0x01;
    GPIOC->DATA = 0x80;
    Delay_ms(3);
}


void forward(){
    GPIOD->DATA = 0x01;
    GPIOC->DATA = 0x80;
    Delay_ms(3);
    GPIOD->DATA = 0x02;
    GPIOC->DATA = 0x40;
    Delay_ms(3);
    GPIOD->DATA = 0x04;
    GPIOC->DATA = 0x20;
    Delay_ms(3);
    GPIOD->DATA = 0x08;
    GPIOC->DATA = 0x10;
    Delay_ms(3);
}

void right(){
    GPIOD->DATA = 0x08;
    GPIOC->DATA = 0x80;
    Delay_ms(3);
    GPIOD->DATA = 0x04;
    GPIOC->DATA = 0x40;
    Delay_ms(3);
    GPIOD->DATA = 0x02;
    GPIOC->DATA = 0x20;
    Delay_ms(3);
    GPIOD->DATA = 0x01;
    GPIOC->DATA = 0x10;
    Delay_ms(3);
}

void left(){
    GPIOD->DATA = 0x01;
    GPIOC->DATA = 0x10;
    Delay_ms(3);
    GPIOD->DATA = 0x02;
    GPIOC->DATA = 0x20;
    Delay_ms(3);
    GPIOD->DATA = 0x04;
    GPIOC->DATA = 0x40;
    Delay_ms(3);
    GPIOD->DATA = 0x08;
    GPIOC->DATA = 0x80;
    Delay_ms(3);
}

void portDriveSetup(){
  
  SYSCTL->RCGCGPIO = 0x04; //pc4-7 for motor (used to be port B) 
  GPIOC->DIR = 0xF0; 
  GPIOC->DEN = 0xF0; 
 
  SYSCTL->RCGCGPIO |= (1<<3); //pd0-3
  GPIOD->DIR |= 0x0F; //PD0 is set for output
  GPIOD->DEN |= 0x0F; //Data enable for PD0

}

void Delay_ms(int n)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = (n*29000)-1;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x00000005;
    while ((SysTick->CTRL & 0x00010000)==0){}
}
