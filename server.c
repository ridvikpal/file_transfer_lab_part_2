#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 1024

int main(int argc, char* argv[]){
    // input should be udp listen port, so only 1 argument
    if (argc != 1){
        // ensure only 2 arguments are passed
        if (argc != 2){
            printf("Error: Please enter only 2 arguments");
            exit(1);
        }

        // setup our server address
        struct sockaddr_in serverSockAddrIn;
        // fill the rest of the struct with 0
        memset(&serverSockAddrIn, 0, sizeof(serverSockAddrIn));
        serverSockAddrIn.sin_family = AF_INET;
        serverSockAddrIn.sin_addr.s_addr = INADDR_ANY;
        serverSockAddrIn.sin_port = htons(atoi(argv[1])); // setting the port number

        // create a new socket using socket(), returning socket fd
        int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        // associate address with the socket via bind()
        bind(udpSocket, (struct sockaddr*) &serverSockAddrIn, sizeof(serverSockAddrIn));

        printf("Server started at port %s...\n", argv[1]);

        // create buffer to store incoming message
        char message[MESSAGE_SIZE];
        // declare our client address
        struct sockaddr_in clientSockAddrIn;
        socklen_t clientAddressSize = sizeof(clientSockAddrIn);
        memset(&clientSockAddrIn, 0, sizeof(clientSockAddrIn));

        while (1){
            // receive incoming UDP connections
            ssize_t message_length = recvfrom(udpSocket, (char*)message, MESSAGE_SIZE, 0,
                                              (struct sockaddr*)&clientSockAddrIn, &clientAddressSize);

            if (message_length < 0){
                printf("Failed to receive message.\n");
                continue;
            }

            // null terminate the string so we can print via printf() easily
            message[message_length] = '\0';

            printf("Received message: %s\n", message);

            // setup the response message
            if (strcmp(message, "ftp") == 0){
                strcpy(message, "yes");
            }else{
                strcpy(message, "no");
            }

            printf("Sending message: %s\n", message);

            // send the response to the client
            if (sendto(udpSocket, message, strlen(message), 0, (struct sockaddr*)&clientSockAddrIn,
                    clientAddressSize)< 0) {
                printf("Failed to send message.\n");
                continue;
            }

        }

        // close the file descriptor after it has been used
        close(udpSocket);

        return 0;
    }
}