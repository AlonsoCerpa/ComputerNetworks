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
#define PORT    8080  //puerto destino
#define MAXLINE 1024  //max longitud del mensaje en bytes

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct hostent *host;
    struct sockaddr_in servaddr;

    host = (struct hostent *)gethostbyname((char *)"127.0.0.1");  //direccion destino

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    //Llenando informacion del destino
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr = *((struct in_addr *)host->h_addr);

    int n;
    socklen_t len;
    len = sizeof(servaddr);

    while (1)
    {
        printf("\nType Something (q or Q to quit):");
        string mssg ="";
        getline(cin,mssg);
        strcpy(buffer,mssg.c_str());

        if ((strcmp(buffer, "q") == 0) || strcmp(buffer, "Q") == 0)
            break;

        sendto(sockfd, buffer , strlen(buffer),
                MSG_CONFIRM, (const struct sockaddr *) &servaddr,
                        sizeof(servaddr));
        printf("Hello message sent.\n");

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                                MSG_WAITALL, (struct sockaddr *) &servaddr,
                                &len);
        buffer[n] = '\0';
        printf("Server : %s\n", buffer);
    }

    close(sockfd);
    return 0;
}