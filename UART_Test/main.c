#include<stdlib.h>
#include<stdint.h>
#include"bsp.h"




void repeat_char(char* buf, int len){
    int i;
    char c = 'a';

    for(i = 0; i < len; i++)
        buf[i] = c;


}



int main(void){

    //initiate UART communication with the computer device
    UART_SETUP();
    char buf1[2];
    char buf2[9];
    char buf3[33];
    char buf4[65];
    char buf5[33];


    repeat_char(buf1, 2);
    repeat_char(buf2, 9);
    repeat_char(buf3, 33);
    repeat_char(buf4, 65);
    repeat_char(buf5, 33);


    UART_TX('1');
    int i = 0;
    for(; i<100; i++){
        print_str(buf1, 2);
        print_str(buf2, 9);
        print_str(buf3, 33);
        print_str(buf4, 65);
        print_str(buf5, 33);
    }
    UART_TX('0');


    return 0;
}
