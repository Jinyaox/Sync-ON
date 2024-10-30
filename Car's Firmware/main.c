#include<stdlib.h>
#include<stdint.h>
#include"sha256.h"
#include"bsp.h"


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

void concatenation(char* source, char* source2, char* dest, int length1, int length2){
    strncpy(dest,source,length1);
    strncpy(dest+length1,source2,length2);
    return;
}

void generate_random_number(char* rand_buffer, int length){
    return;
}



void identification(){
    char rand_buffer[KEYLEN];
    char xor_result[KEYLEN];
    char concatenation_buffer[KEYLEN+RANDLEN];
    char outbounding_buffer[KEYLEN];

    //generate a buffer of random value
    generate_random_number(rand_buffer, KEYLEN);
    //send random number as challenge
    print_str(rand_buffer, KEYLEN);
    //receive response from Fob, h(key^seed||rand)
    response_buffer[KEYLEN];

    //verify received response
    // get_command(response_buffer, KEYLEN);
    strncpy(xor_result,prev_key,KEYLEN);
    XOR(curr_key,xor_result,KEYLEN);
    concatenation(xor_result,rand_buffer,concatenation_buffer,KEYLEN,RANDLEN);

    tc_sha256_update (&hasher,concatenation_buffer,KEYLEN);
    tc_sha256_final(outbounding_buffer, &hasher);
    // compare the seed with the current one and return the result of strncmp
    return strncmp(outbounding_buffer,response_buffer,KEYLEN);
}


void validation(){
    char message_buffer[2*KEYLEN];
    char* seed_buf = message_buffer;
    char* r_buf = message_buffer+KEYLEN;

    //get_command(message_buffer,64);
    generate_random_number(r_buf, KEYLEN);
    strncpy(seed_buf,prev_key,KEYLEN);
    // seed^ID
    XOR(carID,seed_buf,KEYLEN);
    //rand1^ID
    XOR(carID,r_buf,KEYLEN);
    print_str(message_buffer, 2*KEYLEN);
}



void update_key(char* prev_key, char* curr_key, char* selected_rand){
    /*
     * the fob updates its key once it finished
     * Move onto the respond state
     */

    // change the key here
    strncpy(prev_key,curr_key,32);
    strncpy(curr_key,selected_rand,32);

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
        int i=0;
        for(i=0;i<100000;i++){
            ;
        }
        print_str("1",1);
        for(i=0;i<1;i++){ //add this for loop just for measuring time.


            //initiation(buffer); //un-comment this one to test the transmission overhead

            if (identification() == 0)
                //if valication is successful, validation which send out seed^ID, rand^ID, h(rand)
                validation();
                //TODO: h(rand)
            else
                //if the check fails, redo the verification process
                identification();
        }
        print_str("0",1);

    }
	return 0;
}
