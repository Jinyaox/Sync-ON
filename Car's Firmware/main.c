#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"sha256.h"
#include <stdbool.h>
#include"bsp.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"

#define ADC_SEQUENCER_LENGTH 1
/*
 * Define global secrets for the fobs
 */
#define KEYLEN 32
#define RANDLEN 128

//initialize the keys here
char* carID="ABCDEFGHABCDEFGHABCDEFGHABCDEFGH";
char* prev_key="ABCDEFGHABCDEFGHABCDEFGHABCDEFGH";//seed
char* curr_key="ABCDEFGHABCDEFGHABCDEFGHABCDEFGH";//key


static struct tc_sha256_state_struct hasher;


void initADC(void){
  //enable the adc0 peripherial.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  //set the speed to 1msps.
  //SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS);
  //set the auto avergage to 64.
  ADCHardwareOversampleConfigure(ADC0_BASE, 64);
  //before setting up I must disable the sequence 3.
  ADCSequenceDisable(ADC0_BASE, 3);
  //set the sequence to use (adc0 sequence 3).
  ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  //set up the sequence step.
  //set up the last step and start an interrupt when the conversion it's over.
  ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
  //enable the sequence again!
  ADCSequenceEnable(ADC0_BASE, 3);
}


void get_command(char buf[],int max_len){
  char c;
  int counter=0;
  do{
    c=UART_RX();
    buf[counter]=c;
    counter++;
  }while(counter<max_len);
  return;
}

int timed_get_command(char buf[],int max_len){
    char c;
    int counter=0;
    c = Timed_UART_RX();
    if (TIME_OUT_FLAG==1){
        TIME_OUT_FLAG=0;
        return -1;
    }
    else{
        buf[counter]=c;
        counter++;
    }
    do{
    c=UART_RX();
    buf[counter]=c;
    counter++;
    }while(counter<max_len);
    return 0;
}



void XOR(char* source, char* target, int length){
    /*
     * The XOR function takes two strings and XOR them bitwise
     * Result will be saved into the target buffer
     */
    int i=0;
    for (i=0;i<length;i++){
        target[i]=source[i] ^ target[i];
    }
    return;
}

void concatenation(char* source, char* source2, char* dest, int length1,  int length2){
    strncpy(dest,source,length1);
    strncpy(dest+length1,source2,length2);
    return;
}

void generate_random_number(char* rand_buffer, int length){
    //int buffer_length=length/4;
    uint32_t buffer[8];
    uint32_t ulADC0Value[ADC_SEQUENCER_LENGTH];
    uint32_t ulTemp_ValueC=0;

    int i;

    for(i=0;i<length/4;i++){
        initADC();
        ADCIntClear(ADC0_BASE, 3);
        //trigger the adc conversion process.
        ADCProcessorTrigger(ADC0_BASE, 3);
        //wait for the interrupt flag to get set!
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
           ;
        }
        //get the actual data samples from adc0 sequencer 3!
        ADCSequenceDataGet(ADC0_BASE, 3, ulADC0Value);
        //convert the value!
        //  TEMP = 147.5 - ((75 * (VREFP - VREFN) * ADCVALUE) / 4096)
        //  3.3V *10 * 75 = 2475 //13.3.6 of the TM4C123GH6PM datasheet

        buffer[i]=(1475 -((2475 * ulADC0Value[0])) / 4096)/10;
    }
    strncpy(rand_buffer,buffer,length);
}



int identification(){
    char rand_buffer[KEYLEN];
    char xor_result[KEYLEN];
    char concatenation_buffer[KEYLEN+RANDLEN];
    char outbounding_buffer[KEYLEN];
    //receive response from Fob, h(key^seed||rand)
    char response_buffer[KEYLEN];
    //generate a buffer of random value
    generate_random_number(rand_buffer, KEYLEN);
    //send random number as challenge
    print_str(rand_buffer, KEYLEN);



    //verify received response
    get_command(response_buffer, KEYLEN);
    strncpy(xor_result,prev_key,KEYLEN);
    XOR(curr_key,xor_result,KEYLEN);
    concatenation(xor_result,rand_buffer,concatenation_buffer,KEYLEN,RANDLEN);

    tc_sha256_update (&hasher,concatenation_buffer,KEYLEN);
    tc_sha256_final(outbounding_buffer, &hasher);
    // compare the seed with the current one and return the result of strncmp
    return strncmp(outbounding_buffer,response_buffer,KEYLEN);
}


int validation(){
    char message_buffer[2*KEYLEN];
    char* seed_buf = message_buffer;
    char* r_buf = message_buffer+KEYLEN;
    char h_rand_buf[KEYLEN];
    char rand1[KEYLEN];

    //get_command(message_buffer,64);
    generate_random_number(rand1, KEYLEN);
    strncpy(seed_buf,prev_key,KEYLEN);
    strncpy(r_buf, rand1,KEYLEN);
    // seed^ID
    XOR(carID,seed_buf,KEYLEN);
    //rand1^ID
    XOR(carID,r_buf,KEYLEN);
    print_str(message_buffer, 2*KEYLEN);

    if (timed_get_command(h_rand_buf,KEYLEN) == -1)
        return -1;
    else{
        tc_sha256_update (&hasher,rand1,KEYLEN);
        tc_sha256_final(rand1, &hasher);
        if (strncmp(h_rand_buf,rand1,KEYLEN) == 0)
            update_key();
        else
            return 1;
    }
}



void update_key(char* rand1){
    /*
     * the fob updates its key once it finished
     * Move onto the respond state
     */

    // change the key here
    strncpy(prev_key,curr_key,KEYLEN);
    strncpy(curr_key,rand1,KEYLEN);

    // update the SHA 256 of the next car ID
    tc_sha256_update (&hasher,carID,KEYLEN);
    tc_sha256_final(carID, &hasher);

    return;
}



int main(void)
{
    tc_sha256_init(&hasher);

    //initiate UART communication with the computer device
    UART_SETUP();
    
    //allocate a buffer just for receiving and sending the initial wake
    char buffer[128];

    while(1){
        SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

        int i=0;
        for(i=0;i<100000;i++){
            ;
        }
        print_str("1",1);
        for(i=0;i<1;i++){ //add this for loop just for measuring time.
            if (identification() == 0)
                validation();
        }
        print_str("0",1);
    }
	return 0;
}
