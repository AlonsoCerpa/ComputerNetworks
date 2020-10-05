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
#include <map>

using namespace std;

mutex mtx_print;
mutex mtx_map;
map<string, int> clients_and_socketFDs;

vector<string> get_nickname_and_msg(string str)
{
  vector<string> nickname_and_msg;
  string nickname;
  int i = 0;
  for ( ; i < str.length() && str[i] != ' '; ++i)
  {
    nickname.push_back(str[i]);
  }
  nickname_and_msg.push_back(nickname);
  if (i < str.length())
  {
    nickname_and_msg.push_back(str.substr(i+1));
  }
  return nickname_and_msg;
}
 
void process_client_thread(int socket_client)
{
  string msgFromChat;
  int n;
  char buffer_of_length[3];
  char bufferOutput_length[3];
  string nickname("");

  do {
    bzero(buffer_of_length, 3);
    n = read(socket_client, buffer_of_length, 3);
    if (n < 0) perror("ERROR reading from socket");

    int length_buffer = stoi(string(buffer_of_length, 3));
    char buffer[length_buffer];
    n = read(socket_client, buffer, length_buffer);
    if (n < 0) perror("ERROR reading from socket");

    mtx_print.lock();
    string buffer_str(buffer, length_buffer);
    cout << "Mensaje recibido del cliente: [" << buffer_str << "]\n";
    mtx_print.unlock();

    msgFromChat = string(buffer, length_buffer);

    if (msgFromChat != "chau" && msgFromChat != "") //Mensaje recibido no es chau
    {
      vector<string> words = get_nickname_and_msg(msgFromChat);
      
      if (words.size() == 1) //Mensaje contiene solo una palabra
      {
        if (words[0] == "lista") //Mensaje es igual a "lista"
        {
          string list_nicknames("Mensaje recibido del SERVIDOR: [Lista de nicknames: ");
          for (auto it = clients_and_socketFDs.begin(); it != clients_and_socketFDs.end(); ++it)
            list_nicknames += it->first + ", ";
          list_nicknames += "]";

          string length_bufferOutput_str = to_string(list_nicknames.length());
          bzero(bufferOutput_length, 3);
          length_bufferOutput_str.copy(bufferOutput_length, 3);

          char bufferOutput[list_nicknames.length()];
          bzero(bufferOutput, list_nicknames.length());
          list_nicknames.copy(bufferOutput, list_nicknames.length());

          n = write(socket_client, bufferOutput_length, 3);
          if (n < 0) perror("ERROR writing to socket");

          n = write(socket_client, bufferOutput, list_nicknames.length());
          if (n < 0) perror("ERROR writing to socket");
        }
        else  //Mensaje solo es un nickname (para registrarlo)
        {
          nickname = words[0];
          mtx_map.lock();
          clients_and_socketFDs.insert(pair<string, int>(words[0], socket_client));
          mtx_map.unlock();
          string msgOutput("Mensaje recibido del SERVIDOR: [Se registro su nickname: " + words[0] + "]");
          
          string length_bufferOutput_str = to_string(msgOutput.length());
          bzero(bufferOutput_length, 3);
          length_bufferOutput_str.copy(bufferOutput_length, 3);

          char bufferOutput[msgOutput.length()];
          bzero(bufferOutput, msgOutput.length());
          msgOutput.copy(bufferOutput, msgOutput.length());

          n = write(socket_client, bufferOutput_length, 3);
          if (n < 0) perror("ERROR writing to socket");

          n = write(socket_client, bufferOutput, msgOutput.length());
          if (n < 0) perror("ERROR writing to socket");
        }
      }
      else  //Mensaje consta de nickname destino mas el mensaje a enviar al nickname
      {
        map<string,int>::iterator it = clients_and_socketFDs.find(words[0]);
        if (it == clients_and_socketFDs.end()) //No se encontro el nickname destino
        {
          string msgOutput("Mensaje recibido del SERVIDOR: [No se encontro el nickname: " + words[0] + "]");
         
          string length_bufferOutput_str = to_string(msgOutput.length());
          bzero(bufferOutput_length, 3);
          length_bufferOutput_str.copy(bufferOutput_length, 3);

          char bufferOutput[msgOutput.length()];
          bzero(bufferOutput, msgOutput.length());
          msgOutput.copy(bufferOutput, msgOutput.length());

          n = write(socket_client, bufferOutput_length, 3);
          if (n < 0) perror("ERROR writing to socket");

          n = write(socket_client, bufferOutput, msgOutput.length());
          if (n < 0) perror("ERROR writing to socket");
        }
        else //Se encontro el nickname destino
        {
          int SocketFD_Receiver = it->second;
          string msgToReceiver = "Mensaje recibido de USUARIO " + nickname + ": [" + words[1] + "]";
          
          string length_bufferOutput_str = to_string(msgToReceiver.length());
          bzero(bufferOutput_length, 3);
          length_bufferOutput_str.copy(bufferOutput_length, 3);

          char bufferOutput1[msgToReceiver.length()];
          bzero(bufferOutput1, msgToReceiver.length());
          msgToReceiver.copy(bufferOutput1, msgToReceiver.length());

          n = write(SocketFD_Receiver, bufferOutput_length, 3);
          if (n < 0) perror("ERROR writing to socket");

          n = write(SocketFD_Receiver, bufferOutput1, msgToReceiver.length());
          if (n < 0) perror("ERROR writing to socket");



          string msgOutput("Mensaje recibido del SERVIDOR: [Mensaje enviado correctamente a: " + words[0] + "]");
          
          length_bufferOutput_str = to_string(msgOutput.length());
          bzero(bufferOutput_length, 3);
          length_bufferOutput_str.copy(bufferOutput_length, 3);

          char bufferOutput2[msgOutput.length()];
          bzero(bufferOutput2, msgOutput.length());
          msgOutput.copy(bufferOutput2, msgOutput.length());

          n = write(socket_client, bufferOutput_length, 3);
          if (n < 0) perror("ERROR writing to socket");

          n = write(socket_client, bufferOutput2, msgOutput.length());
          if (n < 0) perror("ERROR writing to socket");
        }
      }
    }
  } while (msgFromChat != "chau");

  if (nickname != "")
  {
    mtx_map.lock();
    clients_and_socketFDs.erase(nickname);
    mtx_map.unlock();
    mtx_print.lock();
    cout << "El cliente " << nickname << " salio del chat\n";
    mtx_print.unlock();
  }

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
  stSockAddr.sin_port = htons(2343);
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
