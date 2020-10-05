#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
#define MY_PORT 60003  //el puerto de mi host
#define OTHER_PORT 50003  //el puerto del otro host
#define MAXLINE 1024

// Driver code
int main() {

    int sockfd;
    char buffer[MAXLINE];
    struct hostent *host;
    struct sockaddr_in my_addr, other_addr;

    host = (struct hostent *)gethostbyname((char *)"190.237.34.131");  //direccion del otro host

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    memset(&other_addr, 0, sizeof(other_addr));

    //Llenando informacion del otro host
    other_addr.sin_family = AF_INET; // IPv4
    other_addr.sin_port = htons(OTHER_PORT);
    other_addr.sin_addr = *((struct in_addr *)host->h_addr);

    // Filling server information
    my_addr.sin_family = AF_INET; // IPv4
    my_addr.sin_port = htons(MY_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&my_addr,
                    sizeof(my_addr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int n;
    socklen_t len;
    len = sizeof(other_addr); //len is value/resuslt

    while(1){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                                MSG_WAITALL, ( struct sockaddr *) &other_addr,
                                &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);

        printf("\nType Something (q or Q to quit):");
        string mssg ="";
        getline(cin,mssg);
        strcpy(buffer,mssg.c_str());

        sendto(sockfd, buffer, strlen(buffer),
                MSG_CONFIRM, (const struct sockaddr *) &other_addr,
                        len);
        printf("Hello message sent.\n");
    }

    return 0;
}