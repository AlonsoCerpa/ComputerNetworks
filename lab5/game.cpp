#include "game.h"

PosXY::PosXY() {
    this->x = 0;
    this->y = 0;
}

PosXY::PosXY(const PosXY &p) {
    this->x = p.x;
    this->y = p.y;
}

PosXY::PosXY(int x, int y) {
    this->x = x;
    this->y = y;
}

Snake::Snake() {
    this->body_character = '-';
}

Snake::Snake(const Snake &snake) {
    this->body_position = snake.body_position;
    this->body_character = snake.body_character;
    this->body_erased = snake.body_erased;
}

Snake::Snake(std::list<PosXY> &body_position, char body_character) {
    this->body_position = body_position;
    this->body_character = body_character;
}

Game::Game() {
    this->width_table = 0;
    this->height_table = 0;
    this->end_game = false;
}

Game::Game(int width_table, int height_table) {
    this->width_table = width_table;
    this->height_table = height_table;
    this->end_game = false;
}

bool Game::valid_movement(char direction, int id_client) {
    std::list<PosXY> &snake_body = this->ids_clients_snakes[id_client].body_position;
    auto it = snake_body.end();
    --it;
    PosXY head = *it;
    --it;
    PosXY before_head = *it;
    if (direction == 'U') {
        int new_head_y = head.y - 1;
        if (new_head_y == -1)
            new_head_y = this->height_table - 1;
        if (head.x == before_head.x && new_head_y == before_head.y)
            return false;
        else
            return true;
    }
    else if (direction == 'D') {
        int new_head_y = head.y + 1;
        if (new_head_y == this->height_table)
            new_head_y = 0;
        if (head.x == before_head.x && new_head_y == before_head.y)
            return false;
        else
            return true;
    }
    else if (direction == 'L') {
        int new_head_x = head.x - 1;
        if (new_head_x == -1)
            new_head_x = this->width_table - 1;
        if (head.y == before_head.y && new_head_x == before_head.x)
            return false;
        else
            return true;
    }
    else if (direction == 'R') {
        int new_head_x = head.x + 1;
        if (new_head_x == this->width_table)
            new_head_x = 0;
        if (head.y == before_head.y && new_head_x == before_head.x)
            return false;
        else
            return true;
    }
    return false;
}

bool Game::check_collision(int x_new_cab, int y_new_cab, int id_client) {
    //Comprobar colision con otras serpientes
    for (auto it = this->ids_clients_snakes.begin(); it != this->ids_clients_snakes.end(); ++it) {
        if (it->first != id_client) {
            for (auto it2 = it->second.body_position.begin(); it2 != it->second.body_position.end(); ++it2) {
                if (it2->x == x_new_cab && it2->y == y_new_cab) {
                    return true;
                }
            }
        }
    }
    //Comprobar colision con el cuerpo de mi propia serpiente
    auto it = this->ids_clients_snakes[id_client].body_position.begin();
    ++it;
    for ( ; it != this->ids_clients_snakes[id_client].body_position.end(); ++it) {
        if (it->x == x_new_cab && it->y == y_new_cab) {
            return true;
        }
    }
    return false;
}

bool Game::move(char direction, int id_client, bool erased) {
    Snake &snake = this->ids_clients_snakes[id_client];
    std::list<PosXY> &snake_body = snake.body_position;
    PosXY cabeza = snake_body.back();
    if (direction == 'U') {
        --cabeza.y;
        if (cabeza.y == -1) {
            cabeza.y = height_table - 1;
        }
    }
    else if (direction == 'D') {
        ++cabeza.y;
        if (cabeza.y == height_table) {
            cabeza.y = 0;
        }
    }
    else if (direction == 'L') {
        --cabeza.x;
        if (cabeza.x == -1) {
            cabeza.x = width_table - 1;
        }
    }
    else if (direction == 'R') {
        ++cabeza.x;
        if (cabeza.x == width_table) {
            cabeza.x = 0;
        }
    }
    bool collision = check_collision(cabeza.x, cabeza.y, id_client);
    if (collision == false) {
        if (erased == true) {
            PosXY tail_erased = snake_body.front();
            snake.body_erased.push_back(tail_erased);
        }
        snake_body.pop_front();
        snake_body.push_back(cabeza);
        return true;
    }
    else {
        return false;
    }
}