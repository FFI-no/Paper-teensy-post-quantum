#ifndef LOG_H
#define LOG_H

void log_begin();
bool log_interface_ready();
void log_message(String msg);
void log_signature_not_verified();
void log_signature_verified();
void log_wrong_session_key();
void log_right_session_key();
void log_print_bytes(const unsigned char *bytes, size_t len, const uint8_t wrap);
void log_print_key(unsigned char *key, size_t key_length);

#endif
