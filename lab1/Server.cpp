/* Server code in C */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <mutex>

using namespace std;

mutex mtx_print;

vector<string> split_string_sep_by_spaces(string str)
{
  vector<string> words;
  bool state = false;
  string word;
  for (int i = 0; i < str.length(); ++i)
  {
    if (str[i] != ' ')
    {
      if (state == false)
      {
        word = string();
        word.push_back(str[i]);
        state = true;
      }
      else
      {
        word.push_back(str[i]);
      }
    }
    else if (str[i] == '\0' || str[i] == '\n')
    {
      words.push_back(word);
      return words;
    }
    else
    {
      words.push_back(word);
      state = false;
    }
  }
  words.push_back(word);

  return words;
}
 
void process_client_thread(int socket_client)
{
  string msgFromChat;
  int n;
  char buffer[256];

  do {
    bzero(buffer,256);
    n = read(socket_client,buffer,256);
    if (n < 0) perror("ERROR reading from socket");
    mtx_print.lock();
    printf("Mensaje recibido del cliente: [%s]\n", buffer);
    mtx_print.unlock();
    msgFromChat = string(buffer);

    if (msgFromChat != "chau")
    {
      vector<string> words = split_string_sep_by_spaces(msgFromChat);
      string operation;

      int num1 = stoi(words[0]);
      int num2 = stoi(words[1]);
      int result;

      if (words[2] == "+")
      {
        operation = "suma";
        result = num1+num2;
      }
      else if (words[2] == "-")
      {
        operation = "resta";
        result = num1-num2;
      }
      else if (words[2] == "*")
      {
        operation = "multiplicacion";
        result = num1*num2;
      }
      else
      {
        operation = "division";
        result = (int) (num1/num2);
      }

      char bufferOutput[256];
      bzero(bufferOutput, 256);
      string msgOutput = "La " + operation + " es " + to_string(result);
      msgOutput.copy(bufferOutput, msgOutput.length());
      n = write(socket_client, bufferOutput, 256);
      if (n < 0) perror("ERROR writing to socket");
    }
  } while (msgFromChat != "chau");

  shutdown(socket_client, SHUT_RDWR);
  close(socket_client);
}

int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  char buffer[256];
  int n;

  if(-1 == SocketFD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(2222);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if(-1 == listen(SocketFD, 10))
  {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    if(0 > ConnectFD)
    {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    thread(process_client_thread, ConnectFD).detach();
  }

  close(SocketFD);
  return 0;
}
