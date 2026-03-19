#ifndef MESSAGE_H
#define MESSAGE_H

#include "user.h"

void manage_messages(int userIdx);
void send_message(int userIdx);
void read_messages(int userIdx);
void delete_messages(int userIdx);
void get_time_str(char *buf);

#endif
