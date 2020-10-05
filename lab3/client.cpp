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

using namespace std;

void imprimir_tablero(vector<vector<char>> &tablero, int num_jugadores)
{
  cout << "  ";
  for (int i = 0; i < num_jugadores + 1; ++i)
  {
    cout << i << " ";
  }
  cout << "\n";
  for (int i = 0; i < num_jugadores + 1; ++i)
  {
    cout << i << " ";
    for (int j = 0; j < num_jugadores + 1; ++j)
    {
      cout << tablero[i][j] << " ";
    }
    cout << "\n";
  }
  cout << "\n";
}

int main(int argc, char *argv[])
{
  int num_jugadores = stoi(string(argv[1]));
  vector<vector<char>> tablero(num_jugadores + 1, vector<char>(num_jugadores + 1, ' '));

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
  stSockAddr.sin_port = htons(50003);
  Res = inet_pton(AF_INET, "51.15.220.108", &stSockAddr.sin_addr);

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

  bool termino_juego = false;
  char accion;
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

      cout << "Su pieza es: " << pieza << "\n";

      //Leer mi jugada por standard input (cin)
      string jugada;
      cout << "Ingrese su jugada: ";
      getline(cin, jugada);

      //Enviar mi jugada al servidor
      char buffer_salida_jugada[3];
      buffer_salida_jugada[0] = pieza;
      buffer_salida_jugada[1] = jugada[0];
      buffer_salida_jugada[2] = jugada[1];
      n = write(SocketFD, buffer_salida_jugada, 3);
      if (n < 0) perror("ERROR writing to socket");

      //Actualizar estado del tablero con mi jugada
      int x_coord = (int) (jugada[0] - '0');
      int y_coord = (int) (jugada[1] - '0');
      tablero[y_coord][x_coord] = pieza;

      //Imprimir tablero actualizado
      imprimir_tablero(tablero, num_jugadores);
    }
    else if (accion == '=') //la partida quedo en empate
    {
      cout << "Empato\n";
      termino_juego = true;
    }
    else if (accion == 'W') //gané
    {
      cout << "Gano\n";
      termino_juego = true;
    }
    else if (accion == 'L') //perdí
    {
      cout << "Perdio\n";
      termino_juego = true;
    }
  }

  shutdown(SocketFD, SHUT_RDWR);
  close(SocketFD);

  return 0;
}
