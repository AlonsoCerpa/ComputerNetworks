#ifndef GAME_H
#define GAME_H

#include <list>
#include <unordered_map>

class PosXY {
public:
    PosXY();
    PosXY(const PosXY &p);
    PosXY(int x, int y);
    
    int x, y;
};

class Snake {
public:
    Snake();
    Snake(const Snake &snake);
    Snake(std::list<PosXY> &body_position, char body_character);

    std::list<PosXY> body_erased;
    std::list<PosXY> body_position;
    char body_character;
};

class Game {
public:
    Game();
    Game(int width_table, int height_table);
    bool valid_movement(char direction, int id_client);
    bool move(char direction, int id_client, bool erased);
    bool check_collision(int x_new_cab, int y_new_cab, int id_client);

    std::unordered_map<int, Snake> ids_clients_snakes;
    int width_table;
    int height_table;
    bool end_game;
};

#endif