#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx_print;

void process_input_from_server(int SocketFD)
{
  int n;
  char buffer_of_length[3];

  while (true)
  {
    bzero(buffer_of_length, 3);
    n = read(SocketFD, buffer_of_length, 3);
    if (n < 0) perror("ERROR reading from socket");

    int length_buffer = stoi(string(buffer_of_length, 3));
    char buffer[length_buffer];
    n = read(SocketFD, buffer, length_buffer);
    if (n < 0) perror("ERROR reading from socket");

    mtx_print.lock();
    string buffer_str(buffer, length_buffer);
    cout << buffer_str << "\n";
    mtx_print.unlock();
  }
}

void process_output_to_server(int SocketFD)
{
  int n;
  string input;
  char buffer_of_length[3];
  int length_buffer;
  while (true)
  {
    mtx_print.lock();
    cout << "Ingrese su mensaje: ";
    getline(cin, input);
    mtx_print.unlock();

    length_buffer = input.length();
    string length_buffer_str = to_string(length_buffer);
    bzero(buffer_of_length, 3);
    length_buffer_str.copy(buffer_of_length, length_buffer_str.length());

    char buffer[length_buffer];
    bzero(buffer, length_buffer);
    input.copy(buffer, length_buffer);

    n = write(SocketFD, buffer_of_length, 3);
    if (n < 0) perror("ERROR writing to socket");

    n = write(SocketFD, buffer, length_buffer);
    if (n < 0) perror("ERROR writing to socket");
  }
}

int main(void)
{
  struct sockaddr_in stSockAddr;
  int Res;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;

  if (-1 == SocketFD)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(2343);
  Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter) does not contain valid ipaddress");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  thread(process_input_from_server, SocketFD).detach();
  thread(process_output_to_server, SocketFD).detach();

  while(true)
  {

  }

  shutdown(SocketFD, SHUT_RDWR);
  close(SocketFD);

  return 0;
}
