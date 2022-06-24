#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "libenc.h"
#include <pthread.h>
#include <unistd.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_WHITE   "\x1b[0m"
#define NUM_THREADS 20
unsigned long threshold;
size_t crct_time=0;

uint64_t timestamp();
uint64_t cache_hit();
uint64_t cache_miss();
void maccess(void* p);
void flush_cache(void* p);
void *monitor(void *arg);
uint64_t flush_reload();

void hexprint(char* in, size_t len) {
    for(size_t i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)(in[i]));
    }
}


void main() {
    unsigned char token[64], plain_token[64], key[16];
    size_t t_m,t_h,fn_time=0,seed=0,j=0;
    int rc;
    pthread_t threads[NUM_THREADS];
    
    
    printf(COLOR_CYAN);
    printf("_________                      _____            ________     ______              \n");
    printf("__  ____/___________  ___________  /______      ___  __/________  /_____________ \n");
    printf("_  /    __  ___/_  / / /__  __ \\  __/  __ \\     __  /  _  __ \\_  //_/  _ \\_  __ \\\n");
    printf("/ /___  _  /   _  /_/ /__  /_/ / /_ / /_/ /     _  /   / /_/ /  ,<  /  __/  / / /\n");
    printf("\\____/  /_/    _\\__, / _  .___/\\__/ \\____/      /_/    \\____//_/|_| \\___//_/ /_/ \n");
    printf("               /____/  /_/                                                       \n\n");
    printf(COLOR_RESET);
    
    printf("[" COLOR_RED "*" COLOR_RESET "] Generating encrypted token...\n");
    
    t_h=cache_hit();
	t_m=cache_miss();
	printf("\n Time difference = time_miss= %zu ,time hit = %zu",t_m,t_h );
	threshold=(t_h+t_m)/2;
	//threshold=t_h+30;
	printf("\n Threshold = %zu \n",threshold);
    
    int *funcPtr = (int*)libenc_random_seed+44;
    flush_cache(funcPtr);
  
    rc = pthread_create(&threads[j], NULL,monitor, NULL);
     
    usleep(1); 
    libenc_get_token(token);

    printf("[" COLOR_YELLOW "*" COLOR_RESET "] Encrypted token: " COLOR_YELLOW);
    hexprint(token, 64);
    printf(COLOR_RESET "\n");
    
    strcpy(plain_token,"<TODO>" );
    // TODO: find plaintext token
    int z=0;
	printf("########crct time = %zu", crct_time);
    for (size_t l= crct_time-30490;l<crct_time+1000;l++){
		z++;;
		srand(l);
		for(int i = 0; i < 16; i++) {
			key[i] = rand() % 256;
		}

		libenc_decrypt(token,64,plain_token, key);

		if(plain_token[0]=='S' && plain_token[1]=='C'&& plain_token[2]=='A'&&plain_token[3]=='D'){
			printf("z of sscad %d\n",z);
			for(int n=0;n<64;n++){
				printf("%c",plain_token[n]);
			}
			break;
    
		}
  
   } 
    
    printf("[" COLOR_GREEN "*" COLOR_RESET "] Key: " COLOR_GREEN);
    hexprint(key, 16);
    printf(COLOR_RESET "\n");
    printf("[" COLOR_GREEN "*" COLOR_RESET "] Plain token: " COLOR_GREEN "%s" COLOR_RESET "\n", plain_token);
}

//=================================================================================================================================

void *monitor(void *arg){

	unsigned long fn_time=0,before_flush,before_sleep;
	int *funcPtr = (int*)libenc_random_seed+44;
	flush_cache(funcPtr);
	for(int i=0;i<8032000;i++){
		usleep(1);
		fn_time=flush_reload();
  
		if(fn_time<threshold){
			break;
		}
	}
	pthread_exit(NULL);
	return NULL;
}



//========================================================================================================================

uint64_t flush_reload(){
 
	size_t start,end,time=0,delta=0,seed_time=0;
	int i =0,j=0;

	int *funcPtr = (int*)libenc_random_seed+44;

	asm volatile("mfence");
	start = timestamp();
	asm volatile("mfence");

	maccess(funcPtr);
  
	asm volatile("mfence");
	end = timestamp();
	asm volatile("mfence");

	flush_cache(funcPtr);
	delta=end-start;


	if(delta<threshold){
		crct_time=start;
		printf("*************************success someone accessed: %ld***************************************\n",delta);
	}

	return delta;
}

//===================================================================================================================


uint64_t cache_hit(){
  
	size_t start=0,end=0,time=0,delta=0;
	int i =0,j=0;
	int *funcPtr = (int*)libenc_random_seed+44;
	maccess(funcPtr);
	for (j = 0; j < 100; ++j){
		asm volatile("mfence");
		start = timestamp();
		asm volatile("mfence");

		maccess(funcPtr);
  
		asm volatile("mfence");
		end = timestamp();
		asm volatile("mfence");
		delta=end-start;
		time = time+end-start;

	}
    return time/100;
}

//=======================================================================================================================
 uint64_t cache_miss(){
  
	size_t start=0,end=0,time=0,delta=0;
	int i =0,j=0;

	for (j = 0; j < 100; ++j){
		int *funcPtr = (int*)libenc_random_seed+44;
		flush_cache(funcPtr);
		asm volatile("mfence");
		start = timestamp();
		asm volatile("mfence");

		maccess(funcPtr);
  
		asm volatile("mfence");
		end = timestamp();
		asm volatile("mfence");
		delta=end-start;
		time = time+end-start;

	}
    return time/100;
}
//===============================================================================================================================
uint64_t timestamp() {
	uint64_t a = 0, d = 0;
	//for(int i = 0; i < 100; i++) { asm volatile("nop"); }
	asm volatile("rdtsc" : "=a"(a), "=d"(d));
	return ((((d << 32) | a)) / 1);
}

void flush_cache(void* p) {
    asm volatile ("clflush 0(%0)\n"
      :
      : "r" (p)
      : "rax");
     //   asm volatile("mfence");

      
}

void maccess(void* p)
{
  asm volatile ("movq (%0), %%rax\n"
    :
    : "c" (p)
    : "rax");
}
