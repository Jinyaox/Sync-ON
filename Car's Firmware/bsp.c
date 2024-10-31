#include "tm4c123gh6pm.h"
#include "stdint.h"


void GPIOA_SETUP(){
  SYSCTL_RCGCGPIO_R|=SYSCTL_RCGCGPIO_R0;
  while(!((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R0)==SYSCTL_RCGCGPIO_R0));
  UART0_CTL_R &=(~UART_CTL_UARTEN);
  GPIO_PORTA_DEN_R=0x03U;
  GPIO_PORTA_AFSEL_R=0x03U;
  GPIO_PORTA_PCTL_R=0x11;
  
  //end of GPIOA configuration
}

void UART_SETUP(){
  SYSCTL_RCGCUART_R|=0x01;
  while(!((SYSCTL_RCGCUART_R & SYSCTL_RCGCUART_R0)==SYSCTL_RCGCUART_R0));
  GPIOA_SETUP();

  UART0_CTL_R &=(~UART_CTL_UARTEN);


  //set baud rate
  UART0_IBRD_R=104;
  UART0_FBRD_R=11;
  
  //set bit, parity, stopbit, ~FIFO
  UART0_CC_R=0; //use default clock
  UART0_LCRH_R=0x60;
  
  UART0_CTL_R|=(UART_CTL_RXE|UART_CTL_TXE|UART_CTL_UARTEN);
}

void UART_TX(char c){
  while((UART0_FR_R & 0x20) !=0);
  UART0_DR_R=c;
}

char UART_RX(void)  {    
  char data;  
  while((UART0_FR_R & (1<<4)) != 0); /* wait until Rx buffer is not full */    
  data = UART0_DR_R ;  /* before giving it another byte */    
  return (unsigned char) data; 
}

char Timed_UART_RX(void)  {    
  char data;
  for(i=0;i<10000;i++){;}  /*start a time window*/
  if ((UART0_FR_R & (1<<4)) != 0){ /*if Rx buffer is full*/
    TIME_OUT_FLAG = 1;
    return (unsigned char) 0;
    }
  else{
    data = UART0_DR_R ;  /* before giving it another byte */    
    return (unsigned char) data; 
  }
}

void print_str(char* str,int n){
  int cursor=0;
  for(cursor=0;cursor<n;cursor++){
      UART_TX(str[cursor]);
  }
  return;
}
