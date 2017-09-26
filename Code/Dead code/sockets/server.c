#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_LENGTH 512
#define PORT 8888

struct sockaddr_in socket_in, socket_out;
int socketfd;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void* socketListener(void* args)
{
    char buffer[BUFFER_LENGTH];
    int inputLength, socketlen = sizeof(socket_in);

	while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        inputLength = recvfrom(socketfd, buffer, BUFFER_LENGTH, 0, (struct sockaddr *) &socket_out, (socklen_t *) &socketlen);

        if (inputLength == -1) {
            error("Error on receive");
        }

        printf("Received packet from %s:%d\n", inet_ntoa(socket_out.sin_addr), ntohs(socket_out.sin_port));
        printf("Data: %s\n" , buffer);

        int result = sendto(socketfd, buffer, inputLength, 0, (struct sockaddr*) &socket_out, socketlen);

        if (result == -1) {
            error("Error on send");
        }
	}
	return NULL;
}

void socketInit()
{
    socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketfd == -1)
    {
        error("Error on socket creation");
    }

    memset((char *) &socket_in, 0, sizeof(socket_in));

    socket_in.sin_family = AF_INET;
    socket_in.sin_port = htons(PORT);
    socket_in.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socketfd, (struct sockaddr*) &socket_in, sizeof(socket_in)) == -1)
    {
        error("Error on bind");
    }
}

int main(int argc, char *argv[])
{
    pthread_t pth;

    socketInit();

    pthread_create(&pth, NULL, socketListener, 0);
    //usleep(10000000);
	while (1) {
		printf("I'm alive!\n");
		fflush(stdout);
		usleep(5000000);
	}
    return 0;
}
