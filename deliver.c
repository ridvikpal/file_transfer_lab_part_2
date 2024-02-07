#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc,char *argv[])
{
    //Check to see if there are the correct number of arguments
    if (argc != 3){
        printf ("Error\n");
        return -1;
    }
    char *str = NULL;
    str = (char *) malloc(1024);
    printf ("Please input a message as follows: ftp <file name>\n");
    fgets(str, 1024, stdin);

    if (strncmp (str, "ftp ", 4) != 0){
        printf ("Command must start with \"ftp\" \n");
        return -1;
    }
    char *file_name = str + 4;
    size_t len = strlen(file_name);

    // Remove trailing spaces and newline characters
    while (len > 0 && (file_name[len - 1] == ' ' || file_name[len - 1] == '\t' || file_name[len - 1] == '\n')) {
        len--;
    }

    // Set the null terminator at the last non-whitespace character
    file_name[len] = '\0';
    printf ("%s\n", file_name);

    if (access(file_name, F_OK) != 0) {
        // file doesn't exists
        printf("File with name: \"%s\" does not exist\n", file_name);
        return -1;
    }

    //Fill in server information
    int port = atoi(argv[2]);
    if (port == 0 && strcmp(argv[2], "0") !=0){
        //Failed string to int conversion
        printf("Invalid port number: %s\n", argv[2]);
        return -1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof (server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(argv[1]);


    //Make the socket
    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0))<0){
        perror ("Error making the socket\n");
        return -1;
    }
    clock_t start_time = clock();

    //Send "ftp"
    if (sendto(sockfd, "ftp", strlen("ftp"), MSG_CONFIRM, (struct sockaddr *) &server_address, sizeof (server_address))<0){
        perror ("Sending ftp failed\n");
        close(sockfd);
        return -1;
    }
    printf ("ftp sent to the server\n");

    //Receive message from the server
    ssize_t msg_len;
    socklen_t client_address_size = sizeof(server_address);
    char buffer [1024];
    msg_len = recvfrom(sockfd, (char*) buffer, 1024, 0, (struct sockaddr*) &server_address, &client_address_size);


    if (msg_len == -1){
        perror("recvfrom failed");
        close(sockfd);
        return -1;
    }

    clock_t end_time = clock();
    double time_elapsed = ((double)(end_time - start_time))/ CLOCKS_PER_SEC;
    printf("%fs have elaspsed\n", time_elapsed);

    buffer[msg_len] = '\0';

    if (strcmp(buffer, "yes") == 0){
        printf ("A file transfer can start.\n");
    }else {
        printf ("Exiting");
        return -1;
    }
    free(str);
    close (sockfd);
    return 0;
}