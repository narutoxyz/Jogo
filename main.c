#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define PI 3.14159265358979323846264338327950288
#define largura 640
#define altura 480

//Numero de inimigos falta.
typedef struct inimigo Inimigo;
struct inimigo
{
   int tipo; //pra sabe qual é 0 - 1 -2
   int angulo;
   int lado;
   int dx;
   int dy;
   int visivel;
   SDL_Texture *textura;
   SDL_Rect srect;
   SDL_Rect drect; // posicao dele 0 - 1 - 2
   Inimigo *prox;
};

typedef struct info
{
    int numMinInimigos;
    int numAtualInimigos;
    int cooldownSpawn;
    SDL_Rect *srect;
}InfoInimigo;

typedef struct jogador
{
    int vida; // 3 vidas
    int pontuacao; // pontuação que aparece no jogo
    int direcao; //0-Cima; 1-Direita; 2-Esquerda
    int velocidade;
    int cooldownTiro;
    int dx;
    int dy;
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
    int visivel;
    int dx;
    int dy;
    int angulo;
    SDL_Texture *textura;
    SDL_Rect rect;
    Tiro *prox;
};

//Funções
void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, int *musicaAtual, Objeto *textoTitulo, Objeto *textoPlay, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, Objeto* yoda, int opcaoMenu);
void iniciarJogo(SDL_Renderer *renderer, SDL_Texture *jogo, Mix_Music *musicaJogo, int *musicaAtual, Jogador* jogador);
void iniciarRecorde(SDL_Renderer *renderer, SDL_Texture *recorde, Mix_Music *musicaRecorde, int *musicaAtual);
void iniciarCredito(SDL_Renderer *renderer, SDL_Texture *credito, Mix_Music *musicaCredito, int *musicaAtual);

//Funções - Jogo
void mostrarInfoJogo(SDL_Renderer *renderer, Jogador *jogador, Objeto *vida, Objeto *textoPontuacao, TTF_Font *fontePontuacao, SDL_Color branco);
void moverNave(SDL_Renderer *renderer, Jogador *jogador);

Tiro* atirar(Jogador *jogador, Tiro *tiros, SDL_Texture *texturaTiro);
Tiro* moverTiros(SDL_Renderer *renderer, Tiro *tiros);

Inimigo* spawnInimigos(InfoInimigo *infoInimigo, Inimigo* inimigos, SDL_Texture *texturaInimigo);
Inimigo* moverInimigos(SDL_Renderer *renderer, InfoInimigo *infoInimigo, Inimigo* inimigos);

void resetJogo(Jogador *jogador);

//Funções - Auxiliares
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
    surface = IMG_Load("imagens/credito.png");
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

    Tiro *tiros = NULL;

    SDL_Texture *texturaTiro;
    surface = IMG_Load("imagens/tiro.png");
    texturaTiro = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *texturaInimigo;
    surface = IMG_Load("imagens/inimigos.png");
    texturaInimigo = SDL_CreateTextureFromSurface(renderer, surface);

    Jogador *jogador = (Jogador*) malloc(sizeof(Jogador));
    surface = IMG_Load("imagens/nave.png");
    jogador->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux6 = {(largura - 46)/2, (altura - 80)/2, 46, 80}; //Ajeita o tamanho
    jogador->rect = aux6;
    jogador->vida = 3;
    jogador->pontuacao = 0;
    jogador->angulo = 0.0f;
    jogador->velocidade = 5;
    jogador->cooldownTiro = 0;
    jogador->dx = 0;
    jogador->dy = 0;
    jogador->nome = (char*) malloc(sizeof(char)*30);

    InfoInimigo *infoInimigo = (InfoInimigo*) malloc(sizeof(InfoInimigo));
    infoInimigo->numMinInimigos = 5;
    infoInimigo->numAtualInimigos = 0;
    infoInimigo->cooldownSpawn = 100;
    infoInimigo->srect = (SDL_Rect*) malloc(sizeof(SDL_Rect)*3);
    SDL_Rect aux7 = {0,0,467,474};
    infoInimigo->srect[0] = aux7;
    SDL_Rect aux8 = {471,0,869,382};
    infoInimigo->srect[1] = aux8;
    SDL_Rect aux9 = {872,0,1029,74};
    infoInimigo->srect[2] = aux9;

    Inimigo *inimigos = (Inimigo*) malloc(sizeof(Inimigo));

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
                //Musica + Fundo
                iniciarJogo(renderer, jogo, musicaJogo, musicaAtual, jogador);

                //Mover nave
                moverNave(renderer, jogador);

                //Criar e mover tiros
                if(atirou == 1 && jogador->cooldownTiro == 0)
                {
                    tiros = atirar(jogador, tiros, texturaTiro);
                    jogador->cooldownTiro = 50;
                    atirou = 0;
                }
                tiros = moverTiros(renderer, tiros);
                if(jogador->cooldownTiro > 0)
                    jogador->cooldownTiro--;

                //Criar e mover inimigos
                if(infoInimigo->cooldownSpawn == 0 && infoInimigo->numAtualInimigos < infoInimigo->numMinInimigos)
                {
                    printf("Criou\n");
                    inimigos = spawnInimigos(infoInimigo, inimigos, texturaInimigo);
                    infoInimigo->cooldownSpawn = 100;
                }
                inimigos = moverInimigos(renderer, infoInimigo, inimigos);
                if(infoInimigo->cooldownSpawn > 0)
                    infoInimigo->cooldownSpawn--;

                //Pontuação e vida
                mostrarInfoJogo(renderer, jogador, vida, textoPontuacao, fontePontuacao, branco);

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
                    if(evento.key.keysym.sym == SDLK_SPACE)
                    {
                        printf("Fire!!\n");
                        
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
    
void iniciarJogo(SDL_Renderer *renderer, SDL_Texture *jogo, Mix_Music *musicaJogo, int *musicaAtual, Jogador* jogador)
{
    
    SDL_RenderCopy(renderer, jogo, NULL, NULL);

    //Significa que não estava no jogo
    if(*musicaAtual != 1)
    {
        //SDL_RenderCopy(renderer, jogador->textura, NULL, &jogador->rect);
        Mix_PlayMusic(musicaJogo, -1);
    }

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

    //Calcula o dx e dy
    radiano = angleToRad(jogador->angulo);
    jogador->dx = jogador->dy = 0;

    if((jogador->angulo >= 0 && jogador->angulo <=90))
    {
        if(jogador->angulo == 0)
            jogador->dy = - (jogador->velocidade);
        else if(jogador->angulo == 90)
            jogador->dx = jogador->velocidade;
        else //1º quadrante
        {
            jogador->dx = sin(radiano) * jogador->velocidade;
            jogador->dy = - (cos(radiano) * jogador->velocidade);
        }
    }
    else if(jogador->angulo > 90 && jogador->angulo <= 180)
    {
        if(jogador->angulo == 180)
            jogador->dy = jogador->velocidade;
        else//2º quadrante
        {
            jogador->dx = sin(radiano) * jogador->velocidade;
            jogador->dy = - (cos(radiano) * jogador->velocidade);
        }
    }
    else if(jogador->angulo > 180 && jogador->angulo <= 270)
    {
        if(jogador->angulo == 270)
            jogador->dx = - (jogador->velocidade);
        else//3º quadrante
        {
            jogador->dx = sin(radiano) * jogador->velocidade;
            jogador->dy = - (cos(radiano) * jogador->velocidade);
        }
    }
    else //(angulo > 270 && angulo < 360) //4º quadrante
    {
        jogador->dx = sin(radiano) * jogador->velocidade;
        jogador->dy = - (cos(radiano) * jogador->velocidade);
        
    }

    switch(jogador->direcao)
    {
        case 0: //Ir para frente, independente do angulo
        {
            jogador->rect.x += jogador->dx;
            jogador->rect.y += jogador->dy;

            //Colisão da nave com a borda
            int x1, x2, y1, y2;

            x1 = jogador->rect.x;
            x2 = jogador->rect.x + jogador->rect.w;
            y1 = jogador->rect.y;
            y2 = jogador->rect.y + jogador->rect.h;

            if(x1 < 0)
            {
                jogador->rect.x = largura;
            }
            if(x2 > largura) //ok
            {
                jogador->rect.x = 0;
            }
            if(y1 < 0)
            {
                jogador->rect.y = altura;
            }
            if(y2 > altura) //ok
            {
                jogador->rect.y = 0;
            }
            break;
        }
        case 1: //Ir para direita
        {
            jogador->angulo = (int)(jogador->angulo + angulo)%360;
            break;
        }
        case 2: //Ir para esquerda
        {
            jogador->angulo = (int)((jogador->angulo - angulo)+360) % 360;
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

//Precisa melhorar ...
void resetJogo(Jogador *jogador)
{
    jogador->vida = 3;
    jogador->pontuacao = 0;
    jogador->angulo = 0.0f;
    SDL_Rect aux = {(largura - 46)/2, (altura - 80)/2, 46, 80};
    jogador->rect = aux;
}

//Criar
//Adicionar se atirou

Tiro* atirar(Jogador *jogador, Tiro *tiros, SDL_Texture *texturaTiro)
{
    Tiro *lst;
    SDL_Rect aux = {jogador->rect.x + (jogador->rect.w/2 - 16/2), jogador->rect.y-5, 16, 40};
    //SDL_Rect aux = {(jogador->rect.x + (jogador->rect.w/2 - 16/2)) + jogador->dx*2, jogador->rect.y+jogador->dy*2, 16, 40};

    //Cria o tiro
    Tiro *tiro = (Tiro*) malloc(sizeof(Tiro));
    tiro->textura = texturaTiro;
    tiro->visivel = 1;
    tiro->prox = NULL;
    tiro->rect = aux;
    tiro->dx = jogador->dx;
    tiro->dy = jogador->dy;
    tiro->angulo = jogador->angulo;

    //Lista cheia
    if(tiros != NULL) 
    {
        Tiro *temp = NULL;
        for(lst = tiros; lst != NULL; lst = lst->prox)
        {
            temp = lst;
        }
        temp->prox = tiro;
    }
    //Lista Vazia
    else //tiros == NULL
    {
        tiros = tiro;
    }

    return tiros;
}

Tiro* moverTiros(SDL_Renderer *renderer, Tiro *tiros)
{
    Tiro *lst;
    Tiro *temp;
    Tiro *ant = NULL;
    int x1, x2, y1, y2;

    //Se a lista não estiver vazia ...
    if(tiros != NULL)
    {
        //, então removemos tiros que colidiram
        for(lst = tiros; lst != NULL;)
        {
            x1 = lst->rect.x;
            x2 = lst->rect.x + lst->rect.w;
            y1 = lst->rect.y;
            y2 = lst->rect.y + lst->rect.h;

            if(x1 <= 0 || x2 >= largura || y1 <= 0 || y2 >= altura)//verifica colisao
                lst->visivel = 0;

            if(lst->visivel == 0)
            {
                if(ant == NULL) //Nó inicial
                {
                    temp = lst;
                    if(lst->prox == NULL)
                        tiros = NULL;
                    lst = lst->prox;
                    free(temp);
                    continue;
                }
                else //ant != NullNó não inicial
                {
                    if(lst->prox == NULL) //ultimo
                    {
                        ant->prox = NULL;
                        free(lst);
                    }
                    else //meio
                    {
                        temp = lst;
                        ant->prox = lst->prox;
                        lst = lst->prox;
                        free(temp);
                    }
                }
            }
            else //Se for visível
            {
                ant = lst;
                lst = lst->prox;
            }
        }
    }

    //Imprime os tiros na tela
    for(lst = tiros; lst != NULL; lst = lst->prox)
    {
        lst->rect.x += lst->dx*2;
        lst->rect.y += lst->dy*2;
        SDL_RenderCopyEx(renderer, lst->textura, NULL, &lst->rect, lst->angulo, NULL, SDL_FLIP_NONE);
    }

    return tiros;
}

void mostrarInfoJogo(SDL_Renderer *renderer, Jogador *jogador, Objeto *vida, Objeto *textoPontuacao, TTF_Font *fontePontuacao, SDL_Color branco)
{
    int i;
    int espaco = 58;
    char pontuacao[20];
    SDL_Surface *surface;

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
}

//Cria inimigos se a quantidade for menor que x
Inimigo* spawnInimigos(InfoInimigo *infoInimigo, Inimigo* inimigos, SDL_Texture *texturaInimigo)
{   
    //Se o cooldown estiver em 0 e tiver menos que o numMinInimigos ...

    //Criamos um inimigo
    Inimigo* inimigo = (Inimigo*) malloc(sizeof(Inimigo));
    inimigo->tipo = rand() % 3;
    inimigo->lado = rand() % 4; //lado que o inimigo aparece.
    inimigo->angulo = 0;
    inimigo->prox = NULL;
    inimigo->textura = texturaInimigo;
    inimigo->srect = infoInimigo->srect[inimigo->tipo];
    inimigo->dx = inimigo->dy = 0;

    int tamanhoX, tamanhoY;
    tamanhoX = tamanhoY = 0;

    //Tamanho dos inimigos dependendo do tipo
    switch(inimigo->tipo)
    {
        case 0://Maior
        {
            tamanhoX = 80;
            tamanhoY = 80;
            break;
        }
        case 1://Medio
        {
            tamanhoX = 50;
            tamanhoY = 60;
            break;
        }
        case 2://Pequeno
        {
            tamanhoX = 40;
            tamanhoY = 30;
            break;
        }
    }

    int aux, pos;
    int deslocamento = 5;

    //Surgimento
    switch(inimigo->lado)
    {
        case 0://Cima
        {
            aux = largura/tamanhoX;
            pos = (rand() % aux)*tamanhoX;
            SDL_Rect aux1 = {pos,0,tamanhoX,tamanhoY};
            inimigo->drect = aux1;

            inimigo->dy = +deslocamento;
            break;
        }
        case 1://Baixo
        {
            aux = largura/tamanhoX;
            pos = (rand() % aux)*tamanhoX;
            SDL_Rect aux2 = {pos,altura,tamanhoX,tamanhoY};
            inimigo->drect = aux2;

            inimigo->dy = -deslocamento;
            break;
        }
        case 2://Direita
        {
            aux = altura/tamanhoY;
            pos = (rand() % aux)*tamanhoY;
            SDL_Rect aux3 = {largura,pos,tamanhoX,tamanhoY};
            inimigo->drect = aux3;

            inimigo->dx = -deslocamento;
            break;
        }
        case 3://Esquerda
        {
            aux = altura/tamanhoY;
            pos = (rand() % aux)*tamanhoY;
            SDL_Rect aux4 = {0,pos,tamanhoX,tamanhoY};
            inimigo->drect = aux4;

            inimigo->dx = -deslocamento;
            break;
        }
    }

    Inimigo *lst;

    //Lista com itens
    if(inimigos != NULL) 
    {
        Inimigo *temp = NULL;
        for(lst = inimigos; lst != NULL; lst = lst->prox)
        {
            temp = lst;
        }
        temp->prox = inimigo;
    }
    //Lista Vazia
    else //inimigos == NULL
    {
        inimigos = inimigo;
    }

    printf("DX - %d\n", inimigo->dx);

    return inimigos;
}

Inimigo* moverInimigos(SDL_Renderer *renderer, InfoInimigo *infoInimigo, Inimigo* inimigos)
{
    if(inimigos == NULL)
        return inimigos;

    Inimigo *lst;
    Inimigo *temp;
    Inimigo *ant = NULL;
    int x1, x2, y1, y2;

    //Removemos inimigos que colidiram
    for(lst = inimigos; lst != NULL;)
    {
        x1 = lst->drect.x;
        x2 = lst->drect.x + lst->drect.w;
        y1 = lst->drect.y;
        y2 = lst->drect.y + lst->drect.h;

        if(x1 <= 0 || x2 >= largura || y1 <= 0 || y2 >= altura)//verifica colisao
            lst->visivel = 0;

        if(lst->visivel == 0)
        {
            if(ant == NULL) //Nó inicial
            {
                temp = lst;
                if(lst->prox == NULL)
                    inimigos = NULL;
                lst = lst->prox;
                free(temp);
                continue;
            }
            else //ant != NullNó não inicial
            {
                if(lst->prox == NULL) //ultimo
                {
                    ant->prox = NULL;
                    free(lst);
                }
                else //meio
                {
                    temp = lst;
                    ant->prox = lst->prox;
                    lst = lst->prox;
                    free(temp);
                }
            }
        }
        else //Se for visível
        {
            ant = lst;
            lst = lst->prox;
        }
    }

    //Mostrar os inimigos
    for(lst = inimigos; lst != NULL; lst = lst->prox)
    {
        if(lst->tipo == 0 || lst->tipo == 1) //rotacionar
        {
            lst->angulo = (lst->angulo+5) % 360;
            lst->drect.x += lst->dx;
            lst->drect.y += lst->dy;
            SDL_RenderCopyEx(renderer, lst->textura, &lst->srect, &lst->drect, lst->angulo, NULL, SDL_FLIP_NONE);
        }
        else//(lst->tipo == 2) - Espelhar
        {
            lst->drect.x += lst->dx;
            lst->drect.y += lst->dy;
            SDL_RenderCopyEx(renderer, lst->textura, &lst->srect, &lst->drect, lst->angulo, NULL, SDL_FLIP_HORIZONTAL);   
        }
    }
    
    return inimigos;
}