//
// Created by marc on 12/08/22.
//

#ifndef BROADCAST_MAIN_H
#define BROADCAST_MAIN_H

#include <stdlib.h>
#include <netinet/in.h>

#define BUF_LENGTH 1024
#define PORT 3030

#define CHECK(value, action) if(value < 0){action}

typedef signed short s16;
#define SAMPLE_RATE 48000

extern size_t MESSAGE_LENGTH;

enum message_type {
    TYPE_TEXT_MESSAGE, TYPE_AUDIO_BUFFER
};

struct chat_message {

    char author[255];

    enum message_type message_type;

    char message[BUF_LENGTH];

};

struct args_client_thread {

    char username[255];

    in_addr_t address;

};

#endif //BROADCAST_MAIN_H
