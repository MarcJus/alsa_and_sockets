//
// Created by marc on 13/08/22.
//

#include "client.h"
#include "main.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

void *client_thread(void *args) {
    struct args_client_thread *arguments = (struct args_client_thread*)args;
    char *username = arguments->username;
    ssize_t ret = 0;
    int sock_send = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(sock_send, perror("Erreur socket");exit(EXIT_FAILURE);)
    printf("Socket client initialisé\n");

    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = arguments->address;
    sockaddr.sin_port = htons(PORT);

    ret = connect(sock_send, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));
    CHECK(ret, perror("Connexion impossible"); close(sock_send);exit(EXIT_FAILURE);)

    while(1){
        size_t SAMPLE_LENGTH = sizeof(s16) * SAMPLE_RATE;
        printf("%lu\n", SAMPLE_LENGTH);
        short buffer[SAMPLE_LENGTH];
        bzero(buffer, SAMPLE_LENGTH);
        ret = read(STDIN_FILENO, buffer, 48000 * sizeof(short));
        CHECK(ret, perror("Impossible de lire stdin");break;)

        errno = 0;
        ret = send(sock_send, buffer, SAMPLE_LENGTH, 0);
        CHECK(ret, perror("Impossible d'envoyer les données"); break;)

    }

    close(sock_send);
    return NULL;
}
