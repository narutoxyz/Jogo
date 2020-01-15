#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define PI 3.14159265358979323846264338327950288
#define largura 640
#define altura 480

//SDL_Rect sRect = {xTextura, yTextura, larguraFigura, alturaFigura};
//SDL_Rect dRect = {xRenderer, yRenderer, larguraFigura, alturaFigura};
//SDL_Surface *surface = IMG_Load("imagens/recorde.jpg");
//SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, surface);
//char dir[] = "imagens/rectnave.png";
//char dir[] = "imagens/inimigos.png";

/*Ideias - Jogo*/
/*
Nave:
-Move-se com as setas: ><, fazem a nave girar, enquanto ^ movimenta ela até parar por conta do atrito.
-Essa parte de ir parando ainda não tenho ideia.
-A parte de girar será feita com o RenderCopyEX.
-A movimentação ^ será feita incrementando/decrementando dependendo da angulação que está a nave.
-O tiro tem uma variável cooldown da nave.

Inimigos:
-Surgimento: Escolher aleatoriamente um dos lados e depois uma posição naquele lado e para mover vai ir incrementando/decremen
tando o x,y do rect do inimigo.
-rect: Podemos ao mover, mudar a angulação deles de maneira constante.
-Uma variavel para controlar a quantidade de inimigos
    if(NumInimigos < 3)
    {
        Chama outro inimigo
    }
    else
    {
        for(i < 3)
        {
            mover inimigos.
        }
    }

Tiro:
-Tempo: Definir uma variável cooldown, onde a cada iteração do loop ela é decrementada e quando chegar a 0. Você pode atirar
novamente, quando atirou ela reseta para 2000(ex).
-Animação: O tiro precisa sair da frente da nave ao apertar a barra de espaço, para se movimentar, incrementamos/decrementamos
o x e y do rect na direção da angulação da nave.
*/

/*---Estruturas---*/

// typedef struct inimigo
// {
//     int numInimigos; // 3 inimigos
//     SDL_Texture *textura;
//     SDL_Rect *rect;
// }Inimigo;


//Numero de inimigos falta.
typedef struct inimigo Inimigos;
struct inimigo
{
   int tipo; //pra sabe qual é 0 - 1 -2
   SDL_Texture *textura;
   SDL_Rect rect[3]; // posicao dele 0 - 1 - 2
   Inimigos *prox;
};

typedef struct jogador
{
    int vida; // 3 vidas
    int pontuacao; // pontuação que aparece no jogo
    int direcao; //0-Cima; 1-Direita; 2-Esquerda
    int velocidade;
    int cooldown;
    float angulo;
    char *nome;
    SDL_Texture *textura;
    SDL_Rect rect;
}Jogador;

typedef struct obj
{
    SDL_Texture *textura;
    SDL_Rect rect;
}Objeto;

typedef struct tiro Tiro;
struct tiro
{
    SDL_Texture *textura;
    SDL_Rect rect;
    Tiro *prox;
};

typedef struct borda
{
    int superior;
    int inferior;
    int direita;
    int esquerda;
}Borda;

//Funções
void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, int *musicaAtual, Objeto *textoTitulo, Objeto *textoPlay, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, Objeto* yoda, int opcaoMenu);
void iniciarJogo(SDL_Renderer *renderer, SDL_Texture *jogo, Mix_Music *musicaJogo, int *musicaAtual, Jogador* jogador, Objeto *vida, Objeto *textoPontuacao, TTF_Font *fontePontuacao, SDL_Color branco);
void iniciarRecorde(SDL_Renderer *renderer, SDL_Texture *recorde, Mix_Music *musicaRecorde, int *musicaAtual);
void iniciarCredito(SDL_Renderer *renderer, SDL_Texture *credito, Mix_Music *musicaCredito, int *musicaAtual);

void moverNave(SDL_Renderer *renderer, Jogador *jogador);
void resetJogo(Jogador *jogador);

float angleToRad(float angulo);

int main(int argc, char *argv[])
{
    //VARIAVEIS SIMPLES
    int jogando = 1;
    int escolha = 0; //0-Menu, 1-Jogo, 2-Recorde, 3-Creditos
    int opcaoMenu = 0;//0-Jogar, 1-Recorde, 2-Creditos
    int tam;
    int atirou = 0;

    int *musicaAtual = (int*) malloc(sizeof(int));
    *musicaAtual = 0;

    //INICIALIZADORES DO SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *janela = SDL_CreateWindow("Space Attack",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,largura, altura,SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(janela,-1,0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
    TTF_Init();

	//VARIAVEIS SDL

    TTF_Font *fonteStarWars = TTF_OpenFont("letras/Starjedi.ttf", 52);
    TTF_Font *titulo = TTF_OpenFont("letras/Starjout.ttf", 60);
    TTF_Font *fontePontuacao = TTF_OpenFont("letras/Robotica.ttf", 48);

    SDL_Color verde = {7,224,71};
    SDL_Color amarelo = {233,225,0};
    SDL_Color branco = {255,255,255};

    SDL_Event evento;
    SDL_Surface *surface;

    //Musicas

    Mix_Music *musicaMenu = Mix_LoadMUS("musicas/menu.ogg");
    Mix_Music *musicaJogo = Mix_LoadMUS("musicas/jogo.ogg");
    Mix_Music *musicaRecorde = Mix_LoadMUS("musicas/recorde.ogg");
    Mix_Music *musicaCredito = Mix_LoadMUS("musicas/credito.ogg");

    //Textos

    Objeto *textoPlay = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Jogar", verde);
    textoPlay->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux1 = {(largura - surface->w)/2, (altura - surface->h)/2, surface->w, surface->h};
    textoPlay->rect = aux1;

    tam = surface->h;//Altura do texto

    Objeto *textoRecorde = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Recordes", verde);
    textoRecorde->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux2 = {(largura - surface->w)/2, tam+((altura - surface->h)/2), surface->w, surface->h};
    textoRecorde->rect = aux2;

    Objeto *textoCredito = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Creditos", verde);
    textoCredito->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux3 = {(largura - surface->w)/2, 2*tam+((altura - surface->h)/2), surface->w, surface->h};
    textoCredito->rect = aux3;

    Objeto *textoTitulo = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(titulo, "Space Attack", amarelo);
    textoTitulo->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux4 = {(largura - surface->w)/2, 0, surface->w, surface->h};
    textoTitulo->rect = aux4;

    Objeto *textoPontuacao = (Objeto*) malloc(sizeof(Objeto));
    SDL_Rect aux5 = {(largura - 50)/2, 0, 50, surface->h/2};
    textoPontuacao->rect = aux5;

    int bordaSuperior = surface->h;

    //Structs
    Borda *bordasJogo = (Borda*) malloc(sizeof(Borda));
    bordasJogo->superior = bordaSuperior;
    bordasJogo->inferior = altura;
    bordasJogo->direita = largura;
    bordasJogo->esquerda = 0;

    //Fundos
    SDL_Texture *menu;
    surface = IMG_Load("imagens/menu.jpg");
    menu = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *jogo;
    surface = IMG_Load("imagens/jogo.jpg");
    jogo = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *recorde;
    surface = IMG_Load("imagens/recorde.jpg");
    recorde = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *credito;
    surface = IMG_Load("imagens/credito.jpg");
    credito = SDL_CreateTextureFromSurface(renderer, surface);

    //Imagens Gerais
    Objeto *alien = (Objeto*) malloc(sizeof(Objeto));
    surface = IMG_Load("imagens/alien.png");
    alien->textura = SDL_CreateTextureFromSurface(renderer, surface);
    Objeto *yoda = (Objeto*) malloc(sizeof(Objeto));
    surface = IMG_Load("imagens/babyyoda.png");
    yoda->textura = SDL_CreateTextureFromSurface(renderer, surface);
    Objeto *vida = (Objeto*) malloc(sizeof(Objeto));
    surface = IMG_Load("imagens/vida.png");
    vida->textura = SDL_CreateTextureFromSurface(renderer, surface);
    Tiro *tiros = (Tiro*) malloc(sizeof(Tiro));
    surface = IMG_Load("imagens/tiro.png");
    tiros->textura = SDL_CreateTextureFromSurface(renderer, surface);


    Jogador *jogador = (Jogador*) malloc(sizeof(Jogador));
    surface = IMG_Load("imagens/nave.png");
    jogador->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux6 = {(largura - 46)/2, (altura - 80)/2, 46, 80}; //Ajeita o tamanho
    jogador->rect = aux6;
    jogador->vida = 3;
    jogador->pontuacao = 0;
    jogador->angulo = 0.0f;
    jogador->velocidade = 5;
    jogador->cooldown = 2000;
    jogador->nome = (char*) malloc(sizeof(char)*30);

    // Inimigo *inimigo = (Inimigo*) malloc(sizeof(Inimigo));
    // surface = IMG_Load("imagens/inimigos.png");
    // inimigo->textura = SDL_CreateTextureFromSurface(renderer, surface);
    // inimigo->numInimigos = 3;
    // inimigo->rect = (SDL_Rect*) malloc(sizeof(SDL_Rect)*3);
    
    

    //SDL_Rect aux = {0,0,159,94};
    //jogador->rect[0] = aux;//Cima

    //SDL_Rect aux = {0,0,467,474};
    //inimigo->rect[0] = aux;

    //textura = menu;

	while(jogando == 1)
    {
        SDL_RenderClear(renderer);

        switch(escolha)
        {
            case 0://Menu
            {
                iniciarMenu(renderer, menu, musicaMenu, musicaAtual, textoTitulo, textoPlay, textoRecorde, textoCredito, alien, yoda, opcaoMenu);
                break;
            }
            case 1://Jogo
            {
                iniciarJogo(renderer, jogo, musicaJogo, musicaAtual, jogador, vida, textoPontuacao, fontePontuacao, branco);
                moverNave(renderer, jogador);
                if(atirou == 1)
                {
                    //atirar(jogador, Tiro *tiros);
                    atirou = 0;
                }
                
                break;
            }
            case 2://Recorde
            {
                iniciarRecorde(renderer, recorde, musicaRecorde, musicaAtual);
                break;
            } 
            case 3://Créditos
            {
                iniciarCredito(renderer, credito, musicaCredito, musicaAtual);
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
                // Se a tecla for 0 ...
                if(evento.key.keysym.sym == SDLK_0)
                {
                    if(escolha == 1) //Se eu estava no jogo, então eu preciso resetar as variáveis.
                    {
                        resetJogo(jogador);
                    }
                    escolha = 0; //Volte ao menu
                }
                                
                if(escolha == 0) //Menu
                {
                    printf("Estrou no menu.\n");
                            /*Seleção - Menu*/
                    if(evento.key.keysym.sym == SDLK_RETURN || evento.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        switch(opcaoMenu)
                        {
                            case 0: //Jogar
                            {
                                escolha = 1;
                                break;
                            }
                            case 1: //Recorde
                            {
                                escolha = 2;
                                break;
                            }
                            case 2: //Creditos
                            {
                                escolha = 3;
                                break;
                            }
                        }
                    }
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
                    printf("Entrou Jogo.\n");

                                        /*Movimentação Nave - Jogo*/
                    //Alterar a direção e o rect usando uma função então precisa de uma variavel direção
                    if(evento.key.keysym.sym == SDLK_UP)
                    {
                        jogador->direcao = 0;
                    }
                    if(evento.key.keysym.sym == SDLK_RIGHT)
                    {
                        jogador->direcao = 1;
                    }
                    if(evento.key.keysym.sym == SDLK_LEFT)
                    {
                        jogador->direcao = 2;
                    }
                    if(evento.key.keysym.sym == SDLK_BACKSPACE)
                    {
                        atirou = 1;
                    }
                }
                if(escolha == 2) //Recorde
                {
                    printf("Entrou Recorde.\n");
                }
                if(escolha == 3) //Credito
                {
                    printf("Entrou Credito.\n");
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

void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, int *musicaAtual, Objeto *textoTitulo, Objeto *textoPlay, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, Objeto* yoda, int opcaoMenu)
{
    if(Mix_PlayingMusic() == 0) //Se não estiver tocando música ...
    {
        Mix_PlayMusic(musicaMenu, -1); //Toque a musica do menu
    }
    else // Se estiver tocando musica ...
    {
        if(*musicaAtual != 0) // Se não for a do menu
        {
            Mix_PlayMusic(musicaMenu, -1); //Toque a musica do menu
        }
    }

    SDL_RenderCopy(renderer, menu, NULL, NULL);

    SDL_RenderCopy(renderer, textoTitulo->textura, NULL, &textoTitulo->rect);
    SDL_RenderCopy(renderer, textoPlay->textura, NULL, &textoPlay->rect);
    SDL_RenderCopy(renderer, textoRecorde->textura, NULL, &textoRecorde->rect);
    SDL_RenderCopy(renderer, textoCredito->textura, NULL, &textoCredito->rect);

    switch(opcaoMenu)
    {
        case 0: //Botão Jogar
        {
            //Alien
            SDL_Rect aux1 = {textoPlay->rect.x - 60, textoPlay->rect.y + 20,48,50};
            alien->rect = aux1;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);

            //Baby Yoda
            SDL_Rect aux11 = {textoPlay->rect.x + textoPlay->rect.w, textoPlay->rect.y + 20,60,58};
            yoda->rect = aux11;
            SDL_RenderCopy(renderer, yoda->textura, NULL, &yoda->rect);
            break;
        }
        case 1: //Botão Recorde
        {
            SDL_Rect aux2 = {textoRecorde->rect.x - 60, textoRecorde->rect.y + 20,48,50};
            alien->rect = aux2;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);

            //Baby Yoda
            SDL_Rect aux22 = {textoRecorde->rect.x  + textoRecorde->rect.w, textoRecorde->rect.y + 20,60,58};
            yoda->rect = aux22;
            SDL_RenderCopy(renderer, yoda->textura, NULL, &yoda->rect);
            break;
        }
        case 2: //Botão Creditos
        {
            SDL_Rect aux3 = {textoCredito->rect.x - 60, textoCredito->rect.y + 20,48,50};
            alien->rect = aux3;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);

            //Baby Yoda
            SDL_Rect aux33 = {textoCredito->rect.x  + textoCredito->rect.w, textoCredito->rect.y + 20,60,58};
            yoda->rect = aux33;
            SDL_RenderCopy(renderer, yoda->textura, NULL, &yoda->rect);
            break;
        }
    }

    *musicaAtual = 0;

    return;
}
    
void iniciarJogo(SDL_Renderer *renderer, SDL_Texture *jogo, Mix_Music *musicaJogo, int *musicaAtual, Jogador* jogador, Objeto *vida, Objeto *textoPontuacao, TTF_Font *fontePontuacao, SDL_Color branco)
{
    int i;
    int espaco = 58;
    char pontuacao[20];
    SDL_Surface *surface;


    SDL_RenderCopy(renderer, jogo, NULL, NULL);

    //Significa que não estava no jogo
    if(*musicaAtual != 1)
    {
        SDL_RenderCopy(renderer, jogador->textura, NULL, &jogador->rect);
        Mix_PlayMusic(musicaJogo, -1);
    }

    SDL_Rect aux = {0,0,48,textoPontuacao->rect.h};
    vida->rect = aux;

    for(i = 0; i < jogador->vida; i++)
    {
        vida->rect.x += i*espaco;
        SDL_RenderCopy(renderer, vida->textura, NULL, &vida->rect);
        vida->rect = aux;
    }

    sprintf(pontuacao, "%d", jogador->pontuacao);

    surface = TTF_RenderText_Solid(fontePontuacao, pontuacao, branco);
    textoPontuacao->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, textoPontuacao->textura, NULL, &textoPontuacao->rect);

    *musicaAtual = 1;
}

void iniciarRecorde(SDL_Renderer *renderer, SDL_Texture *recorde, Mix_Music *musicaRecorde, int *musicaAtual)
{
    if(*musicaAtual != 2)
    {
        Mix_PlayMusic(musicaRecorde, -1);
    }

    SDL_RenderCopy(renderer, recorde, NULL, NULL);

    *musicaAtual = 2;
}

void iniciarCredito(SDL_Renderer *renderer, SDL_Texture *credito, Mix_Music *musicaCredito, int *musicaAtual)
{
    if(*musicaAtual != 3)
    {
        Mix_PlayMusic(musicaCredito, -1);
    }

    SDL_RenderCopy(renderer, credito, NULL, NULL);

    *musicaAtual = 3;
}

//Altera os valores do rect
void moverNave(SDL_Renderer *renderer, Jogador *jogador)
{
    float angulo = 5.0f;
    float radiano;

    switch(jogador->direcao)
    {
        case 0: //Ir para cima
        {
            radiano = angleToRad(jogador->angulo);
            if((jogador->angulo >= 0 && jogador->angulo <=90))
            {
                if(jogador->angulo == 0)
                    jogador->rect.y -= jogador->velocidade;
                else if(jogador->angulo == 90)
                    jogador->rect.x += jogador->velocidade;
                else //1º quadrante
                {
                    jogador->rect.x += sin(radiano) * jogador->velocidade;
                    jogador->rect.y -= cos(radiano) * jogador->velocidade;
                }
            }
            else if(jogador->angulo > 90 && jogador->angulo <= 180)
            {
                if(jogador->angulo == 180)
                    jogador->rect.y += jogador->velocidade;
                else//2º quadrante
                {
                    jogador->rect.x += sin(radiano) * jogador->velocidade;
                    jogador->rect.y -= cos(radiano) * jogador->velocidade;
                }
            }
            else if(jogador->angulo > 180 && jogador->angulo <= 270)
            {
                if(jogador->angulo == 270)
                    jogador->rect.x -= jogador->velocidade;
                else//3º quadrante
                {
                    jogador->rect.x += sin(radiano) * jogador->velocidade;
                    jogador->rect.y -= cos(radiano) * jogador->velocidade;
                }
            }
            else //(angulo > 270 && angulo < 360) //4º quadrante
            {
                jogador->rect.x += sin(radiano) * jogador->velocidade;
                jogador->rect.y -= cos(radiano) * jogador->velocidade;
            }
            
            break;
        }
        case 1: //Ir para direita
        {
            jogador->angulo = (int)(jogador->angulo + angulo)%360;
            printf("Angulo D %.2f\n", jogador->angulo);
            break;
        }
        case 2: //Ir para esquerda
        {
            jogador->angulo = (int)((jogador->angulo - angulo)+360) % 360;
            printf("Angulo E %.2f\n", jogador->angulo);
            break;
        }
    }
    jogador->direcao = 4;

    SDL_RenderCopyEx(renderer, jogador->textura, NULL, &jogador->rect, jogador->angulo, NULL, SDL_FLIP_NONE);
}

float angleToRad(float angulo)
{
    return angulo*(PI/180.0f);
}

void resetJogo(Jogador *jogador)
{
    jogador->vida = 3;
    jogador->pontuacao = 0;
    jogador->angulo = 0.0f;
    SDL_Rect aux = {(largura - 46)/2, (altura - 80)/2, 46, 80};
    jogador->rect = aux;
}

// void atirar(Jogador *jogador, Tiro *tiros)
// {
//     Tiro *tiro;
//     for(tiro = tiros; p != NULL; p = p->prox)
//     {
//         if()
//     }
//     Tiro *tiro = 
    

// }

/* LISTA
void imprime (celula *le) {
   celula *p;
   for (p = le; p != NULL; p = p->prox)
      printf ("%d\n", p->conteudo);
}
*/