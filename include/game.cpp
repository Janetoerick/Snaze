#include "game.h"

/*!
 * @brief return the number of lifes
*/ 
int game::getlife(){
    return life;
}

/*!
 * @brief check if the game is over
*/ 
bool game::finish_game(){
    if(mapas.empty() or life == 0){
        return true;
    }
    return false;
}

/*!
 * @brief check if the map is over
*/ 
void game::finish_map(){
    if(points == 10){
        show_map();
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        mapas.pop_front();
        points = 0;
        if(!mapas.empty())
            create_food();
    }
}

/*!
 * @brief read the maps
 * @param local_        the file of maps
*/ 
bool game::set_map(std::string local_){ 

    int q1, q2; // take the rows and columns
    std::ifstream local(local_);
    int num_map = 1; // counter of maps
    bool have_head = false; // check if exist the point of start in the map
    bool other_caractere = false;
    bool thats_ok = true; // check if all maps it's all right

    std::string s;
    char temp;
    while(local >> q1 >> q2){
        mapa a(q1, q2);
        mapas.push_back(a);
        for(int i(0); i < mapas.back().rows; ++i){
            for(int j(0); j < mapas.back().columns; ++j){
                
                if((i > 0) and (i < mapas.back().rows-1) and (j > 0) and (j < mapas.back().columns-1)){
                    local >> mapas.back().map[i][j].type;
                    if(mapas.back().map[i][j].type == '*'){ // if find the start point in this map
                        mapas.back().map[i][j].type = '0';
                        mapas.back().map[i][j].situacion = 'h';
                        std::get<0>(mapas.back().jogador.head) = i;
                        std::get<1>(mapas.back().jogador.head) = j;
                        std::get<0>(mapas.back().jogador.head_s) = i;
                        std::get<1>(mapas.back().jogador.head_s) = j;
                        have_head = true;
                    } else if(mapas.back().map[i][j].type == '0'){
                        mapas.back().map[i][j].type = '0';
                    } else if(mapas.back().map[i][j].type == '#'){
                        
                    } else if(mapas.back().map[i][j].type == '.'){
                        
                    } else {
                        other_caractere = true;
                    }
                }else
                    mapas.back().map[i][j].type = '#';
            }
        }
        std::cout << std::endl;
        if(!have_head){ // check if this map have a problem
            std::cout << "MAPA DE NUMERO: " << num_map << " NAO TEM PONTO INICIAL!\n";
            thats_ok = false;
        }
        if(other_caractere){
            std::cout << "MAPA DE NUMERO: " << num_map << " TEM UM CARACTERE NAO IDENTIFICADO!\n";
            thats_ok = false;
        }
        other_caractere = false;
        have_head = false;
        num_map++;
    }
    local.close(); // close the file
    if(thats_ok){ // if all is right, return true
        create_food();
        std::cout << "\n\n";
        return true;
    } else { // if have one error, return false
        std::cout << "Program aborted!\n";
        return false;
    }
}

/*!
 * @brief Show an end game message
*/
void game::show_game_over(){
    std::cout   << "_______________________________________________\n"
                << "                   GAME OVER\n\n"
                << "           More luck in the next...\n"
                << "_______________________________________________\n\n";
}

/*!
 * @brief Show a win game message
*/
void game::show_congratulations(){
    std::cout   << "_______________________________________________\n"
                << "               CONGRATULATIONS!!\n\n"
                << "                score:  " << score << "\n"
                << " final life quanttity:  " << life << "\n\n"
                << "              THANKS FOR PLAYING\n\n"
                << "_______________________________________________\n\n";
}

/*!
 * @brief Show lifes in the game
*/
void game::show_lifes(){
    std::cout << "lifes: ";
    for(int i(0); i < 5; i++){
        if(i < life){
            std::cout << "\u2764";
        } else {
            std::cout << "_";
        }
    }
    std::cout << "   |\t";
}

/*!
 * @brief Show points in the map
*/
void game::show_points(){
    std::cout << "Coffes: ";
    for(int i(0); i < 10; i++){
        if(i < points){
            std::cout << "\u2615";
        } else {
            std::cout << "_";
        }
    }
    std::cout << "\n_______________________________________________________________\n\n";
}

/*!
 * @brief Show the map
*/
void game::show_map(){
    system("clear");
    show_lifes();
    std::cout << "Score: " << score << "\t|   ";
    show_points();
    score += 10;
    for(int i(1); i < mapas.front().rows-1; ++i){
        std::cout << "\u2593";
        for(int j(1); j < mapas.front().columns-1; ++j){
            if(mapas.front().map[i][j].situacion == 'h'){ // head
                std::cout << "\u23fa";
                std::get<0>(mapas.front().jogador.head) = i;
                std::get<1>(mapas.front().jogador.head) = j;
            }else if(mapas.front().map[i][j].type == '#') // wall
                std::cout << "\u2593"; // u23f9
            else if(mapas.front().map[i][j].situacion == 'f') // food
                std::cout << "\u2615";
            else if(mapas.front().map[i][j].situacion == 'b') // body
                std::cout << "\u0e4f";
            else
                std::cout << " ";
        }
        std::cout << "\u2593" << std::endl;
    }
    std::cout << "\n\n\n\n\n\n\n\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
}

/*!
 * @brief check how many cells were accessed
*/
int game::verified_cells(std::vector<way> caminhos){
    int count = 0;
    for(int i(0); i < caminhos.size(); ++i){
        for(int j(0); j < caminhos[i].ways.size(); ++j){
            count++;
        }
    }
    return count;
}

/*!
 * @brief genarates the best way to food
*/
void game::jogada_i(){
    int i = 0;
    int x = std::get<0>(mapas.front().jogador.head);
    int y = std::get<1>(mapas.front().jogador.head);
    std::vector<way> caminhos; // ways possibles
    way a(x,y);
    caminhos.push_back(a);

    // Special case only with the head, not to go back the way you came after eating
    if(posible_bug){
        mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = '!';
        posible_bug = false;
    }
    
    int size_inicial = 0; // only check the paths that are from the beginning of the current while
    int size_cells = 0; // check if don't have a new way
    // while you have not found the food or if you do not have a path to the food
    while((!find_food(caminhos)) and (verified_cells(caminhos) != size_cells)){
        size_cells = verified_cells(caminhos);
        size_inicial = caminhos.size();
        while((i < size_inicial) and !find_food(caminhos)){
            x = std::get<0>(caminhos[i].ways.back());
            y = std::get<1>(caminhos[i].ways.back());
            // If the last position of the way is the food, do not change the way
            if((x == std::get<0>(position_food)) and (y == std::get<1>(position_food))){
                break;
            }
            // If you only have one path to follow
            if(ramificacao(x, y) == 1){
                caminhos[i].ways.push_back(choose_way(x,y));
                caminhos[i].size++;                        
            } else if(ramificacao(x, y) == 2){ // If you have two paths to follow
                caminhos.push_back(caminhos[i]);
                caminhos[i].ways.push_back(choose_way(x,y));
                caminhos[i].size++;
                caminhos.back().ways.push_back(choose_way(x,y));
                caminhos.back().size++;
            } else if(ramificacao(x, y) == 3){ // If you have three paths to follow
                caminhos.push_back(caminhos[i]);
                caminhos.push_back(caminhos[i]);
                caminhos[i].ways.push_back(choose_way(x,y));
                caminhos[i].size++;
                caminhos[caminhos.size()-2].ways.push_back(choose_way(x,y));
                caminhos[caminhos.size()-2].size++;
                caminhos.back().ways.push_back(choose_way(x,y));
                caminhos.back().size++;
            } else if(ramificacao(x,y) == 4){ // If you have four paths to follow
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

            mapas.front().map[std::get<0>(position_food)][std::get<1>(position_food)].situacion = 'f';
            i++;
        }
        if(!posible_bug)
            mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = 'n';
        i = 0;
    }

    jogada_rodada.clear();
    int smaller_size = (mapas.front().rows * mapas.front().columns) +1;
    for(int i(0); i < caminhos.size(); ++i){ // Find the path found until the food
        int x_food = std::get<0>(position_food);
        int y_food = std::get<1>(position_food);
        if((std::get<0>(caminhos[i].ways.back()) == x_food)
        and (std::get<1>(caminhos[i].ways.back()) == y_food)
        and (caminhos[i].size < smaller_size)){
            for(int j(0); j < caminhos[i].size; ++j){ // save the smallest path
                jogada_rodada.ways.push_back(caminhos[i].ways[j]);
                jogada_rodada.size++;
            }
            smaller_size = jogada_rodada.size;
        }
    }
    clear_map();
}

/*!
 * @brief move the snake
*/
void game::move_snake(){
    int x = std::get<0>(mapas.front().jogador.head);
    int y = std::get<1>(mapas.front().jogador.head);
    jogada_i(); // take the best way
    if(jogada_rodada.size != 0 and mapas.front().jogador.body.size() == 0){ // if don't have a body
        if(jogada_rodada.size == 1){
            std::get<0>(mapas.front().jogador.head_l) = x;
            std::get<1>(mapas.front().jogador.head_l) = y;
        }
        for(int i(1); i < jogada_rodada.size; ++i){
            mapas.front().map[x][y].situacion = 'n';


            x = std::get<0>(jogada_rodada.ways[i]);
            y = std::get<1>(jogada_rodada.ways[i]);
            
            if (i == jogada_rodada.size - 2){
                std::get<0>(mapas.front().jogador.head_l) = x;
                std::get<1>(mapas.front().jogador.head_l) = y;
            }

            mapas.front().map[x][y].situacion = 'h';
            if(i == jogada_rodada.size-1){
                create_food();
                points++;
            }
            show_map();
        }
        posible_bug = true;
        std::get<0>(mapas.front().jogador.head) = std::get<0>(jogada_rodada.ways.back());
        std::get<1>(mapas.front().jogador.head) = std::get<1>(jogada_rodada.ways.back());
        std::tuple<int, int> temp(std::get<0>(mapas.front().jogador.head), std::get<1>(mapas.front().jogador.head));
        mapas.front().jogador.body.push_back(temp);
        jogada_rodada.clear();
        score += 100;
        to_dead = 0;  
        
    } else {
        if(jogada_rodada.ways.empty()){ // if dont have a way to food
            if(mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion != 'b'
            and mapas.front().jogador.body.size() < 2)
                mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = '!';
            try_survive();
            add_tail = false;
        } else { // if have a way and have a snake body
            if(add_tail == true){
                mapas.front().map[x][y].situacion = 'b';
                add_tail = false;
            } else {
                move_tail();
            }
            x = std::get<0>(jogada_rodada.ways[1]);
            y = std::get<1>(jogada_rodada.ways[1]);
            
            mapas.front().map[x][y].situacion = 'h';
            
            show_map();

            if(jogada_rodada.size == 2){
                create_food();
                points++;
                std::get<0>(mapas.front().jogador.head) = std::get<0>(jogada_rodada.ways.back());
                std::get<1>(mapas.front().jogador.head) = std::get<1>(jogada_rodada.ways.back());
                std::tuple<int, int> temp(std::get<0>(mapas.front().jogador.head), std::get<1>(mapas.front().jogador.head));
                mapas.front().jogador.body.push_front(temp);
                jogada_rodada.clear();
                add_tail = true;
                score += 100;
            }
            to_dead = 0;  
        }
    } 
}

/*!
 * @brief kills the snake and restarts the map
*/
void game::dead(){
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    while(mapas.front().jogador.body.size() != 0){
        mapas.front().jogador.body.pop_back();
    }

    for(int i(0); i < mapas.front().rows; ++i){
        for(int j(0); j < mapas.front().columns; ++j){
            if(mapas.front().map[i][j].situacion == 'b'){
                mapas.front().map[i][j].situacion = 'n';
            }
        }
    }
    mapas.front().map[std::get<0>(mapas.front().jogador.head_l)][std::get<1>(mapas.front().jogador.head_l)].situacion = 'n';
    mapas.front().map[std::get<0>(mapas.front().jogador.head)][std::get<1>(mapas.front().jogador.head)].situacion = 'n';
    mapas.front().map[std::get<0>(mapas.front().jogador.head_s)][std::get<1>(mapas.front().jogador.head_s)].situacion = 'h';
    std::get<0>(mapas.front().jogador.head) = std::get<0>(mapas.front().jogador.head_s);
    std::get<1>(mapas.front().jogador.head) = std::get<1>(mapas.front().jogador.head_s);
    create_food();
    life--;
    score -= 500;
}

/*!
 * @brief move the snake if there is no way to food
*/
void game::try_survive(){
    bool is_dead = false;
    int x = std::get<0>(mapas.front().jogador.head);
    int y = std::get<1>(mapas.front().jogador.head);
    if(mapas.front().jogador.body.size() == 0){
        while(mapas.front().map[x-1][y].type != '#'){
            mapas.front().map[x][y].situacion = 'n';
            mapas.front().map[x-1][y].situacion = 'h';
            x = x-1;
        }
        dead();
        to_dead = 0;
        is_dead = true;
        return ;
    }

    if(mapas.front().map[x-1][y].type == '0' and mapas.front().map[x-1][y].situacion == 'n' and to_dead <= mapas.front().jogador.body.size()+4){ // para cima
        x = x-1;
        mapas.front().map[x][y].situacion = 'h';
    } else if(mapas.front().map[x][y+1].type == '0' and mapas.front().map[x][y+1].situacion == 'n' and to_dead <= mapas.front().jogador.body.size()+4){ // para direita
        y = y+1;
        mapas.front().map[x][y].situacion = 'h';
    } else if(mapas.front().map[x+1][y].type == '0' and mapas.front().map[x+1][y].situacion == 'n' and to_dead <= mapas.front().jogador.body.size()+4){ // para baixo
        x = x+1;
        mapas.front().map[x][y].situacion = 'h';
    } else if(mapas.front().map[x][y-1].type == '0' and mapas.front().map[x][y-1].situacion == 'n' and to_dead <= mapas.front().jogador.body.size()+4){ // para esquerda
        y = y-1;
        mapas.front().map[x][y].situacion = 'h';
    } else {
        dead();
        to_dead = 0;
        is_dead = true;
    }
    if(mapas.front().jogador.body.size() != 0 and !is_dead){
        move_tail();
    }
    to_dead++;
    if(!is_dead){
        std::get<0>(mapas.front().jogador.head) = x;
        std::get<1>(mapas.front().jogador.head) = y;
    }
    show_map();
}

/*!
 * @brief clear the map of all "!"
*/
void game::clear_map(){
    for(int i(0); i < mapas.front().rows; ++i){
        for(int j(0); j < mapas.front().columns; ++j){
            if(mapas.front().map[i][j].situacion == '!')
                mapas.front().map[i][j].situacion = 'n';
        }
    }
}

/*!
 * @brief Move the last cell of the body to before the head
*/
void game::move_tail(){
    mapas.front().map[std::get<0>(mapas.front().jogador.body.back())][std::get<1>(mapas.front().jogador.body.back())].situacion = 'n';
    mapas.front().jogador.body.pop_back();
    std::tuple<int,int> a(std::get<0>(mapas.front().jogador.head), std::get<1>(mapas.front().jogador.head));
    mapas.front().jogador.body.push_front(a);
    mapas.front().map[std::get<0>(mapas.front().jogador.body.front())][std::get<1>(mapas.front().jogador.body.front())].situacion = 'b';
}

/*!
 * @brief Choose a cell for the snake to follow, return the first free cell you find
*/
std::tuple<int,int> game::choose_way(int x, int y){
    mapas.front().map[x][y].situacion = '!';
    if(mapas.front().map[x+1][y].type == '0' and mapas.front().map[x+1][y].situacion != '!' and mapas.front().map[x+1][y].situacion != 'b'){
        mapas.front().map[x+1][y].situacion = '!';
        std::tuple<int,int> a(x+1, y);
        return a;
    } if(mapas.front().map[x][y-1].type == '0' and mapas.front().map[x][y-1].situacion != '!' and mapas.front().map[x][y-1].situacion != 'b') {
        mapas.front().map[x][y-1].situacion = '!';
        std::tuple<int,int> a(x, y-1);
        return a;
    } if(mapas.front().map[x][y+1].type == '0' and mapas.front().map[x][y+1].situacion != '!' and mapas.front().map[x][y+1].situacion != 'b') {
        mapas.front().map[x][y+1].situacion = '!';
        std::tuple<int,int> a(x, y+1);
        return a;
    } if(mapas.front().map[x-1][y].type == '0' and mapas.front().map[x-1][y].situacion != '!' and mapas.front().map[x-1][y].situacion != 'b') {
        mapas.front().map[x-1][y].situacion = '!';
        std::tuple<int,int> a(x-1, y);
        return a;
    }
}

/*!
 * @brief check for food on the map
*/
bool game::exist_food(){
    for(int i(0); i < mapas.front().rows; ++i){
        for(int j(0); j < mapas.front().columns; ++j){
            if(mapas.front().map[i][j].situacion == 'f')
                return true;
        }
    }
    return false;
}

/*!
 * @brief Check if any path has already found the food
*/
bool game::find_food(std::vector<way> w){
    int x_food = std::get<0>(position_food);
    int y_food = std::get<1>(position_food);
    for(int i(0); i < w.size(); ++i){
        if((std::get<0>(w[i].ways.back()) == x_food) and (std::get<1>(w[i].ways.back()) == y_food))
            return true;
    }
    return false;
}

/*!
 * @brief Check how many possible paths you can follow from the local map [x] [y]
*/
int game::ramificacao(int x, int y){ 
    int count = 0;
    if(mapas.front().map[x+1][y].type == '0' and mapas.front().map[x+1][y].situacion != '!' and mapas.front().map[x+1][y].situacion != 'b')
        count++;
    if(mapas.front().map[x][y-1].type == '0' and mapas.front().map[x][y-1].situacion != '!' and mapas.front().map[x][y-1].situacion != 'b')
        count++;
    if(mapas.front().map[x][y+1].type == '0' and mapas.front().map[x][y+1].situacion != '!' and mapas.front().map[x][y+1].situacion != 'b')
        count++;
    if(mapas.front().map[x-1][y].type == '0' and mapas.front().map[x-1][y].situacion != '!' and mapas.front().map[x-1][y].situacion != 'b')
        count++;

    return count;
}

/*!
 * @brief Create food randomly on the map
*/
void game::create_food(){
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

/*!
 * @brief show where the body is
*/
void game::print_body(std::list<std::tuple<int, int>> body){
    std::cout << "Body of snake:";
    for(auto it(body.begin()); it != body.end(); ++it){
        std::cout << "(" << std::get<0>(*it) << "," << std::get<1>(*it) << ")";
    }
    std::cout << "\n";
}

/*!
 * @brief show a way
*/
void game::print_way(way a){
    int i = 0;
    while(i < a.size){
        std::cout << "(" << std::get<0>(a.ways[i]) << "," << std::get<1>(a.ways[i]) << ")";
        i++;
    }
    std::cout << std::endl << std::endl;
}