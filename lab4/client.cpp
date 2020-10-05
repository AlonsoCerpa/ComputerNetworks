//g++ client.cpp -o client.exe

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <sstream>

using namespace std;

mutex mtx_queue_mensajes;
mutex mtx_print;
mutex mtx_juego_esperando;

queue<char> mensajes;
bool juego_esperando = false;

void push_string_to_queue(string s)
{
  mtx_queue_mensajes.lock();
  for (int i = 0; i < s.size(); ++i)
  {
    mensajes.push(s[i]);
  }
  mtx_queue_mensajes.unlock();
}

void print_and_pop_queue()
{
  mtx_queue_mensajes.lock();
  mtx_print.lock();
  while (mensajes.empty() == false)
  {
    cout << mensajes.front();
    mensajes.pop();
  }
  mtx_print.unlock();
  mtx_queue_mensajes.unlock();
}

void imprimir_tablero(vector<vector<char>> &tablero, int num_jugadores)
{
  stringstream ss;
  ss << "  ";
  for (int i = 0; i < num_jugadores + 1; ++i)
  {
    ss << i << " ";
  }
  ss << "\n";
  for (int i = 0; i < num_jugadores + 1; ++i)
  {
    ss << i << " ";
    for (int j = 0; j < num_jugadores + 1; ++j)
    {
      ss << tablero[i][j] << " ";
    }
    ss << "\n";
  }
  ss << "\n";
  string str = ss.str();
  push_string_to_queue(str);
}

void func_thread_juego(int SocketFD, int num_jugadores)
{
  vector<vector<char>> tablero(num_jugadores + 1, vector<char>(num_jugadores + 1, ' '));
  bool termino_juego = false;
  char accion;
  int n;

  while (termino_juego == false)
  {
    n = read(SocketFD, &accion, 1);
    if (n < 0) perror("ERROR reading from socket");

    if (accion == 'A') //actualizar tablero
    {
      //Leer movimiento que otro cliente ha realizado y actualizar tablero
      char buffer_entrada_actualizacion[3];
      n = read(SocketFD, buffer_entrada_actualizacion, 3);
      if (n < 0) perror("ERROR reading from socket");

      //Modificar el estado del tablero
      int x_coord = (int) (buffer_entrada_actualizacion[1] - '0');
      int y_coord = (int) (buffer_entrada_actualizacion[2] - '0');
      tablero[y_coord][x_coord] = buffer_entrada_actualizacion[0];

      //Imprimir tablero actualizado
      imprimir_tablero(tablero, num_jugadores);
    }
    else if (accion == 'T') //es mi turno
    {
      //Leer el caracter que representa a mi pieza
      char pieza;
      n = read(SocketFD, &pieza, 1);
      if (n < 0) perror("ERROR reading from socket");

      stringstream ss;
      ss << "Su pieza es: " << pieza << "\n";

      //Leer mi jugada por standard input (cin)
      //print_and_pop_queue();
      string jugada;
      mtx_juego_esperando.lock();
      juego_esperando = true;
      mtx_juego_esperando.unlock();
      mtx_print.lock();
      cout << "Ingrese su jugada: ";
      getline(cin, jugada);
      mtx_print.unlock();
      mtx_juego_esperando.lock();
      juego_esperando = false;
      mtx_juego_esperando.unlock();

      //Enviar mi jugada al servidor
      char buffer_salida_jugada[3];
      buffer_salida_jugada[0] = pieza;
      buffer_salida_jugada[1] = jugada[0];
      buffer_salida_jugada[2] = jugada[1];
      n = write(SocketFD, buffer_salida_jugada, 3);
      if (n < 0) perror("ERROR writing to socket");

      //Leer respuesta del servidor
      char buffer_entrada_actualizacion[4];
      n = read(SocketFD, buffer_entrada_actualizacion, 4);
      if (n < 0) perror("ERROR reading from socket");

      //Actualizar estado del tablero con mi jugada
      int x_coord = (int) (buffer_entrada_actualizacion[2] - '0');
      int y_coord = (int) (buffer_entrada_actualizacion[3] - '0');
      tablero[y_coord][x_coord] = buffer_entrada_actualizacion[1];

      //Imprimir tablero actualizado
      imprimir_tablero(tablero, num_jugadores);
    }
    else if (accion == '=') //la partida quedo en empate
    {
      stringstream ss;
      ss << "Empato\n";
      string str = ss.str();
      push_string_to_queue(str);
      termino_juego = true;
    }
    else if (accion == 'W') //gané
    {
      stringstream ss;
      ss << "Gano\n";
      string str = ss.str();
      push_string_to_queue(str);
      termino_juego = true;
    }
    else if (accion == 'L') //perdí
    {
      stringstream ss;
      ss << "Perdio\n";
      string str = ss.str();
      push_string_to_queue(str);
      termino_juego = true;
    }
  }

  shutdown(SocketFD, SHUT_RDWR);
  close(SocketFD);
}

void func_thread_out_to_chat(int SocketFD_chat)
{
  int n;
  string input;
  char buffer_of_length[3];
  int length_buffer;
  while (true)
  {
    mtx_print.lock();
    cout << "Ingrese su mensaje al chat: ";
    getline(cin, input);
    mtx_print.unlock();

    if (input != "")
    {
      length_buffer = input.length();
      string length_buffer_str = to_string(length_buffer);
      bzero(buffer_of_length, 3);
      length_buffer_str.copy(buffer_of_length, length_buffer_str.length());

      char buffer[length_buffer];
      bzero(buffer, length_buffer);
      input.copy(buffer, length_buffer);

      n = write(SocketFD_chat, buffer_of_length, 3);
      if (n < 0) perror("ERROR writing to socket");

      n = write(SocketFD_chat, buffer, length_buffer);
      if (n < 0) perror("ERROR writing to socket");
    }
    
    print_and_pop_queue();

    mtx_juego_esperando.lock();
    bool aux = juego_esperando;
    mtx_juego_esperando.unlock();
    while (aux == true)
    {
      mtx_juego_esperando.lock();
      aux = juego_esperando;
      mtx_juego_esperando.unlock();
    }
  }
}

void func_thread_in_from_chat(int SocketFD_chat)
{
  int n;
  char buffer_of_length[3];

  while (true)
  {
    bzero(buffer_of_length, 3);
    n = read(SocketFD_chat, buffer_of_length, 3);
    if (n < 0) perror("ERROR reading from socket");

    int length_buffer = stoi(string(buffer_of_length, 3));
    char buffer[length_buffer];
    n = read(SocketFD_chat, buffer, length_buffer);
    if (n < 0) perror("ERROR reading from socket");

    string buffer_str(buffer, length_buffer);
    push_string_to_queue(buffer_str + "\n");
  }
}

int main(int argc, char *argv[])
{
  int num_jugadores = stoi(string(argv[1]));

  struct sockaddr_in stSockAddr_juego;
  struct sockaddr_in stSockAddr_chat;
  int Res;
  int SocketFD_juego = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int SocketFD_chat = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (-1 == SocketFD_juego)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  if (-1 == SocketFD_chat)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr_juego, 0, sizeof(struct sockaddr_in));
  memset(&stSockAddr_chat, 0, sizeof(struct sockaddr_in));

  stSockAddr_juego.sin_family = AF_INET;
  stSockAddr_juego.sin_port = htons(50003);
  Res = inet_pton(AF_INET, "51.15.220.108", &stSockAddr_juego.sin_addr);
//127.0.0.1
  stSockAddr_chat.sin_family = AF_INET;
  stSockAddr_chat.sin_port = htons(60003);
  Res = inet_pton(AF_INET, "51.15.220.108", &stSockAddr_chat.sin_addr);

  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD_juego);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter) does not contain valid ipaddress");
    close(SocketFD_juego);
    exit(EXIT_FAILURE);
  }

  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD_chat);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter) does not contain valid ipaddress");
    close(SocketFD_chat);
    exit(EXIT_FAILURE);
  }

  if (-1 == connect(SocketFD_juego, (const struct sockaddr *)&stSockAddr_juego, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD_juego);
    exit(EXIT_FAILURE);
  }

  if (-1 == connect(SocketFD_chat, (const struct sockaddr *)&stSockAddr_chat, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD_chat);
    exit(EXIT_FAILURE);
  }

  thread(func_thread_juego, SocketFD_juego, num_jugadores).detach();
  thread(func_thread_out_to_chat, SocketFD_chat).detach();
  thread(func_thread_in_from_chat, SocketFD_chat).detach();

  while(true)
  {

  }

  return 0;
}
