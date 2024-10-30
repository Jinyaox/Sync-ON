#include<string.h>
#include<stdint.h>
#include"sha256.h"
#include"bsp.h"


/*
 * Define global secrets for the fobs
 */
#define KEYLEN 32
#define RANDLEN 16

//initialize the keys here
char* carID="ABCDEFGHABCDEFGHABCDEFGHABCDEFGH";
char* prev_key="ABCDEFGHABCDEFGHABCDEFGHABCDEFGH"; //seed
char* curr_key="ABCDEFGHABCDEFGHABCDEFGHABCDEFGH"; //key


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


void initiation(char* dest_buffer){
    //send out "wake" message
    //print_str("wake", 4);



    //get the random number from the car
    // get_command(dest_buffer,16);
    return;
}


void identification(int prev_key, int curr_key,char* rand_buffer){
    /*
     * This is the initial challenge created by the key fob
     * The fob creates initial challenge for sync to the car (also initial verification)
     * tc_sha256_update (TCSha256State_t s, const uint8_t *data, size_t datalen);
     */
    char outbounding_buffer[KEYLEN];
    char xor_result[KEYLEN];
    char concatenation_buffer[KEYLEN+RANDLEN];


    strncpy(xor_result,prev_key,KEYLEN);
    XOR(curr_key,xor_result,KEYLEN);
    concatenation(xor_result,rand_buffer,concatenation_buffer,KEYLEN,RANDLEN);


    tc_sha256_update (&hasher,concatenation_buffer,KEYLEN+RANDLEN);
    tc_sha256_final(outbounding_buffer, &hasher);

    //send it out here
    //print_str(outbounding_buffer,KEYLEN);

    return;
}

void update_key(char* prev_key, char* curr_key, char* selected_rand){
    /*
     * the fob updates its key once it finished
     * Move onto the respond state
     */

    // change the key here
    strncpy(prev_key,curr_key,KEYLEN);
    strncpy(curr_key,selected_rand,KEYLEN);

    // update the SHA 256 of the next car ID
    tc_sha256_update (&hasher,carID,KEYLEN);
    tc_sha256_final(carID, &hasher);

    return;
}

int validation(int prev_key, int curr_key){
    /*
     * Once fob receive the response message from the car, it validates its integrity
     * The response from the car should be 64 bytes
     *
     * if validation fails, return 0;
     */
    char message_buffer[2*KEYLEN];
    char* seed_buf = message_buffer;
    char* r_buf = message_buffer+KEYLEN;

    //receive the message first by polling

    //get_command(message_buffer,64);

    //this is the stub
    strncpy(message_buffer,"ABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGH",2*KEYLEN);


    //extract seed and random next
    XOR(carID,seed_buf,KEYLEN); // this is the seed
    XOR(carID,r_buf,KEYLEN); //this is the r

    // compare the seed with the current one
    if (strncmp(prev_key,seed_buf,KEYLEN)==0){
        return 0;
    }

    update_key(prev_key,curr_key,r_buf);
    tc_sha256_update (&hasher,r_buf,KEYLEN);
    tc_sha256_final(r_buf, &hasher);


    //send out the r_buf
    print_str(r_buf,KEYLEN);

    return 1;
}






int main(void)
{
    tc_sha256_init(&hasher);

    //initiate UART communication with the computer device
    UART_SETUP();

    //allocate a buffer just for receiving and sending the initial wake
    char buffer[17] = "1234567812345678";



    while(1){
        int i=0;
        for(i=0;i<100000;i++){
            ;
        }
        print_str("1",1);
        for(i=0;i<1;i++){ //add this for loop just for measuring time.


            //initiation(buffer); //un-comment this one to test the transmission overhead

            identification(prev_key,curr_key,buffer);
            validation(prev_key,curr_key);
        }
        print_str("0",1);

    }
	return 0;
}
