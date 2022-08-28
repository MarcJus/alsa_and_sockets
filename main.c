#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <pwd.h>

#include "main.h"
#include "server.h"
#include "client.h"

void listenMessages(int sock_fd);

void sendMessages(int sock_fd);

size_t MESSAGE_LENGTH = sizeof(struct chat_message);

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr, "Adresse IP non spécifiée.\n");
        exit(EXIT_FAILURE);
    }

    uid_t uid = getuid();
    struct passwd *pass;
    pass = getpwuid(uid);

    pthread_t server_thread_t;

    server_side(&server_thread_t, pass, inet_addr(argv[1]));

    return 0;
}
