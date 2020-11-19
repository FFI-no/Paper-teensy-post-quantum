#include <Arduino.h>

#include "log.h"
#include "com.h"
#include "params.h"
#include "status.h"

unsigned char public_key[CRYPTO_KEM_PUBLICKEYBYTES];
unsigned char ciphertext[CRYPTO_KEM_CIPHERTEXTBYTES];
unsigned char session_key[CRYPTO_KEM_BYTES];

unsigned char verification_key[CRYPTO_SIGN_PUBLICKEYBYTES];
unsigned char signed_message[MESSAGE_LENGTH];
unsigned char smlenBuffer[SMLEN_BYTES];

unsigned long current_time;
unsigned long time_sign_open;
unsigned long time_kem_enc;

size_t smlen[1];
size_t pkb;

void setup(){
  log_begin();
  com_begin();

  init_lights();

  // Set LED and wait for message from initiator
  set_lights(LEDGREEN);
  while(com_interface_ready() == 0);
  log_message("------------- BOB -------------\n");
  log_message("Waiting for signed public key from Alice...\n");
  set_lights(LEDYELLOW);

  // Read information
  com_read_string(smlenBuffer, SMLEN_BYTES);
  memcpy(smlen, smlenBuffer, SMLEN_BYTES);
  com_read_string(verification_key, CRYPTO_SIGN_PUBLICKEYBYTES);
  com_read_string(signed_message, *smlen);
  log_message("Received signed public key from Alice.\n");

  log_message("Signed public key:\n");
  log_print_bytes(signed_message, 256, 32);
  log_message("...\n");

  // Verify signature
  current_time = micros();
  int ver = crypto_sign_open(public_key, &pkb, signed_message, *smlen, verification_key);
  time_sign_open = micros() - current_time;

  if(ver != 0){
    set_lights(LEDRED);
    log_signature_not_verified();
    while(true);
  }
  else{
    log_signature_verified();
  }

  // Generate session key and ciphertext
  current_time = micros();
  crypto_kem_enc(ciphertext, session_key, public_key);
  time_kem_enc = micros() - current_time;

  // Send ciphertext
  com_send_burst(ciphertext, CRYPTO_KEM_CIPHERTEXTBYTES, 32);
  log_message("\nSent encapsulated session key to Alice.\n");

  // Print
  log_message("\nCiphertext:\n");
  log_print_bytes(ciphertext, 256, 32);
  log_message("...\n");

  log_message("Session key:\n");
  log_print_bytes(session_key, CRYPTO_KEM_BYTES, 32);

  set_lights(LEDGREEN);

  // Set session key
  prepare_aes(session_key, CRYPTO_KEM_BYTES);

  log_message("\nKEM scheme: ");
  log_message(CRYPTO_KEM_ALGNAME);
  log_message("\nSignature scheme: ");
  log_message(CRYPTO_SIGN_ALGNAME);

  log_message("\n\nTime consumption (microseconds):\n");
  log_message("Signature verification:\t\t\t    ");
  log_message(time_sign_open);
  log_message("\nSession key generation and encapsulation:  ");
  log_message(time_kem_enc);

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
  chat_loop("Bob", "Alice");
}
