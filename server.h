//
// Created by marc on 13/08/22.
//

#ifndef BROADCAST_SERVER_H
#define BROADCAST_SERVER_H

#include <pthread.h>
#include <pwd.h>
#include "main.h"

int server_side(pthread_t *client_thread_t, struct passwd *pass, in_addr_t address);

#endif //BROADCAST_SERVER_H
