#include <iostream>
#include <vector>
#include <fstream>
#include <tuple> // tupla
#include <list>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <stdlib.h>

class game{
    private:
        struct cell{
            char type; // qual tipo de bloco é (wall"#", way"0")
            char situacion = 'n'; // situação da cell (food"f",snake head"h")
                                  // (snake body"b", nothing "n", snake pass'!', wall invisible ".") 
        };

        struct snake{
            std::tuple<int, int> head;
            std::tuple<int, int> head_s; // starting point
            std::tuple<int, int> head_l; // variable to prevent the return of the head
            std::list<std::tuple<int, int>> body;
        };

        struct way{
            int size = 0;
            std::vector<std::tuple<int,int>> ways;

            way() {}

            way(int x, int y){
                std::tuple <int, int> a(x,y);
                ways.push_back(a);
                size++;
            }

            way(std::list<std::tuple<int,int>> &other_, int x, int y){
                std::list<std::tuple<int,int>> other = other_;
                while((std::get<0>((other.front())) != x) and (std::get<1>(other.front()) != y)){
                    other.pop_front();
                    size--;
                }
            }

            void clear(){
                ways.clear();
                size = 0;
            }
        };
        struct mapa{
            int rows;
            int columns;
            std::vector<std::vector<cell>> map; // map of the game
            snake jogador; // player

            mapa(int rows_ = 0, int columns_ = 0)
                :rows(rows_+2)
                ,columns(columns_+2)
            {
                map.resize(rows);

                for(int i(0); i < rows; ++i)
                    map[i].resize(columns);
            }
        };
    private:
        std::list<mapa> mapas; // game map
        std::tuple<int, int> position_food; // position of food
        way jogada_rodada; // round turn
        int score = 0; // game points
        bool add_tail = true; // if need to increase the snake
        int points = 0; // quantity eat
        int life = 5; // number of lifes
        bool posible_bug = false; //  variable to fix a posible bug
        int to_dead = 0; // variable responsible for not letting in case the food bake in an impossible corner to pick up

    public:

        game() {}

        int getlife();
        bool finish_game();
        void finish_map();
        bool set_map(std::string local_);
        void show_game_over();
        void show_congratulations();
        void show_lifes();
        void show_points();
        void show_map();
        int verified_cells(std::vector<way> caminhos);
        void jogada_i();
        void move_snake();
        void dead();
        void try_survive();
        void clear_map();
        void move_tail();
        std::tuple<int,int> choose_way(int x, int y);
        bool exist_food();
        bool find_food(std::vector<way> w);
        int ramificacao(int x, int y);
        void create_food();

        // FUNCTIONS TO TESTS
        void print_body(std::list<std::tuple<int, int>> body);
        void print_way(way a);
};