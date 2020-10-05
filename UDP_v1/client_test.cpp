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
#include <thread>

using namespace std;
#define MY_PORT 50003  //el puerto de mi host
#define OTHER_PORT 60003  //el puerto del otro host
#define MAXLINE 1024  //max longitud del mensaje en bytes

int sockfd;
char buffer[MAXLINE];
char buffer1[MAXLINE];
struct hostent *host;
struct sockaddr_in my_addr, other_addr, other_addr1;
int n;
socklen_t len1;

void process_sends()
{
    while (true)
    {
        printf("\nType Something (q or Q to quit):");
        string mssg ="";
        getline(cin,mssg);
        strcpy(buffer,mssg.c_str());

        if ((strcmp(buffer, "q") == 0) || strcmp(buffer, "Q") == 0)
            break;

        sendto(sockfd, buffer , strlen(buffer),
                MSG_CONFIRM, (const struct sockaddr *) &other_addr,
                        sizeof(other_addr));
    }
}

void process_recvs()
{
    while (true)
    {
        n = recvfrom(sockfd, (char *)buffer1, MAXLINE,
                                MSG_WAITALL, (struct sockaddr *) &other_addr1,
                                &len1);
        buffer1[n] = '\0';
        printf("Server : %s\n", buffer1);
    }
}

int main() {
    host = (struct hostent *)gethostbyname((char *)"51.15.220.108");  //direccion del otro host

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

    other_addr1.sin_family = AF_INET; // IPv4
    other_addr1.sin_port = htons(OTHER_PORT);
    other_addr1.sin_addr = *((struct in_addr *)host->h_addr);

    //Llenando informacion de mi host
    my_addr.sin_family = AF_INET; // IPv4
    my_addr.sin_port = htons(MY_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    //Enlazar mi socket con mi direccion ip y puerto
    if ( bind(sockfd, (const struct sockaddr *)&my_addr,
                    sizeof(my_addr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    len1 = sizeof(other_addr1);

    std::thread(process_sends).detach();
    std::thread(process_recvs).detach();

    while (true)
    {

    }

    close(sockfd);
    return 0;
}