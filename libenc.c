#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define CBC 1
#include "aes.h"

// ---------------------------------------------------------------------------
uint64_t libenc_random_seed() {
  uint64_t a = 0, d = 0;
  for(int i = 0; i < 100; i++) { asm volatile("nop"); }
  asm volatile("rdtsc" : "=a"(a), "=d"(d));
  return ((((d << 32) | a)) / 1);
}


// ---------------------------------------------------------------------------
size_t libenc_encrypt(const char* in, size_t len, char* out) {
    uint8_t key[16]; 
    
    srand(time(NULL));
    usleep(rand() % 4000000);
    asm volatile("lfence");
    size_t seed = libenc_random_seed(); 
    usleep(rand() % 4000000);
    
    srand(seed);
    for(int i = 0; i < 16; i++) {
        key[i] = rand() % 256;
    }
    
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    
    size_t blen = ((len + 15) / 16) * 16;
    uint8_t* data = (uint8_t*)malloc(blen * sizeof(uint8_t));
    memset(data, 0, blen);
    memcpy(data, in, len);
    
    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, data, blen);
    memcpy(out, data, blen);    
    free(data);
    return blen;
}


// ---------------------------------------------------------------------------
size_t libenc_decrypt(const char* in, size_t len, char* out, uint8_t key[16]) {
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

    size_t blen = ((len + 15) / 16) * 16;
    uint8_t* data = (uint8_t*)malloc(blen * sizeof(uint8_t));
    memset(data, 0, blen);
    memcpy(data, in, len);
    
    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, data, blen);
    memcpy(out, data, blen);
    free(data);
    return blen;
}


// ---------------------------------------------------------------------------
size_t libenc_get_token(char* token) {
#define _(x) plain_token[index++] = (#x)[0];
    char plain_token[64];
    int index = 0;
    asm volatile("jmp 1f; .byte 'T','O','K','E','N'; 1: nop");
    // this is just a test token that will be replaced with a real token on the testsystem
    _(S) _(C) _(A) _(D) _(T) _(O) _(K) _(E) _(N) _({) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(X) _(})
    if(token) {
        libenc_encrypt(plain_token, 64, token);
        return 1;
    }
    return 0;
}
