#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

//SDL_Rect sRect = {xTextura, yTextura, larguraFigura, alturaFigura};
//SDL_Rect dRect = {xRenderer, yRenderer, larguraFigura, alturaFigura};
//SDL_Surface *surface = IMG_Load("imagens/recorde.jpg");
//SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, surface);
//char dir[] = "imagens/spritenave.png";
//char dir[] = "imagens/inimigos.png";
//Mix_PlayMusic(musica, -1);

/*---Estruturas---*/

typedef struct inimigo
{
    int numInimigos; // 3 inimigos
    SDL_Texture *textura;
    SDL_Rect *sprite;
}Inimigo;

typedef struct jogador
{
    int vida; // 3 vidas
    char *nome;
    SDL_Texture *textura;
    SDL_Rect *sprite;
}Jogador;

void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu);

int main(int argc, char *argv[])
{
    //VARIAVEIS SIMPLES
    int largura = 640, altura = 480;
    int jogando = 1, escolha = 0;

    //INICIALIZADORES DO SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *janela = SDL_CreateWindow("jogo",50,50, largura, altura,SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(janela,-1,0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

	//VARIAVEIS SDL, STRUCTS

    Mix_Music *musicaMenu = Mix_LoadMUS("musicas/menu.ogg");
    Mix_Music *musicaJogo = Mix_LoadMUS("musicas/jogo.ogg");

    SDL_Event evento;

    SDL_Surface *surface;
    SDL_Texture *textura;

    SDL_Texture *menu;
    surface = IMG_Load("imagens/menu.jpeg");
    menu = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *jogo;
    surface = IMG_Load("imagens/jogo.jpeg");
    jogo = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *recorde;
    surface = IMG_Load("imagens/recorde.jpg");
    recorde = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *credito;
    surface = IMG_Load("imagens/credito.jpeg");
    credito = SDL_CreateTextureFromSurface(renderer, surface);

    Jogador *jogador = (Jogador*) malloc(sizeof(Jogador));
    surface = IMG_Load("imagens/nave.png");
    jogador->textura = SDL_CreateTextureFromSurface(renderer, surface);
    Inimigo *inimigo = (Inimigo*) malloc(sizeof(Inimigo));
    surface = IMG_Load("imagens/inimigos.png");
    inimigo->textura = SDL_CreateTextureFromSurface(renderer, surface);

    jogador->vida = 3;
    jogador->nome = (char*) malloc(sizeof(char)*30);
    inimigo->numInimigos = 3;

    jogador->sprite = (SDL_Rect*) malloc(sizeof(SDL_Rect)*20);
    inimigo->sprite = (SDL_Rect*) malloc(sizeof(SDL_Rect)*4);

    //SDL_Rect aux = {0,0,159,94};
    //jogador->sprite[0] = aux;//Cima

    //SDL_Rect aux = {0,0,467,474};
    //inimigo->sprite[0] = aux;

    //textura = menu;

	while(jogando == 1)
    {
        SDL_RenderClear(renderer);

        switch(escolha)
        {
            case 0://Menu
            {
                iniciarMenu(renderer, menu, musicaMenu);
                break;
            }
            case 1://Jogo
            {
                break;
            }
            case 2://Recorde
            {
                break;
            } 
            case 3://Créditos
            {
                break;
            }
        }
        while(SDL_PollEvent(&evento) != 0)
        {
            //Fechou a tela
            if(evento.type == SDL_QUIT)
            {
                jogando = 0;
            }
            //Tecla prescionada
            if(evento.type == SDL_KEYDOWN)
            {
                // Se a tecla for ESC ...
                if(evento.key.keysym.sym == SDLK_ESCAPE)
                {
                    jogando = 0; //Encerre o Game Loop
                }
                                /*Movimentação Nave*/
                // Se a tecla for -> no jogo
                if(evento.key.keysym.sym == SDLK_UP && escolha == 1)
                {
                    //mover(UP);
                }
                if(evento.key.keysym.sym == SDLK_DOWN && escolha == 1)
                {
                    //mover(DOWN);
                }
                if(evento.key.keysym.sym == SDLK_RIGHT && escolha == 1)
                {
                    //girar(RIGHT);
                }
                if(evento.key.keysym.sym == SDLK_LEFT && escolha == 1)
                {
                    //girar(LEFT);
                }
            }
        }

        //SDL_RenderCopy(renderer, textura, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000/30);
    }

	return 0;
}

//void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, SDL_Texture *textura, Mix_Music *musicaMenu)

void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu)
{
    if(Mix_PlayingMusic() == 0)
        Mix_PlayMusic(musicaMenu, -1);

    SDL_RenderCopy(renderer, menu, NULL, NULL);
    return;
}