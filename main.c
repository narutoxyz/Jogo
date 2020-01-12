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

typedef struct obj
{
    SDL_Texture *textura;
    SDL_Rect rect;
}Objeto;

void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, Objeto *textoPlay, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, int opcao);

int main(int argc, char *argv[])
{
    //VARIAVEIS SIMPLES
    int largura = 640, altura = 480;
    int jogando = 1;
    int escolha = 0; //0-Menu, 1-Jogo, 2-Recorde, 3-Creditos
    int opcaoMenu = 0;//0-Jogar, 1-Recorde, 2-Creditos

    //INICIALIZADORES DO SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *janela = SDL_CreateWindow("jogo",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,largura, altura,SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(janela,-1,0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
    TTF_Init();

	//VARIAVEIS SDL, STRUCTS

    TTF_Font *fonteStarWars = TTF_OpenFont("letras/Starjedi.ttf", 32);
    SDL_Color verde = {7,224,71};

    Mix_Music *musicaMenu = Mix_LoadMUS("musicas/menu.ogg");
    Mix_Music *musicaJogo = Mix_LoadMUS("musicas/jogo.ogg");

    SDL_Event evento;

    SDL_Surface *surface;
    SDL_Texture *textura;

    //Textos
    Objeto *textoPlay = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Jogar", verde);
    textoPlay->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux1 = {250, 150, 395, 182};
    textoPlay->rect = aux1;

    Objeto *textoRecorde = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Recordes", verde);
    textoRecorde->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux2 = {250, 214, 395, 246};
    textoRecorde->rect = aux2;

    Objeto *textoCredito = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Creditos", verde);
    textoCredito->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux3 = {250, 278, 395, 310};
    textoCredito->rect = aux3;

    //Fundos
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

    //Imagens alien Toy Story, nave, inimigos
    Objeto *alien = (Objeto*) malloc(sizeof(Objeto));
    surface = IMG_Load("imagens/alien.png");
    alien->textura = SDL_CreateTextureFromSurface(renderer, surface);
    //alien->rect = {200,150,30,30};
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
                iniciarMenu(renderer, menu, musicaMenu, textoPlay, textoRecorde, textoCredito, alien, opcaoMenu);
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
                                
                if(escolha == 0) //Menu
                {
                            /*Movimentação de Seleção Texto - Menu*/
                    if(evento.key.keysym.sym == SDLK_UP)
                    {
                        if(opcaoMenu-1 >= 0)
                        {
                            opcaoMenu = opcaoMenu-1;
                        }
                    }
                    if(evento.key.keysym.sym == SDLK_DOWN)
                    {
                        if(opcaoMenu + 1 <=2 )
                        {
                            opcaoMenu = opcaoMenu+1;
                        }
                    }
                }
                if(escolha == 1) //Jogo
                {
                                        /*Movimentação Nave - Jogo*/
                    if(evento.key.keysym.sym == SDLK_UP)
                    {
                        //mover(UP);
                    }
                    if(evento.key.keysym.sym == SDLK_DOWN)
                    {
                        //mover(DOWN);
                    }
                    if(evento.key.keysym.sym == SDLK_RIGHT)
                    {
                        //girar(RIGHT);
                    }
                    if(evento.key.keysym.sym == SDLK_LEFT)
                    {
                        //girar(LEFT);
                    }
                }
            }
        }

        //SDL_RenderCopy(renderer, textura, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000/30);
    }

    //Free
    //TTF_CloseFont(fonteStarWars);
    //TTF_Quit();

	return 0;
}

//void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, SDL_Texture *textura, Mix_Music *musicaMenu)

void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, Objeto *textoPlay, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, int opcaoMenu)
{
    if(Mix_PlayingMusic() == 0)
        Mix_PlayMusic(musicaMenu, -1);

    SDL_RenderCopy(renderer, menu, NULL, NULL);

    SDL_RenderCopy(renderer, textoPlay->textura, NULL, &textoPlay->rect);
    SDL_RenderCopy(renderer, textoRecorde->textura, NULL, &textoRecorde->rect);
    SDL_RenderCopy(renderer, textoCredito->textura, NULL, &textoCredito->rect);

    switch(opcaoMenu)
    {
        case 0: //Botão Jogar
        {
            SDL_Rect aux1 = {218,150,64,64};
            alien->rect = aux1;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);
            break;
        }
        case 1: //Botão Recorde
        {
            SDL_Rect aux2 = {218,214,64,64};
            alien->rect = aux2;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);
            break;
        }
        case 2: //Botão Creditos
        {
            SDL_Rect aux3 = {218,278,64,64};
            alien->rect = aux3;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);
            break;
        }

    }

    return;
}