#include <iostream>
#include <thread>
#include <string>

#include "process_clientes.h"

int main(int argc, char *argv[]) {
    int num_jugadores = std::stoi(std::string(argv[1]));

    std::thread(process_clientes, num_jugadores).detach();

    while (true) {

    }
    
    return 0;
}