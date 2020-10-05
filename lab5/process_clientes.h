#ifndef CHAT_H
#define CHAT_H

#include <string>

void process_cliente(int ConnectFD, std::string nickname, int id_client);
void process_clientes(int num_jugadores);

#endif