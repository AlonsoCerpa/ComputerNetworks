#ifndef PROCESS_CONNECTION_H
#define PROCESS_CONNECTION_H

#include <vector>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <map>
#include <string>

#include "game.h"

extern std::vector<char> msgs_from_chat;
extern std::queue<std::vector<char> > queue_game;
extern std::string nickname;
extern char character_snake;
extern int id_client;
extern std::vector<std::vector<char> > inputs_to_server;
extern std::map<int, std::pair<std::string, char> > id_client_nickname_pieza;
extern std::unordered_map<std::string, int> nickname_id_client;
extern Game game;

extern std::mutex mtx_msgs_from_chat;
extern std::mutex mtx_queue_game;
extern std::mutex mtx_nickname;
extern std::mutex mtx_character_snake;
extern std::mutex mtx_id_client;
extern std::mutex mtx_inputs_to_server;
extern std::mutex mtx_id_client_nickname_pieza;
extern std::mutex mtx_nickname_id_client;
extern std::mutex mtx_game;

void process_from_server(int socketFD);
void process_to_server(int socketFD);
void process_connection(int num_jugadores);

#endif