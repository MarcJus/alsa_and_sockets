//
// Created by marc on 13/08/22.
//

#include "server.h"
#include "client.h"
#include "main.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <linux/cdrom.h>
#include <alsa/asoundlib.h>

void init_pcm(snd_pcm_t *pcm, snd_pcm_hw_params_t *params);

int socket_fd = 0;
char username[255];

void sig_term_handler(int signal){
    close(socket_fd);
    printf("Fin du programme...\n");
    exit(EXIT_SUCCESS);
}

float clamp(float value, float min, float max){
    if(value > max) return max;
    if(value < min) return min;
    return value;
}

int server_side(pthread_t *client_thread_t, struct passwd *pass, in_addr_t address) {
    int ret = 0;
    int sock_recv = socket(AF_INET, SOCK_STREAM, 0); // TCP
    CHECK(sock_recv, perror("Erreur socket");exit(EXIT_FAILURE);)

    socket_fd = sock_recv;

    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // écoute toutes les adresses
    sockaddr.sin_port = htons(PORT);

    strcpy(username, pass->pw_name);

    ret = bind(sock_recv, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    CHECK(ret, perror("Erreur bind()"); close(sock_recv); exit(EXIT_FAILURE);)

    struct args_client_thread arguments;
    strcpy(arguments.username, pass->pw_name);
    arguments.address = address;

    signal(SIGTERM, sig_term_handler);

    int ret_client = pthread_create(client_thread_t, NULL, client_thread,
                                    &arguments); // lancement du client
    if(ret_client != 0){
        fprintf(stderr, "Erreur lors de la création du thread client\n");
        exit(EXIT_FAILURE);
    }

    // alsa
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    uint val, val2;

    ret = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    CHECK(ret, fprintf(stderr, "Erreur open alsa : %s\n", snd_strerror(ret));)

    // Définition des paramètres alsa
    init_pcm(pcm, params);

    listen(sock_recv, 2);

    size_t SAMPLE_LENGTH = sizeof(s16) * SAMPLE_RATE;
    char sample_to_write[SAMPLE_LENGTH];
    struct sockaddr_in client;
    while(1){
        bzero(sample_to_write, SAMPLE_LENGTH);
        socklen_t socklen = sizeof(struct sockaddr_in);
        ret = accept(sock_recv, (struct sockaddr*)&client, &socklen);
        CHECK(ret, perror("Erreur accept"); continue;)

        ret = recv(sock_recv, sample_to_write, SAMPLE_LENGTH, 0);
        CHECK(ret, perror("Erreur recvfrom"); continue;)

        int chunk_sample_count = SAMPLE_RATE;

        CHECK(ret, perror("Erreur read"); break;)
        for (int i = 0; i < chunk_sample_count; i++) { // on prend chaque échantillion
            float volume = 1.0f;
            s16 *sample = sample_to_write + i;
            float normalized_sample = *sample / (float ) 32768;
            normalized_sample *= volume;
            *sample = (s16) clamp(normalized_sample * 32768, -32768, 32767);

//            sample_index++;
        }
        snd_pcm_writei(pcm, /*(u8*)samples + offset*/ sample_to_write, chunk_sample_count); // on écrit les données pour 1 seconde d'audio
        free(sample_to_write);

    }

    close(sock_recv);
    return 0;
}

void init_pcm(snd_pcm_t *pcm, snd_pcm_hw_params_t *params) {
    snd_pcm_hw_params_alloca(&params);

    snd_pcm_hw_params_any(pcm, params);

    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_MMAP_INTERLEAVED);

    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);

    snd_pcm_hw_params_set_channels(pcm, params, 1);

    snd_pcm_hw_params_set_rate(pcm, params, SAMPLE_RATE, 0);

    snd_pcm_hw_params_set_periods(pcm, params, 10, 0);

    snd_pcm_hw_params_set_period_time(pcm, params, 100000, 0);

    snd_pcm_hw_params(pcm, params);
}
