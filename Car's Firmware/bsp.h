#ifndef __BSP__
#define __BSP__

#define TIME_OUT_FLAG 0

void UART_TX(char c);
void UART_SETUP(void);
char UART_RX(void);
char Timed_UART_RX(void);
void print_str(char*,int);


#endif
