#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

using namespace std;

// Tipos necessários para a execução do algoritmo

struct Ponto{
    float x, y, z;
    void set(float Px, float Py, float Pz){
        x = Px;
        y = Py;
        z = Pz;
    }
};

struct Vetor{
    float x, y, z;
    void set(Ponto P0, Ponto P1){
        x = P1.x - P0.x;
        y = P1.y - P0.y;
        z = P1.z - P0.z;
    }
};

struct Esfera{
    Ponto centro;
    float r;
    void set(Ponto Ecentro, float Eraio){
        centro = Ecentro;
        r = Eraio;
    }
};

struct Raio{
    Ponto P0, Pp;
    Vetor dr;
    void set(Ponto p0, Ponto pp){
        P0 = p0;
        Pp = pp;
        dr.set(P0, Pp);
    }
};

// Funções necessárias para se trabalhar com os vetores

float dot(Vetor u, Vetor v);

Vetor mulscalar(float k, Vetor v);

float norm(Vetor v);

bool checarColisao(Raio R, Esfera E);

int main(int argc, char* argv[]){
    // Definindo o pintor
    Ponto pintor;
    pintor.set(0,0,0);
    
    // Definindo a janela
    float wJanela = 1280, hJanela = 720, dJanela = 10;

    // Definindo a esfera
    float rEsfera = 15;
    Ponto cEsfera;
    cEsfera.set(0,0,-(dJanela+rEsfera));
    Esfera E;
    E.set(cEsfera, rEsfera);

    // Definindo a matriz de cores
    int nLin = 80, nCol = 45, esfColor[] = {255, 0, 0}, bgColor[] = {100, 100, 100};
    int canvas[nLin][nCol][3];

    // Algoritmo responsável por preencher o canvas
    int x, y, z, Dx, Dy;
    bool colisao;
    Ponto P;
    Raio R;

    Dx = wJanela/nLin;
    Dy = hJanela/nCol;
    z = -dJanela;
    for(int l=0; l < nLin; l++){
        y = hJanela/2 - Dy/2 - l*Dy;
        for(int c=0; c < nCol; c++){
            x = -wJanela/2 + Dx/2 + c*Dx;
            P.set(x, y, z);
            R.set(pintor, P);
            R.dr = mulscalar(1/norm(R.dr), R.dr);
            // Checagem da presença ou não de colisão entre o raio R ,que vai do pintor até P, e a esfera E
            colisao = checarColisao(R, E);
            if(colisao){
                canvas[l][c][0] = esfColor[0];
                canvas[l][c][1] = esfColor[1];
                canvas[l][c][2] = esfColor[2];
            }else{
                canvas[l][c][0] = bgColor[0];
                canvas[l][c][1] = bgColor[1];
                canvas[l][c][2] = bgColor[2];
            } 
        }
    }

    // Desenho do círculo a partir do canvas (ambiente SDL)
    // Inicialização da biblioteca
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_Log("Não foi possível inicializar o SDL! SDL_Error: %s", SDL_GetError());
        return -1;
    }

    // Criação da janela SDL
    SDL_Window* window = SDL_CreateWindow("CG I - Tarefa 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wJanela, hJanela, SDL_WINDOW_SHOWN);
    
    // Checagem se a criação da janela foi bem sucedida
    if(!window){
        SDL_Log("Criação da janela falhou! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Criação do renderer SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Checagem se a criação do renderer foi bem sucedida
    if(!renderer){
        SDL_Log("Criação do renderer falhou! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Main loop
    bool isRunning = true; // Variável de controle do loop
    SDL_Event event; // Variável para rastreio de eventos presentes na janela
    while(isRunning){
        // Laço responsável por encerrar a janela em caso de evento de fechamento de janela
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                isRunning = false;
            }
        }

        // Limpeza do renderer antes do desenho do pixel
        SDL_RenderClear(renderer);

        // Desenho do pixel para cada um presente na janela
        for(int x=0; x <nLin; x++){
            for(int y = 0; y < nCol; y++){
                // Configuração da cor do pixel a ser pintado pelo padrão RGBA
                SDL_SetRenderDrawColor(renderer, canvas[x][y][0], canvas[x][y][1], canvas[x][y][2], 255);
                // Desenho do pixel
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        // Atualização da janela pelo renderer
        SDL_RenderPresent(renderer);
    }

    // Limpeza dos objetos não mais utilizados na memória
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

float dot(Vetor u, Vetor v){
    /*
    Função que realiza o produto interno entre os vetores u e v.
    */
        return u.x*v.x + u.y*v.y + u.z*v.z;
}

Vetor mulscalar(float k, Vetor v){
    Vetor kv = v;
    kv.x *= k; 
    kv.y *= k;
    kv.z *= k;
    return kv;
}

float norm(Vetor v){
    /*
    Função que computa a norma do vetor v através de um produto interno.
    */
    return sqrt(dot(v, v));
}

bool checarColisao(Raio R, Esfera E){
    /*
    Função que checa se o raio R colide em 1 ou 2 pontos com a esfera E.
    */
    float a, b, c;
    Vetor V;

    V.set(E.centro, R.P0);
    a = dot(R.dr, R.dr);
    b = 2 * dot(V, R.dr);
    c = dot(V, V) - E.r*E.r;

    return ((b * b - 4*a*c) >= 0);
}