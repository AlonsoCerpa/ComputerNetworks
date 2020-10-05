//g++ server.cpp -o server.exe

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
#include <map>
#include <vector>

using namespace std;

char ganador_n_en_raya(int num_jugadores, vector<vector<char>> &tablero, string &piezas)
{
  //Verificar si es empate
  bool es_empate = true;
  for (int i = 0; i < num_jugadores + 1; ++i)
  {
    for (int j = 0; j < num_jugadores + 1; ++j)
    {
      if (tablero[i][j] == ' ')
      {
        es_empate = false;
        break;
      }
    }
    if (es_empate == false)
    {
      break;
    }
  }
  if (es_empate == true) //si es empate
  {
    return '\0';
  }

  //Verificar si alguien ganó con una vertical
  bool gano = true;
  for (int x = 0; x < num_jugadores; ++x)
  {
    for (int i = 0; i < num_jugadores + 1; ++i)
    {
      gano = true;
      for (int j = 0; j < num_jugadores + 1; ++j)
      {
        if (tablero[j][i] != piezas[x])
        {
          gano = false;
          break;
        }
      }
      if (gano == true)
      {
        return piezas[x];
      }
    }
  }

  //Verificar si alguien ganó con una horizontal
  for (int x = 0; x < num_jugadores; ++x)
  {
    for (int i = 0; i < num_jugadores + 1; ++i)
    {
      gano = true;
      for (int j = 0; j < num_jugadores + 1; ++j)
      {
        if (tablero[i][j] != piezas[x])
        {
          gano = false;
          break;
        }
      }
      if (gano == true)
      {
        return piezas[x];
      }
    }
  }

  //Verificar si alguien ganó con una diagonal
  for (int x = 0; x < num_jugadores; ++x)
  {
    //primera diagonal
    gano = true;
    for (int i = 0; i < num_jugadores + 1; ++i)
    {
      if (tablero[i][i] != piezas[x])
      {
        gano = false;
        break;
      }
    }
    if (gano == true)
    {
      return piezas[x];
    }

    //segunda diagonal
    gano = true;
    for (int i = 0; i < num_jugadores + 1; ++i)
    {
      if (tablero[i][num_jugadores - i] != piezas[x])
      {
        gano = false;
        break;
      }
    }
    if (gano == true)
    {
      return piezas[x];
    }
  }

  return ' '; //No ganó nadie todavía
}

int main(int argc, char *argv[])
{
  map<char, int> piezas_y_clientSDs;
  string piezas("XOBDEFGHIJKMNPQRSUVYZ0123456789!$?()<>{},.;:-_#|&/+*[]");
  int num_jugadores = stoi(string(argv[1]));
  vector<vector<char>> tablero(num_jugadores + 1, vector<char>(num_jugadores + 1, ' '));
  
  struct sockaddr_in stSockAddr;
  int serverSD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;
  
  if(-1 == serverSD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(50003);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if(-1 == bind(serverSD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(serverSD);
    exit(EXIT_FAILURE);
  }

  if(-1 == listen(serverSD, 10))
  {
    perror("error listen failed");
    close(serverSD);
    exit(EXIT_FAILURE);
  }

  for(int i = 0; i < num_jugadores; ++i)
  {
    int clientFD = accept(serverSD, NULL, NULL);

    if(0 > clientFD)
    {
      perror("error accept failed");
      close(serverSD);
      exit(EXIT_FAILURE);
    }
    piezas_y_clientSDs.insert(pair<char, int>(piezas[i], clientFD));
  }

  bool termino_juego = false;
  map<char, int>::iterator iter = piezas_y_clientSDs.begin();
  while (termino_juego == false)
  {
    if (iter == piezas_y_clientSDs.end())
    {
      iter = piezas_y_clientSDs.begin();
    }

    //Escribir a un cliente avisándole que es su turno
    char buffer_salida1[2];
    buffer_salida1[0] = 'T';
    buffer_salida1[1] = iter->first;
    n = write(iter->second, buffer_salida1, 2);
    if (n < 0) perror("ERROR writing to socket");

    //Leer movimiento que cliente ha realizado
    char buffer_entrada1[3];
    n = read(iter->second, buffer_entrada1, 3);
    if (n < 0) perror("ERROR reading from socket");
    cout << "Mensaje recibido: " << string(buffer_entrada1, 3) << "\n";

    //Modificar el estado del tablero
    int x_coord = (int) (buffer_entrada1[1] - '0');
    int y_coord = (int) (buffer_entrada1[2] - '0');
    tablero[y_coord][x_coord] = buffer_entrada1[0];

    //Escribir a los demás la actualización del tablero con la jugada
    //del cliente actual
    char buffer_salida2[4];
    buffer_salida2[0] = 'A';
    buffer_salida2[1] = buffer_entrada1[0];
    buffer_salida2[2] = buffer_entrada1[1];
    buffer_salida2[3] = buffer_entrada1[2];
    for (auto it = piezas_y_clientSDs.begin(); it != piezas_y_clientSDs.end(); ++it)
    {
      if (it != iter)
      {
        n = write(it->second, buffer_salida2, 4);
        if (n < 0) perror("ERROR writing to socket");
      }
    }

    //Verificar si gano alguien
    char ganador = ganador_n_en_raya(num_jugadores, tablero, piezas);
    if (ganador == '\0') //empate
    {
      char buffer_salida_empate = '=';
      for (auto it = piezas_y_clientSDs.begin(); it != piezas_y_clientSDs.end(); ++it)
      {
        n = write(it->second, &buffer_salida_empate, 1);
        if (n < 0) perror("ERROR writing to socket");
      }
      termino_juego = true;
    }
    else if (ganador == ' ') //nadie ganó
    {

    }
    else  //ganó alguien
    {
      char buffer_salida_perdio = 'L';
      for (auto it = piezas_y_clientSDs.begin(); it != piezas_y_clientSDs.end(); ++it)
      {
        if (it->first != ganador)
        {
          n = write(it->second, &buffer_salida_perdio, 1);
          if (n < 0) perror("ERROR writing to socket");
        }
      }

      char buffer_salida_gano = 'W';
      n = write(piezas_y_clientSDs[ganador], &buffer_salida_gano, 1);
      if (n < 0) perror("ERROR writing to socket");

      termino_juego = true;
    }
    ++iter;
  }

  for (auto it = piezas_y_clientSDs.begin(); it != piezas_y_clientSDs.end(); ++it)
  {
    shutdown(it->second, SHUT_RDWR);
    close(it->second);
  }

  shutdown(serverSD, SHUT_RDWR);
  close(serverSD);
  
  return 0;
}
