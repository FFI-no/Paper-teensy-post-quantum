#ifndef COM_H
#define COM_H

#include "../arduinolibs/libraries/Crypto/AES.h"
#include "../arduinolibs/libraries/Crypto/GCM.h"
#define AES_MAX_MSG_LENGTH 1000

void com_begin();
bool com_interface_ready();
void com_send_string(unsigned char *str, size_t str_len);
void com_read_string(unsigned char *buffer, size_t buffer_length);
void com_send_burst(unsigned char *message, size_t length, size_t burst_size);
void prepare_aes(const uint8_t *key, size_t len);
void chat_loop(String me, String theother);

#endif
