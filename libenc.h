#ifndef _LIBENC_H_
#define _LIBENC_H_
#include <stdint.h>


/**
 * Encrypt a string with a random key.
 * 
 * The function takes a C string as input and encrypts the string using a random key. 
 * The random number generator is seeded using libenc_random_seed(), and then called 
 * 16 times to get 16 random bytes for the 128-bit AES key. Encryption mode is CBC.
 * 
 * @param in The string to encrypt. The size of the input must be a multiple of 16 bytes.
 * @param len The exact length of the input string in bytes.
 * @param out The buffer to store the encrypted string. The size must be 'len' rounded up to the next multiple of 16 bytes. 
 * @return Number of bytes written to the output buffer
 */
size_t libenc_encrypt(const char* in, size_t len, char* out);


/**
 * Decrypt a string with a given key.
 * 
 * The function takes an encrypted C string as input and decrypts the string using the provided key. 
 * This function decrypts strings encrypted with libenc_encrypt(). 
 * 
 * @param in The string to decrypt. The size of the input must be a multiple of 16 bytes.
 * @param len The exact length of the input string in bytes.
 * @param out The buffer to store the encrypted string. The size must be 'len' rounded up to the next multiple of 16 bytes. 
 * @param key The 128-bit AES decryption key, provided as an array of 16 unsigned characters.
 * @return Number of bytes written to the output buffer
 */
size_t libenc_decrypt(const char* in, size_t len, char* out, uint8_t key[16]);


/**
 * Get a seed value for generating a random key. 
 * 
 * The function returns a 64-bit seed based on the timestamp counter of the CPU. 
 * This value is used to seed the random number generator used for deriving the encryption key.
 * 
 * @return 64-bit random seed
 * 
 */
uint64_t libenc_random_seed();

/**
 * Get an encrypted token.
 * 
 * This function encrypts an inaccessible secret token with a random key and return the encrypted token.
 * Internally, it relies on the function libenc_encrypt() to encrypt the token.
 * While the token itself is unknown and inaccessible, the structure of the token is known to be "SCADTOKEN{[A-Za-z0-9]+}"
 * 
 * @param token Buffer to store the encrypted token. Its size has to be at least 64 bytes.
 */
size_t libenc_get_token(char* token);

 
#endif
