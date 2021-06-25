#include <iostream>
#include <vector>
#include <fstream>
#include <tuple> // tupla
#include <stack> // pilha
//#include <queue> // fila
#include <list>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

class game{
    private:
        struct cell{
            char type; // qual tipo de bloco é (wall"1", way"0")
            char situacion = 'n'; // situação da cell (food"f",snake head"h", old head"o")
                                  // (snake body"b", nothing "n", snake pass".") 
        };

        struct snake{
            std::tuple<int, int> head;
            std::tuple<int, int> head_s; // local onde começa
            std::tuple<int, int> head_l;
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
                std::cout << "\n\n\n way:" << std::get<0>((other.front())) << " " << std::get<1>((other.front())) << "\n\n\n";
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
            std::vector<std::vector<cell>> map; // mapa de jogo    
            snake jogador; // jogador
            int rodada = 0; // rodada do jogo

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
        std::list<mapa> mapas; // mapa do jogo
        std::tuple<int, int> position_food; // posição da comida
        way jogada_rodada; // caminho ate a comida
        int score = 0; // Score do jogador
        bool add_tail = true; // Caso no inicio do movimento precise aumentar o snake
        int points = 0; // Qnt que já comeu
        int life = 5; // Qnt de vidas


    public:

        game() {}

        // verifica se o jogo acabou
        bool finish_game(){
            if(mapas.empty() or life == 0){
                return true;
            }
            return false;
        }

        // verifica se o jogo acabou no mapa atual
        void finish_map(){
            if(points == 10){
                mapas.pop_front();
                points = 0;
                if(!mapas.empty())
                    create_food();
            }
        }

        // Guarda o mapa
        void set_mapa(std::string local_){ 
            int count = 0;
            int q1, q2;
            std::ifstream local(local_);
            while(local >> q1 >> q2){
                mapa a(q1, q2);
                mapas.push_back(a);
                for(int i(0); i < mapas.back().rows; ++i){
                    for(int j(0); j < mapas.back().columns; ++j){
                        if((i > 0) and (i < mapas.back().rows-1) and (j > 0) and (j < mapas.back().columns-1)){
                            local >> mapas.back().map[i][j].type;
                            if(mapas.back().map[i][j].type == 'm'){
                                mapas.back().map[i][j].type = '0';
                                mapas.back().map[i][j].situacion = 'h';
                                std::get<0>(mapas.back().jogador.head) = i;
                                std::get<1>(mapas.back().jogador.head) = j;
                                std::get<0>(mapas.back().jogador.head_s) = i;
                                std::get<1>(mapas.back().jogador.head_s) = j;
                            }
                        }else
                            mapas.back().map[i][j].type = '1';
                    }
                }
            }
            local.close();
            create_food();
            std::cout << "\n\n";
        }
        void show_lifes(){
            for(int i(0); i < 5; i++){
                if(i < life){
                    std::cout << "\u2764";
                } else {
                    std::cout << "_";
                }
            }
            std::cout << "\n";
        }
        void show_points(){
            std::cout << "Foods: ";
            for(int i(0); i < 10; i++){
                if(i < points){
                    std::cout << "\u2615";
                } else {
                    std::cout << "_";
                }
            }
            std::cout << "\n\n";
        }

        void show_map(){
            std::cout << "Rodada: " <<mapas.front().rodada << "\t";
            show_lifes();
            show_points();
           mapas.front().rodada++;
            for(int i(1); i < mapas.front().rows-1; ++i){
                std::cout << "\u2593";
                for(int j(1); j < mapas.front().columns-1; ++j){
                    if(mapas.front().map[i][j].situacion == 'h'){ // head
                        std::cout << "\u23fa";
                        std::get<0>(mapas.front().jogador.head) = i;
                        std::get<1>(mapas.front().jogador.head) = j;
                    }else if(mapas.front().map[i][j].type == '1') // wall
                        std::cout << "\u2591"; // u23f9
                    else if(mapas.front().map[i][j].situacion == 'f') // food
                        std::cout << "\u2615";
                    else if(mapas.front().map[i][j].situacion == 'b') // body
                        std::cout << "\u0e4f";
                    /*else if(mapas.front().map[i][j].situacion == '.')// PARA TESTES
                        std::cout << ".";               //*/
                    else
                        std::cout << " ";
                }
                std::cout << "\u2593" << std::endl;
            }
            std::cout << "\n\n\n\n\n\n\n\n\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        // Verifica quantas celulas foram acessadas
        int verified_cells(std::vector<way> caminhos){
            int count = 0;
            for(int i(0); i < caminhos.size(); ++i){
                for(int j(0); j < caminhos[i].ways.size(); ++j){
                    count++;
                }
            }
            return count;
        }

        // Gera a melhor jogada até a comida
        void jogada_i(){
            int i = 0;
            int x = std::get<0>(mapas.front().jogador.head);
            int y = std::get<1>(mapas.front().jogador.head);
            std::vector<way> caminhos;
            //std::stack<std::tuple<int,int>> branch; // camificações ao longo do caminho
            way a(x,y);
            caminhos.push_back(a);

            // Caso especial só com a cabeça, para n voltar pelo caminho onde veio depois de comer
            if(mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion != 'b' and mapas.front().jogador.body.size() < 2)
                mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = '.';
            
            int size_inicial = 0; // só verificar os caminhos que estao des do inicio do while atual
            int size_cells = 0;
            while((!find_food(caminhos)) and (verified_cells(caminhos) != size_cells)){
                //std::cout << verified_cells(caminhos) << "|" << size_cells << '\n';
                size_cells = verified_cells(caminhos);
                size_inicial = caminhos.size();
                while((i < size_inicial) and !find_food(caminhos)){
                    //print_way(caminhos[i]); //
                    x = std::get<0>(caminhos[i].ways.back());
                    y = std::get<1>(caminhos[i].ways.back());
                    // Caso a ultima posição do caminho seja a comida, nao mexe mais no caminho
                    if((x == std::get<0>(position_food)) and (y == std::get<1>(position_food))){
                        break;
                    }
                    // Caso só tenha um caminho a seguir
                    if(ramificacao(x, y) == 1){
                        caminhos[i].ways.push_back(choose_way(x,y));
                        caminhos[i].size++;                        
                    } else if(ramificacao(x, y) == 2){ // Caso tenha 2 caminhos a seguir
                        caminhos.push_back(caminhos[i]);
                        caminhos[i].ways.push_back(choose_way(x,y));
                        caminhos[i].size++;
                        caminhos.back().ways.push_back(choose_way(x,y));
                        caminhos.back().size++;
                    } else if(ramificacao(x, y) == 3){ // Caso tenha 3 caminhos a seguir
                        caminhos.push_back(caminhos[i]);
                        caminhos.push_back(caminhos[i]);
                        caminhos[i].ways.push_back(choose_way(x,y));
                        caminhos[i].size++;
                        caminhos[caminhos.size()-2].ways.push_back(choose_way(x,y));
                        caminhos[caminhos.size()-2].size++;
                        caminhos.back().ways.push_back(choose_way(x,y));
                        caminhos.back().size++;
                    } else if(ramificacao(x,y) == 4){ // Caso tenha 4 caminhos a seguir(só tenha a cabeça e seja o  inicio)
                        caminhos.push_back(caminhos[i]);
                        caminhos.push_back(caminhos[i]);
                        caminhos.push_back(caminhos[i]);

                        caminhos[i].ways.push_back(choose_way(x,y));
                        caminhos[i].size++;
                        caminhos[caminhos.size()-3].ways.push_back(choose_way(x,y));
                        caminhos[caminhos.size()-3].size++;
                        caminhos[caminhos.size()-2].ways.push_back(choose_way(x,y));
                        caminhos[caminhos.size()-2].size++;
                        caminhos.back().ways.push_back(choose_way(x,y));
                        caminhos.back().size++;

                    }
                    //std::cout << i << ": ";
                    //print_way(caminhos[i]);

                    mapas.front().map[std::get<0>(position_food)][std::get<1>(position_food)].situacion = 'f';
                    i++;
                }
                if(mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion == '.')
                    mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = 'n';
                i = 0;
            }

            int smaller_size = (mapas.front().rows * mapas.front().columns) +1;
            for(int i(0); i < caminhos.size(); ++i){ // percorrendo toda o vetor de caminhos
                int x_food = std::get<0>(position_food);
                int y_food = std::get<1>(position_food);
                if((std::get<0>(caminhos[i].ways.back()) == x_food)
                and (std::get<1>(caminhos[i].ways.back()) == y_food)
                and (caminhos[i].size < smaller_size)){ // caso o caminho atual termine com a posição
                                                        // da comida e tenha um tamanho menor que o 
                                                        // menor ja achado
                    for(int j(0); j < caminhos[i].size; ++j){ // salva na jogada_rodada o menor caminho achado
                        jogada_rodada.ways.push_back(caminhos[i].ways[j]);
                        jogada_rodada.size++;
                    }
                    smaller_size = jogada_rodada.size;
                }
            }

            clear_map();
            // Caso muito especial, se a comida estiver no canto para morrer depois de pegar e n tiver corpo ainda
            //std::cout << std::get<0>(mapas.front().jogador.head_l) << "|" << std::get<1>(mapas.front().jogador.head_l) << '\n';
                
            if(smaller_size == (mapas.front().rows * mapas.front().columns) +1){ // if dont have a way to food
                if(mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion != 'b'
                and mapas.front().jogador.body.size() < 2)
                    mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = '.';
                //std::cout << "tentando sobreviver:\n"; // teste
                //show_map();
                try_survive();
                add_tail = false;
            } else {
                //print_way(jogada_rodada);
                move_snake();
                add_tail = true;
            }
            
            
        }

        // Move a cobra, caso tenha dado certo chegar a comida return True, caso contrario
        // 
        void move_snake(){
            int x = std::get<0>(mapas.front().jogador.head);
            int y = std::get<1>(mapas.front().jogador.head);
            int x_, y_;
            if(jogada_rodada.size != 0 and mapas.front().jogador.body.size() == 0){ // Caso tenha achado o caminho até a comida
                for(int i(1); i < jogada_rodada.size; ++i){
                    if(i < jogada_rodada.size){
                        mapas.front().map[x][y].situacion = 'n';
                    } 


                    x = std::get<0>(jogada_rodada.ways[i]);
                    y = std::get<1>(jogada_rodada.ways[i]);
                    
                    if (i == jogada_rodada.size - 2){
                        std::get<0>(mapas.front().jogador.head_l) = x;
                        std::get<1>(mapas.front().jogador.head_l) = y;
                        std::cout << x << y;
                    }

                    mapas.front().map[x][y].situacion = 'h';
                    if(i == jogada_rodada.size-1){
                        create_food();
                        points++;
                    }
                    show_map();
                }
                std::get<0>(mapas.front().jogador.head) = std::get<0>(jogada_rodada.ways.back());
                std::get<1>(mapas.front().jogador.head) = std::get<1>(jogada_rodada.ways.back());
                std::tuple<int, int> temp(std::get<0>(mapas.front().jogador.head), std::get<1>(mapas.front().jogador.head));
                mapas.front().jogador.body.push_back(temp);
                jogada_rodada.clear();
                
            } else if(jogada_rodada.size != 0 and mapas.front().jogador.body.size() != 0){
                // Move the head one position to add new body
                for(int i(1); i < jogada_rodada.size; ++i){
                    if(i == 1 and add_tail == true)
                        mapas.front().map[x][y].situacion = 'b';
                    else {
                        move_tail();
                    }
                    x = std::get<0>(jogada_rodada.ways[i]);
                    y = std::get<1>(jogada_rodada.ways[i]);
                    
                    mapas.front().map[x][y].situacion = 'h';
                    if(i == jogada_rodada.size-1){
                        create_food();
                        points++;
                    }
                    show_map();
                }
                std::get<0>(mapas.front().jogador.head) = std::get<0>(jogada_rodada.ways.back());
                std::get<1>(mapas.front().jogador.head) = std::get<1>(jogada_rodada.ways.back());
                std::tuple<int, int> temp(std::get<0>(mapas.front().jogador.head), std::get<1>(mapas.front().jogador.head));
                mapas.front().jogador.body.push_front(temp);
                jogada_rodada.clear();
            }   
        }

        /*
        // Crescer snake
        void add_body(){
            int x = std::get<0>(jogada_rodada.ways[0]);
            int y = std::get<1>(jogada_rodada.ways[0]);
            mapa[x][y].situacion = 'b';
            std::tuple<int, int> temp(x,y);
            jogador.body.push_front(temp);

            x = std::get<0>(jogada_rodada.ways[1]);
            y = std::get<1>(jogada_rodada.ways[1]);
            mapa[x][y].situacion = 'h';
            std::get<0>(jogador.head) = x;
            std::get<1>(jogador.head) = y;
        }
        */

        void dead(){
            while(mapas.front().jogador.body.size() != 0){
                mapas.front().jogador.body.pop_back();
            }
            //std::cout << mapas.front().jogador.body.size();
            for(int i(0); i < mapas.front().rows; ++i){
                for(int j(0); j < mapas.front().columns; ++j){
                    if(mapas.front().map[i][j].situacion == 'b'){
                        mapas.front().map[i][j].situacion = 'n';
                    }
                }
            }
            mapas.front().map[std::get<0>(mapas.front().jogador.head)][std::get<1>(mapas.front().jogador.head)].situacion = 'n';
            mapas.front().map[std::get<0>(mapas.front().jogador.head_s)][std::get<1>(mapas.front().jogador.head_s)].situacion = 'h';
            std::get<0>(mapas.front().jogador.head) = std::get<0>(mapas.front().jogador.head_s);
            std::get<1>(mapas.front().jogador.head) = std::get<1>(mapas.front().jogador.head_s);
            create_food();
            life--;
        }

        // A cobra n tem nenhum caminho a seguir, faz movimento aleatorio para mover todo corpo
        void try_survive(){
            bool is_dead = false;
            int x = std::get<0>(mapas.front().jogador.head);
            int y = std::get<1>(mapas.front().jogador.head);
            
            if(mapas.front().map[x-1][y].type == '0' and mapas.front().map[x-1][y].situacion == 'n'){ // para cima
                x = x-1;
                mapas.front().map[x][y].situacion = 'h';
            } else if(mapas.front().map[x][y+1].type == '0' and mapas.front().map[x][y+1].situacion == 'n'){ // para direita
                y = y+1;
                mapas.front().map[x][y].situacion = 'h';
            } else if(mapas.front().map[x+1][y].type == '0' and mapas.front().map[x+1][y].situacion == 'n'){ // para baixo
                x = x+1;
                mapas.front().map[x][y].situacion = 'h';
            } else if(mapas.front().map[x][y-1].type == '0' and mapas.front().map[x][y-1].situacion == 'n'){ // para esquerda
                y = y-1;
                mapas.front().map[x][y].situacion = 'h';
            } else {
                //std::cout << "morreu";
                dead();
                is_dead = true;
            }
            if(mapas.front().jogador.body.size() != 0 and !is_dead){
                move_tail();
            }
            if(!is_dead){
                std::get<0>(mapas.front().jogador.head) = x;
                std::get<1>(mapas.front().jogador.head) = y;
            }
            show_map();
        }

        void clear_map(){
            for(int i(0); i < mapas.front().rows; ++i){
                for(int j(0); j < mapas.front().columns; ++j){
                    if(mapas.front().map[i][j].situacion == '.')
                        mapas.front().map[i][j].situacion = 'n';
                }
            }
        }
        // Move a ultima celula do corpo da cobra para antes da cabeça
        void move_tail(){
            mapas.front().map[std::get<0>(mapas.front().jogador.body.back())][std::get<1>(mapas.front().jogador.body.back())].situacion = 'n';
            mapas.front().jogador.body.pop_back();
            std::tuple<int,int> a(std::get<0>(mapas.front().jogador.head), std::get<1>(mapas.front().jogador.head));
            mapas.front().jogador.body.push_front(a);
            mapas.front().map[std::get<0>(mapas.front().jogador.body.front())][std::get<1>(mapas.front().jogador.body.front())].situacion = 'b';
        }

        //  Escolhe uma celula para a cobra seguir, retorna a primeira celula livre que achar
        //    que esteja colado em mapa[x][y]
        //
        std::tuple<int,int> choose_way(int x, int y){
            mapas.front().map[x][y].situacion = '.';
            if(mapas.front().map[x+1][y].type == '0' and mapas.front().map[x+1][y].situacion != '.' and mapas.front().map[x+1][y].situacion != 'b'){
                mapas.front().map[x+1][y].situacion = '.';
                std::tuple<int,int> a(x+1, y);
                return a;
            } if(mapas.front().map[x][y-1].type == '0' and mapas.front().map[x][y-1].situacion != '.' and mapas.front().map[x][y-1].situacion != 'b') {
                mapas.front().map[x][y-1].situacion = '.';
                std::tuple<int,int> a(x, y-1);
                return a;
            } if(mapas.front().map[x][y+1].type == '0' and mapas.front().map[x][y+1].situacion != '.' and mapas.front().map[x][y+1].situacion != 'b') {
                mapas.front().map[x][y+1].situacion = '.';
                std::tuple<int,int> a(x, y+1);
                return a;
            } if(mapas.front().map[x-1][y].type == '0' and mapas.front().map[x-1][y].situacion != '.' and mapas.front().map[x-1][y].situacion != 'b') {
                mapas.front().map[x-1][y].situacion = '.';
                std::tuple<int,int> a(x-1, y);
                return a;
            }
        }

        // Verifica se algum caminho já achou a comida
        bool find_food(std::vector<way> w){
            int x_food = std::get<0>(position_food);
            int y_food = std::get<1>(position_food);
            for(int i(0); i < w.size(); ++i){
                if((std::get<0>(w[i].ways.back()) == x_food) and (std::get<1>(w[i].ways.back()) == y_food))
                    return true;
            }
            return false;
        }

        // Verificar quantos caminhos possiveis pode ir a partir do local mapa[x][y]
        int ramificacao(int x, int y){ 
            int count = 0;
            if(mapas.front().map[x+1][y].type == '0' and mapas.front().map[x+1][y].situacion != '.' and mapas.front().map[x+1][y].situacion != 'b')
                count++;
            if(mapas.front().map[x][y-1].type == '0' and mapas.front().map[x][y-1].situacion != '.' and mapas.front().map[x][y-1].situacion != 'b')
                count++;
            if(mapas.front().map[x][y+1].type == '0' and mapas.front().map[x][y+1].situacion != '.' and mapas.front().map[x][y+1].situacion != 'b')
                count++;
            if(mapas.front().map[x-1][y].type == '0' and mapas.front().map[x-1][y].situacion != '.' and mapas.front().map[x-1][y].situacion != 'b')
                count++;

            //std::cout << "ramificacao: "<< count << std::endl;
            return count;
        }

        // Cria uma comida aleatoriamente no mapa
        void create_food(){
            srand((unsigned)time(0));
            int random_row = std::get<0>(position_food);
            int random_columns = std::get<1>(position_food);
            while(true){
                random_row = rand()%(mapas.front().rows);
                random_columns = rand()%(mapas.front().columns);
                if(((random_row != std::get<0>(position_food)) or (random_columns != std::get<1>(position_food)))
                and (mapas.front().map[random_row][random_columns].type == '0') 
                and (mapas.front().map[random_row][random_columns].situacion == 'n')){
                    break;
                }
            }
            if(mapas.front().map[std::get<0>(position_food)][std::get<1>(position_food)].situacion == 'f')
                mapas.front().map[std::get<0>(position_food)][std::get<1>(position_food)].situacion = 'n';

            std::get<0>(position_food) = random_row;
            std::get<1>(position_food) = random_columns;
            mapas.front().map[random_row][random_columns].situacion = 'f';
        }

        // Função para escrever um caminho (Usado nos testes)
        void print_way(way a){
            int i = 0;
            while(i < a.size){
                std::cout << "(" << std::get<0>(a.ways[i]) << "," << std::get<1>(a.ways[i]) << ")";
                i++;
            }
            std::cout << std::endl << std::endl;
        }
};