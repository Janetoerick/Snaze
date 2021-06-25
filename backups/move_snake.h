// h = u23fa
// b = u0e4f
// life = u2764

void move_snake(){
            int x = std::get<0>(mapas[atual].jogador.head);
            int y = std::get<1>(mapas[atual].jogador.head);
            int x_, y_;
            if(jogada_rodada.size != 0 and mapas[atual].jogador.body.size() == 0){ // Caso tenha achado o caminho até a comida
                for(int i(1); i < jogada_rodada.size; ++i){
                    if(i < jogada_rodada.size){
                        mapas[atual].map[x][y].situacion = 'n';
                    } 


                    x = std::get<0>(jogada_rodada.ways[i]);
                    y = std::get<1>(jogada_rodada.ways[i]);
                    
                    if (i == jogada_rodada.size - 2){
                        std::get<0>(mapas[atual].jogador.head_l) = x;
                        std::get<1>(mapas[atual].jogador.head_l) = y;
                    }

                    mapas[atual].map[x][y].situacion = 'h';
                    if(i == jogada_rodada.size-1){
                        create_food();
                        mapas[atual].points++;
                    }
                    show_map();
                }
                std::get<0>(mapas[atual].jogador.head) = std::get<0>(jogada_rodada.ways.back());
                std::get<1>(mapas[atual].jogador.head) = std::get<1>(jogada_rodada.ways.back());
                std::tuple<int, int> temp(std::get<0>(mapas[atual].jogador.head), std::get<1>(mapas[atual].jogador.head));
                mapas[atual].jogador.body.push_back(temp);
                jogada_rodada.clear();
                
            } else if(jogada_rodada.size != 0 and mapas[atual].jogador.body.size() != 0){
                // Move the head one position to add new body
                for(int i(1); i < jogada_rodada.size; ++i){
                    if(i == 1 and add_tail == true)
                        mapas[atual].map[x][y].situacion = 'b';
                    else {
                        move_tail();
                    }
                    x = std::get<0>(jogada_rodada.ways[i]);
                    y = std::get<1>(jogada_rodada.ways[i]);
                    
                    mapas[atual].map[x][y].situacion = 'h';
                    if(i == jogada_rodada.size-1){
                        create_food();
                        mapas[atual].points++;
                    }
                    show_map();
                }
                std::get<0>(mapas[atual].jogador.head) = std::get<0>(jogada_rodada.ways.back());
                std::get<1>(mapas[atual].jogador.head) = std::get<1>(jogada_rodada.ways.back());
                std::tuple<int, int> temp(std::get<0>(mapas[atual].jogador.head), std::get<1>(mapas[atual].jogador.head));
                mapas[atual].jogador.body.push_front(temp);
                jogada_rodada.clear();
            }   
        }