#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <stdlib.h>
#include "../include/game.cpp"

#include <vector>

int main(int argc, char *argv[]){

    system("clear");
    int rows, columns;
    std::ifstream local(argv[1]);
    local >> rows >> columns;
    game game;


    if(!game.set_map(argv[1])){
        return 0;
    }

    game.show_map();

    while(!game.finish_game()){
        game.move_snake();
        game.finish_map();
    }

    system("clear");
    if(game.getlife() == 0){
        game.show_game_over();
    } else {
        game.show_congratulations();
    }

    return 0;
}