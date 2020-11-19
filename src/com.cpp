#include <Arduino.h>
#include "com.h"
extern "C" {
  #include "randombytes.h"
}

GCM<AES256> gcm;

void com_begin() {
  Serial1.begin(115200);
}

bool com_interface_ready() {
  return Serial1.available();
}

void com_read_string(unsigned char *buffer, size_t buffer_length){
  Serial1.readStringUntil('%');
  Serial1.readBytes(buffer, buffer_length);
}

void com_send_burst(unsigned char *message, size_t length, size_t burst_size){
  size_t count = 0;
  Serial1.write('%');
  while(count < length){
    if(count + burst_size < length){
      Serial1.write(&message[count], burst_size);
    }
    else{
      Serial1.write(&message[count], length - count);
    }
    count += burst_size;
    delay(100);
  }
}

void prepare_aes(const uint8_t *key, size_t len) {
  gcm.setKey(key, len);
}

void chat_loop(String me, String theother) {
  if(Serial.available()){
    Serial.print("\n\nMessage to "+theother+" (before encryption):");
    unsigned char msg[AES_MAX_MSG_LENGTH];
    unsigned char tag[gcm.tagSize()];
    unsigned char msg_size_buffer[4];
    unsigned char iv[gcm.ivSize()];

    size_t msg_size = Serial.readBytesUntil('\n', msg, AES_MAX_MSG_LENGTH);

    randombytes(iv, gcm.ivSize());
    gcm.setIV(iv, gcm.ivSize());
    Serial1.write(iv,gcm.ivSize());

    unsigned char ciphertext[msg_size];
    gcm.encrypt(ciphertext, msg, msg_size);
    gcm.computeTag(tag, gcm.tagSize());

    memcpy(msg_size_buffer, &msg_size, 4);
    com_send_burst(msg_size_buffer, 4, 256);
    com_send_burst(ciphertext, msg_size, 256);
    com_send_burst(tag, gcm.tagSize(), 256);

    Serial.println();
    Serial.write(msg, msg_size);
    delay(1000);
  }
  if (Serial1.available()){

    Serial.print("\n\nMessage from "+theother+" (after decryption): ");
    unsigned char msg_size_buffer[4];
    unsigned char iv[gcm.ivSize()];
    unsigned char tag[gcm.tagSize()];
    size_t msg_size;

    Serial1.readBytes(iv, gcm.ivSize());
    gcm.setIV(iv, gcm.ivSize());

    com_read_string(msg_size_buffer, 4);
    memcpy(&msg_size, msg_size_buffer, 4);
    unsigned char ciphertext[msg_size];
    com_read_string(ciphertext, msg_size);
    com_read_string(tag, gcm.tagSize());

    delay(10);
    unsigned char msg[msg_size];
    gcm.decrypt(msg, ciphertext, msg_size);

    if(gcm.checkTag(tag, gcm.tagSize())){
      Serial.println();
      Serial.write(msg, msg_size);
    }
    else{
      Serial.write("\n\nAuthentication failed. Is Eve interfering?");
    }
    delay(1000);
  }
}
