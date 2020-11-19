#include <Arduino.h>

#include "log.h"
#include "com.h"
#include "params.h"
#include "status.h"

unsigned char public_key[CRYPTO_KEM_PUBLICKEYBYTES];
unsigned char secret_key[CRYPTO_KEM_SECRETKEYBYTES];
unsigned char ciphertext[CRYPTO_KEM_CIPHERTEXTBYTES];
unsigned char session_key[CRYPTO_KEM_BYTES];

unsigned char verification_key[CRYPTO_SIGN_PUBLICKEYBYTES];
unsigned char signature_key[CRYPTO_SIGN_SECRETKEYBYTES];
unsigned char signed_message[MESSAGE_LENGTH];
unsigned char smlenBuffer[SMLEN_BYTES];

unsigned long current_time;
unsigned long time_kem_keypair;
unsigned long time_sign_keypair;
unsigned long time_sign;
unsigned long time_kem_dec;

size_t smlen[1];
size_t pkb;

void setup(){
  log_begin();
  com_begin();

  init_lights();
  set_lights(LEDGREEN+LEDYELLOW);
  while (log_interface_ready() == 0);

  // Generate keys and sign message
  current_time = micros();
  crypto_kem_keypair(public_key, secret_key);
  time_kem_keypair = micros() - current_time;

  current_time = micros();
  crypto_sign_keypair(verification_key, signature_key);
  time_sign_keypair = micros() - current_time;

  current_time = micros();
  crypto_sign(signed_message, smlen, public_key, CRYPTO_KEM_PUBLICKEYBYTES, signature_key);
  time_sign = micros() - current_time;

  memcpy(smlenBuffer, smlen, SMLEN_BYTES);

  // Set LED, and wait for program to be started
  set_lights(LEDGREEN);
  log_message("------------ ALICE ------------\n");
  log_message("Key exchange initiated.\n");
  set_lights(LEDYELLOW);

  log_message("\nSigned public key:\n");
  log_print_bytes(signed_message, 256, 32);
  log_message("...\n");

  // Send information
  com_send_burst(smlenBuffer, SMLEN_BYTES, 256);
  com_send_burst(verification_key, CRYPTO_SIGN_PUBLICKEYBYTES, 256);
  com_send_burst(signed_message, *smlen, 256);
  log_message("\nSent signed public key to Bob.\n");

  // Get ciphertext
  log_message("\nWaiting for encapsulated session key from Bob ...\n");
  while (com_interface_ready() == 0);
  com_read_string(ciphertext, CRYPTO_KEM_CIPHERTEXTBYTES);
  log_message("\nReceived ciphertext from Bob.\n");

  // Decapsulate ciphertext
  current_time = micros();
  crypto_kem_dec(session_key, ciphertext, secret_key);
  time_kem_dec = micros() - current_time;

  // Print
  log_message("\nCiphertext:\n");
  log_print_bytes(ciphertext, 256, 32);
  log_message("...\n");

  log_message("Session key:\n");
  log_print_bytes(session_key, CRYPTO_KEM_BYTES, 32);

  set_lights(LEDGREEN);

  // Set session key
  prepare_aes(session_key, CRYPTO_KEM_BYTES);
//  gcm.setKey(session_key, CRYPTO_KEM_BYTES);

  log_message("\nKEM scheme: ");
  log_message(CRYPTO_KEM_ALGNAME);
  log_message("\nSignature scheme: ");
  log_message(CRYPTO_SIGN_ALGNAME);

  log_message("\n\nTIME CONSUMPTION (microseconds)");
  log_message("\nGenerate keypair (signature):\t  ");
  log_message(time_sign_keypair);
  log_message("\nGenerate keypair (KEM):\t\t");
  log_message(time_kem_keypair);
  log_message("\nSign message:\t\t\t ");
  log_message(time_sign);
  log_message("\nDecapsulate session key:\t  ");
  log_message(time_kem_dec);

  log_message("\n\nSIZE (bytes)");
  log_message("\nPublic key (KEM):\t\t");
  log_message(CRYPTO_KEM_PUBLICKEYBYTES);
  log_message("\nPrivate key (KEM):\t\t");
  log_message(CRYPTO_KEM_SECRETKEYBYTES);
  log_message("\nSession key:\t\t\t  ");
  log_message(CRYPTO_KEM_BYTES);
  log_message("\nCiphertext:\t\t\t");
  log_message(CRYPTO_KEM_CIPHERTEXTBYTES);
  log_message("\nSignature key:\t\t\t");
  log_message(CRYPTO_SIGN_SECRETKEYBYTES);
  log_message("\nVerification key:\t\t");
  log_message(CRYPTO_SIGN_PUBLICKEYBYTES);
  log_message("\nSigned message:\t\t\t");
  log_message(CRYPTO_SIGN_BYTES);

  log_message("\n\n\n--------------------- AES encryption/decryption using the new session key ---------------------\n");
}

void loop(){
  chat_loop("Alice", "Bob");
}
