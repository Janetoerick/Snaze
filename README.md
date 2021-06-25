Projeto Snake

Esse projeto tem como objetivo reproduzir o jogo "snake".
Foi feito de maneira autonoma, ou seja, o snake tem inteligência própria, o usuário apenas deve definir os mapas para o jogo.

Para definir o mapa:
Deve-se criar um arquivo txt e chama-lo na hora de compilar. Pode se colocar vários mapas de uma vez no arquivo.
Para cada mapa coloca-se a quantidade de linhas e depois a quantidade de colunas, logo após o mapa(exemplos na pasta "test").
No mapa desenhado pode ter:
*" * " - ponto inicial do snake(obrigatório em cada mapa).
" # "- parede
" 0 " - caminho livre
" . " - parede invisível

Para compilar:
Pelo terminal abra a pasta "src" e digite "g++ main.cpp -std=c++11 -o ex" para gerar o executável "ex", após isso digite "./ex local_dos_mapas.txt" para executar com os mapas desejados.

OBS: O score funciona da seguinte forma:
Cada vez que o snake anda uma casa, ganha 10
Caso pegue um café ganha 100
Caso morra, perde 500
    
Autor:
Janeto Erick da Costa Lima
