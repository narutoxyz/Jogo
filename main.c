#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
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
   int pontos;
   int angulo;
   int flip;
   int cooldownFlip;
   int lado;
   int dx;
   int dy;
   int visivel;
   SDL_Texture *textura;
   SDL_Rect rect; // posicao dele 0 - 1 - 2
   Inimigo *prox;
};

typedef struct info
{
    int numMaxInimigos;
    int numAtualInimigos;
    int cooldownSpawn;
    int numeroSpawn;
    int velocidade;
    SDL_Texture *textura0;
    SDL_Texture *textura1;
    SDL_Texture *textura2;
}InfoInimigo;

typedef struct jogador
{
    int vida; // 3 vidas
    int pontuacao; // pontuação que aparece no jogo
    int direcao; //0-Cima; 1-Direita; 2-Esquerda
    float velocidade;
    float aceleracao;
    int cooldownTiro;
    int cooldownMov;
    int dx;
    int dy;
    char* nome;
    float angulo;
    float velAngular;
    int empty;
    SDL_Texture *textura;
    SDL_Rect rect;
}Jogador;

typedef struct obj
{
    SDL_Texture *textura;
    SDL_Rect rect;
}Objeto;

typedef struct infoTiro
{
	int velocidade;
	int dx;
	int dy;
}InfoTiro;

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

typedef struct inforank
{
    int numRanqueados;
    int numRanks;
}InfoRank;

typedef struct rank
{
    char nome[10];
    int pontuacao;
}Rank;

//Funções
void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, int *musicaAtual, Objeto *textoTitulo, Objeto *textoPlay, Objeto *textoInstrucao, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, Objeto* yoda, int opcaoMenu, Mix_Chunk* somSelect, int* efeitoSonoro);
void iniciarJogo(SDL_Renderer *renderer, SDL_Texture *jogo, Mix_Music *musicaJogo, int *musicaAtual, Jogador* jogador);
void iniciarRecorde(SDL_Renderer *renderer, SDL_Texture *recorde, Mix_Music *musicaRecorde, int *musicaAtual);
void iniciarCredito(SDL_Renderer *renderer, SDL_Texture *credito, Mix_Music *musicaCredito, int *musicaAtual);
void iniciarInstrucao(SDL_Renderer *renderer, SDL_Texture *instrucao, Mix_Music *musicaInstrucao, int *musicaAtual);

//Funções - Jogo
void mostrarInfoJogo(SDL_Renderer *renderer, Jogador *jogador, Objeto *vida, Objeto *textoPontuacao, TTF_Font *fontePontuacao, SDL_Color branco);
void moverNave(SDL_Renderer *renderer, Jogador *jogador, InfoTiro* infoTiro);

Tiro* atirar(Jogador *jogador, Tiro *tiros, SDL_Texture *texturaTiro, Mix_Chunk *tiro, InfoTiro* infoTiro);
Tiro* moverTiros(SDL_Renderer *renderer, Tiro *tiros);

Inimigo* spawnInimigos(InfoInimigo *infoInimigo, Inimigo* inimigos, int tipo);
Inimigo* moverInimigos(SDL_Renderer *renderer, InfoInimigo *infoInimigo, Inimigo* inimigos);

void colisao(Jogador* jogador,InfoInimigo* infoInimigo, Inimigo* inimigos, Tiro* tiros, Mix_Chunk* explosion);

void iniciarFimDeJogo(SDL_Renderer *renderer, Objeto *textoGameOver, Objeto *textoNome, Objeto *textoJogarNovamente, Objeto *textoYes, Objeto *textoNo, Mix_Music *musicaFimDeJogo, int *musicaAtual, Jogador* jogador, int opcaoFimDeJogo, TTF_Font *fontePixel, SDL_Color verde, SDL_Color branco, Mix_Chunk* somSelect, int* efeitoSonoro);
void resetJogo(SDL_Renderer* renderer, Jogador *jogador, Inimigo* inimigos, Tiro* tiros, Objeto* textoNome, TTF_Font *fontePixel, SDL_Color branco, InfoInimigo* infoInimigo);

int escrever(SDL_Renderer* renderer, Jogador *jogador, TTF_Font *fontePixel, SDL_Color branco, Objeto *textoGameOver, Objeto *textoNome, Objeto *textoJogarNovamente, Objeto *textoYes, Objeto *textoNo, int* jogando);
void salvarDadosJogador(InfoRank* infoRank, Rank* ranking, Jogador* jogador, FILE* arqRecorde);
int compararRanks(const void* a, const void* b);
void mostrarRanking(SDL_Renderer* renderer, Rank* ranking, InfoRank* infoRank, Objeto* textoRank, TTF_Font *fonteRecorde, SDL_Color branco, FILE* arqRecorde);

//Funções - Auxiliares
float angleToRad(float angulo);
void verificaNomes(Rank* ranking, InfoRank* infoRank);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    //ARQUIVOS
    FILE* arqRecorde = fopen("recordes.dat", "r+b");
    if(arqRecorde == NULL)
    {
        printf("Erro: problema ao abrir o recorde");
        return 0;
    }

    //VARIAVEIS SIMPLES
    int i;
    int escolha = 0; //0-Menu, 1-Jogo, 2-Recorde, 3-Creditos
    int opcaoMenu = 0;//0-Jogar, 1-Recorde, 2-Creditos
    int opcaoFimDeJogo = 0;
    int tam;
    int atirou = 0;

    int *jogando = (int*) malloc(sizeof(int));
    *jogando = 1;

    int *efeitoSonoro = (int*) malloc(sizeof(int));
    *efeitoSonoro = 0;

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
    TTF_Font *fonteGameOver = TTF_OpenFont("letras/PixelCaps.ttf", 42);
    TTF_Font *fontePixel = TTF_OpenFont("letras/PixelCaps.ttf", 32);
    TTF_Font *fonteRecorde = TTF_OpenFont("letras/AnimeAce.ttf", 24);

    SDL_Color verde = {7,224,71,255};
    SDL_Color amarelo = {233,225,0,255};
    SDL_Color branco = {255,255,255,255};

    SDL_Event evento;
    SDL_Surface *surface;

    //Musicas

    Mix_Music *musicaMenu = Mix_LoadMUS("musicas/menu.ogg");
    Mix_Music *musicaJogo = Mix_LoadMUS("musicas/jogo.ogg");
    Mix_Music *musicaRecorde = Mix_LoadMUS("musicas/recorde.ogg");
    Mix_Music *musicaCredito = Mix_LoadMUS("musicas/credito.ogg");
    Mix_Music *musicaFimDeJogo = Mix_LoadMUS("musicas/gameover.ogg");
    Mix_Music *musicaInstrucao = Mix_LoadMUS("musicas/instrucao.ogg");

    //Efeitos Sonoros

    Mix_Chunk* somSelect = Mix_LoadWAV("efeitos/select.wav");
    Mix_Chunk* somTiro = Mix_LoadWAV("efeitos/tiro.wav");
    Mix_Chunk* somExplosion = Mix_LoadWAV("efeitos/explosion.wav");

    //Textos

    int start = 120;

    Objeto *textoPlay = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Jogar", verde);
    textoPlay->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux1 = {(largura - surface->w)/2, start, surface->w, surface->h};
    textoPlay->rect = aux1;

    tam = surface->h;//Altura do texto

    Objeto *textoRecorde = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Recordes", verde);
    textoRecorde->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux2 = {(largura - surface->w)/2, tam+start, surface->w, surface->h};
    textoRecorde->rect = aux2;

    Objeto *textoCredito = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "Creditos", verde);
    textoCredito->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux3 = {(largura - surface->w)/2, 2*tam+start, surface->w, surface->h};
    textoCredito->rect = aux3;

    Objeto *textoInstrucao = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteStarWars, "instrucoes", verde);
    textoInstrucao->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux13 = {(largura - surface->w)/2, 3*tam+start, surface->w, surface->h};
    textoInstrucao->rect = aux13;

    Objeto *textoTitulo = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(titulo, "Space Attack", amarelo);
    textoTitulo->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux4 = {(largura - surface->w)/2, 0, surface->w, surface->h};
    textoTitulo->rect = aux4;

    Objeto *textoPontuacao = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fontePontuacao, "0", branco);
    textoPontuacao->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux5 = {(largura - 50)/2, 0, surface->w, surface->h};
    textoPontuacao->rect = aux5;

    Objeto *textoRank = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteRecorde, "Nome - Pontos", branco);
    textoRank->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux12 = {30, 100, surface->w, surface->h};
    textoRank->rect = aux12;

    //Tela de Fim

    Objeto *textoGameOver = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fonteGameOver, "Game Over", branco);
    textoGameOver->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux6 = {(largura - surface->w)/2, 50, surface->w, surface->h};
    textoGameOver->rect = aux6;

    Objeto *textoNome = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fontePixel, "Nome:", branco);
    textoNome->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux7 = {0, 180, surface->w, surface->h};
    textoNome->rect = aux7;

    Objeto *textoJogarNovamente = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fontePixel, "Jogar Novamente", branco);
    textoJogarNovamente->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux8 = {(largura - surface->w)/2, altura - 180, surface->w, surface->h};
    textoJogarNovamente->rect = aux8;

    Objeto *textoYes = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fontePixel, "Yes", branco);
    textoYes->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux9 = {((largura - surface->w)/2)-100, altura - 100, surface->w, surface->h};
    textoYes->rect = aux9;

    Objeto *textoNo = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fontePixel, "No", branco);
    textoNo->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux10 = {((largura - surface->w)/2)+100, altura - 100, surface->w, surface->h};
    textoNo->rect = aux10;

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
    SDL_Texture *instrucao;
    surface = IMG_Load("imagens/instrucoes.png");
    instrucao = SDL_CreateTextureFromSurface(renderer, surface);

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

    InfoTiro* infoTiro = (InfoTiro*) malloc(sizeof(InfoTiro));
    infoTiro->velocidade = 10;
    infoTiro->dx = 0;
    infoTiro->dy = 0;

    SDL_Texture *texturaTiro;
    surface = IMG_Load("imagens/tiro.png");
    texturaTiro = SDL_CreateTextureFromSurface(renderer, surface);

    Jogador *jogador = (Jogador*) malloc(sizeof(Jogador));
    jogador->nome = (char*) malloc(sizeof(char)*10);

    for(i = 0; i < 10; i++)
    {
        jogador->nome[i] = ' ';
    }

    surface = IMG_Load("imagens/nave.png");
    jogador->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux11 = {(largura - 46)/2, (altura - 80)/2, 46, 80}; //Ajeita o tamanho
    jogador->rect = aux11;
    jogador->vida = 3;
    jogador->pontuacao = 0;
    jogador->angulo = 0.0f;
    jogador->velAngular = 10.0f;
    jogador->velocidade = 10.0f;
    jogador->aceleracao = 0.0f;
    jogador->cooldownTiro = 0;
    jogador->cooldownMov = 10;
    jogador->dx = 0;
    jogador->dy = 0;
    jogador->empty = 0;

    InfoInimigo *infoInimigo = (InfoInimigo*) malloc(sizeof(InfoInimigo));
    infoInimigo->numMaxInimigos = 5;
    infoInimigo->numAtualInimigos = 0;
    infoInimigo->cooldownSpawn = 100;
    infoInimigo->numeroSpawn = 2;
    infoInimigo->velocidade = 5;
    surface = IMG_Load("imagens/inimigo0.png");
    infoInimigo->textura0 = SDL_CreateTextureFromSurface(renderer, surface);
    surface = IMG_Load("imagens/inimigo1.png");
    infoInimigo->textura1 = SDL_CreateTextureFromSurface(renderer, surface);
    surface = IMG_Load("imagens/inimigo2.png");
    infoInimigo->textura2 = SDL_CreateTextureFromSurface(renderer, surface);

    Inimigo *inimigos = NULL;

    //Talvez tenha que ler essa info do arquivo
    InfoRank* infoRank = (InfoRank*) malloc(sizeof(InfoRank));
    infoRank->numRanqueados = 0;
    infoRank->numRanks = 10;

    Rank ranking[infoRank->numRanks];

	while(*jogando == 1)
    {
        SDL_RenderClear(renderer);

        switch(escolha)
        {
            case 0://Menu
            {
                iniciarMenu(renderer, menu, musicaMenu, musicaAtual, textoTitulo, textoPlay, textoInstrucao, textoRecorde, textoCredito, alien, yoda, opcaoMenu, somSelect, efeitoSonoro);
                break;
            }
            case 1://Jogo
            {
                if(jogador->vida <= 0)// break AQUI
                {
                    printf("Sem vida\n");
                    escolha = 5;
                    break;
                }

                //Musica + Fundo
                iniciarJogo(renderer, jogo, musicaJogo, musicaAtual, jogador);

                colisao(jogador, infoInimigo, inimigos, tiros, somExplosion);

                //Mover nave
                moverNave(renderer, jogador, infoTiro);
                if(jogador->cooldownMov > 0)
		    		jogador->cooldownMov--;
		    	else
		    		jogador->cooldownMov = 10;

                //Criar e mover tiros
                if(atirou == 1 && jogador->cooldownTiro == 0)
                {
                    tiros = atirar(jogador, tiros, texturaTiro, somTiro, infoTiro);
                    jogador->cooldownTiro = 50;
                    atirou = 0;
                }
                tiros = moverTiros(renderer, tiros);
                if(jogador->cooldownTiro > 0)
                    jogador->cooldownTiro--;

                //Criar e mover inimigos
                if(infoInimigo->cooldownSpawn == 0 && infoInimigo->numAtualInimigos < infoInimigo->numMaxInimigos)
                {
                    inimigos = spawnInimigos(infoInimigo, inimigos, 3);
                    infoInimigo->cooldownSpawn = 100;
                    //printf("Nº de Inimigos: %d\n", infoInimigo->numAtualInimigos);
                }
                inimigos = moverInimigos(renderer, infoInimigo, inimigos);
                if(infoInimigo->cooldownSpawn > 0)
                    infoInimigo->cooldownSpawn--;

                //printf("Cooldown: %d\n", infoInimigo->cooldownSpawn);

                //printf("N de Inimigos: %d\n", infoInimigo->numAtualInimigos);

                //Pontuação e vida
                mostrarInfoJogo(renderer, jogador, vida, textoPontuacao, fontePontuacao, branco);

                break;
            }
            case 2://Recorde
            {
                iniciarRecorde(renderer, recorde, musicaRecorde, musicaAtual);
                mostrarRanking(renderer, ranking, infoRank, textoRank, fonteRecorde, branco, arqRecorde);
                break;
            } 
            case 3://Créditos
            {
                iniciarCredito(renderer, credito, musicaCredito, musicaAtual);
                break;
            }
            case 4://Fim de Jogo
            {
            	iniciarInstrucao(renderer, instrucao, musicaInstrucao, musicaAtual);
                break;
            }
            case 5://Instrucoes
            {
            	iniciarFimDeJogo(renderer, textoGameOver, textoNome, textoJogarNovamente, textoYes, textoNo, musicaFimDeJogo, musicaAtual, jogador, opcaoFimDeJogo, fontePixel, verde, branco, somSelect, efeitoSonoro);
                if(opcaoFimDeJogo == 0)
                {
                    opcaoFimDeJogo = escrever(renderer, jogador, fontePixel, branco, textoGameOver, textoNome, textoJogarNovamente, textoYes, textoNo, jogando);
                }
            	break;
            }
        }

        while(SDL_PollEvent(&evento) != 0)
        {
            //Fechou a tela
            if(evento.type == SDL_QUIT)
            {
                *jogando = 0;
            }
            //Tecla prescionada
            if(evento.type == SDL_KEYDOWN)
            {
                // Se a tecla for ESC ...
                if(evento.key.keysym.sym == SDLK_ESCAPE)
                {
                    *jogando = 0; //Encerre o Game Loop
                }
                // Se a tecla for 0 ...
                if(evento.key.keysym.sym == SDLK_0)
                {
                    if(escolha == 1) //Se eu estava no jogo, então eu preciso resetar as variáveis.
                    {
                        resetJogo(renderer, jogador, inimigos, tiros, textoNome, fontePixel, branco, infoInimigo);
                        opcaoFimDeJogo = 0;
                    }
                    escolha = 0; //Volte ao menu
                }
                                
                if(escolha == 0) //Menu
                {
                    printf("Entrou Menu.\n");
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
                            case 3: //Instrucao
                            {
                            	escolha = 4;
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
                            *efeitoSonoro = 1;
                        }
                    }
                    if(evento.key.keysym.sym == SDLK_DOWN)
                    {
                        if(opcaoMenu + 1 <= 3)
                        {
                            opcaoMenu = opcaoMenu+1;
                            *efeitoSonoro = 1;
                        }
                    }
                }
                if(escolha == 1) //Jogo
                {
                                        /*Movimentação Nave - Jogo*/
                    //Alterar a direção e o rect usando uma função então precisa de uma variavel direção
                    if(evento.key.keysym.sym == SDLK_UP)
                    {
                        jogador->direcao = 0;
                        if(jogador->aceleracao + 0.1f <= 1.1f)
                        	jogador->aceleracao += 0.1f; 
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
                if(escolha == 4) //Instrucao
                {
                    printf("Entrou Instrucao.\n");
                }
                if(escolha == 5) //Fim de Jogo
                {
                    printf("Entro no Fim de Jogo\n");

                    //Quando terminar de escrever o Yes ficará verde.
                    if(opcaoFimDeJogo != 0)
                    {
                        if(evento.key.keysym.sym == SDLK_LEFT)
                        {
                            opcaoFimDeJogo = 1;
                            *efeitoSonoro = 1;
                        }
                        if(evento.key.keysym.sym == SDLK_RIGHT)
                        {
                            opcaoFimDeJogo = 2;
                            *efeitoSonoro = 1;
                        }
                        if(evento.key.keysym.sym == SDLK_RETURN || evento.key.keysym.sym == SDLK_KP_ENTER)
                        {
                            if(opcaoFimDeJogo == 1) //Jogar Novamente
                            {
                                //Precisa salvar os dados do jogador, caso ele tenha conseguido uma pontuação maior do que os top 10
                                /*Lê: A quantidade de jogadores salvos, se for abaixo de 10 salvar na posição vazia, ordenar o vetor
                                e escrever no arquivo. Caso sejam 10 ranquados, comparamos a pontuação do jogador atual com a pontuação
                                do ultimo ranqueado do vetor. Dai se a pontuação do ranqueado for maior do que a do novo jogador, nada
                                fazemos. Entretanto, caso ela seja menor, vamos substituir o nome e a pontuação do ranquedo pelos do 
                                jogador. Por fim, salvamos no arquivo*/ 
                                salvarDadosJogador(infoRank, ranking, jogador, arqRecorde);
                                resetJogo(renderer, jogador, inimigos, tiros, textoNome, fontePixel, branco, infoInimigo);
                                escolha = 1;
                                opcaoFimDeJogo = 0;
                            }
                            else if(opcaoFimDeJogo == 2) //Voltar ao Menu
                            {
                                //Precisa salvar os dados do jogador, caso ele tenha conseguido uma pontuação maior do que os top 10
                                salvarDadosJogador(infoRank, ranking, jogador, arqRecorde);
                                resetJogo(renderer, jogador, inimigos, tiros, textoNome, fontePixel, branco, infoInimigo); //Reseta os dados do jogador, para uma nova partida.
                                escolha = 0;
                                opcaoFimDeJogo = 0;
                            }
                        }
                    }
                }
            }
        }

        //SDL_RenderCopy(renderer, textura, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000/60);
    }

    //Free
    //TTF_CloseFont(fonteStarWars);
    //TTF_Quit();

	return 0;
}

void iniciarMenu(SDL_Renderer *renderer, SDL_Texture *menu, Mix_Music *musicaMenu, int *musicaAtual, Objeto *textoTitulo, Objeto *textoPlay, Objeto *textoInstrucao, Objeto *textoRecorde, Objeto *textoCredito, Objeto* alien, Objeto* yoda, int opcaoMenu, Mix_Chunk* somSelect, int* efeitoSonoro)
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
    SDL_RenderCopy(renderer, textoInstrucao->textura, NULL, &textoInstrucao->rect);

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
        case 3: //Botão Instrucao
        {
            SDL_Rect aux4 = {textoInstrucao->rect.x - 60, textoInstrucao->rect.y + 20,48,50};
            alien->rect = aux4;
            SDL_RenderCopy(renderer, alien->textura, NULL, &alien->rect);

            //Baby Yoda
            SDL_Rect aux44 = {textoInstrucao->rect.x  + textoInstrucao->rect.w, textoInstrucao->rect.y + 20,60,58};
            yoda->rect = aux44;
            SDL_RenderCopy(renderer, yoda->textura, NULL, &yoda->rect);
            break;
        }
    }

    if(*efeitoSonoro == 1)
    {
        Mix_PlayChannel(-1, somSelect, 0);
        *efeitoSonoro = 0;
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

void iniciarFimDeJogo(SDL_Renderer *renderer, Objeto *textoGameOver, Objeto *textoNome, Objeto *textoJogarNovamente, Objeto *textoYes, Objeto *textoNo, Mix_Music *musicaFimDeJogo, int *musicaAtual, Jogador* jogador, int opcaoFimDeJogo, TTF_Font *fontePixel, SDL_Color verde, SDL_Color branco, Mix_Chunk* somSelect, int* efeitoSonoro)
{
    SDL_Surface *surface;

    if(*musicaAtual != 4)
    {
        Mix_PlayMusic(musicaFimDeJogo, -1);
    }

    SDL_SetRenderDrawColor(renderer, 0,0,0,255);

    //Onde está a selecao? No Nome, no Yes, no No.
    switch(opcaoFimDeJogo)
    {
        case 0:
        {
            surface = TTF_RenderText_Solid(fontePixel, "Yes", branco);
            textoYes->textura = SDL_CreateTextureFromSurface(renderer, surface);
            surface = TTF_RenderText_Solid(fontePixel, "No", branco);
            textoNo->textura = SDL_CreateTextureFromSurface(renderer, surface);
            break;
        }
        //Se for 0, ainda está escrevendo, então Yes/No estão brancos
        //Vamos mudar para 1 quando terminar de escrever
        case 1://Yes
        {
            surface = TTF_RenderText_Solid(fontePixel, "Yes", verde);
            textoYes->textura = SDL_CreateTextureFromSurface(renderer, surface);
            surface = TTF_RenderText_Solid(fontePixel, "No", branco);
            textoNo->textura = SDL_CreateTextureFromSurface(renderer, surface);
            break;
        }
        case 2://No
        {
            surface = TTF_RenderText_Solid(fontePixel, "No", verde);
            textoNo->textura = SDL_CreateTextureFromSurface(renderer, surface);
            surface = TTF_RenderText_Solid(fontePixel, "Yes", branco);
            textoYes->textura = SDL_CreateTextureFromSurface(renderer, surface);
            break;
        }
    }

    SDL_RenderCopy(renderer, textoGameOver->textura, NULL, &textoGameOver->rect);
    SDL_RenderCopy(renderer, textoNome->textura, NULL, &textoNome->rect);
    SDL_RenderCopy(renderer, textoJogarNovamente->textura, NULL, &textoJogarNovamente->rect);
    SDL_RenderCopy(renderer, textoYes->textura, NULL, &textoYes->rect);
    SDL_RenderCopy(renderer, textoNo->textura, NULL, &textoNo->rect);

    if(*efeitoSonoro == 1)
    {
        Mix_PlayChannel(-1, somSelect, 0);
        *efeitoSonoro = 0;
    }

    *musicaAtual = 4;
}

void iniciarInstrucao(SDL_Renderer *renderer, SDL_Texture *instrucao, Mix_Music *musicaInstrucao, int *musicaAtual)
{
	if(*musicaAtual != 5)
    {
        Mix_PlayMusic(musicaInstrucao, -1);
    }

    SDL_RenderCopy(renderer, instrucao, NULL, NULL);

    *musicaAtual = 5;
}

//Altera os valores do rect
void moverNave(SDL_Renderer *renderer, Jogador *jogador, InfoTiro* infoTiro)
{
    float radiano;

    //Calcula o dx e dy
    radiano = angleToRad(jogador->angulo);
    jogador->dx = jogador->dy = 0;
    infoTiro->dx = infoTiro->dy = 0;

    //atrito
    jogador->velocidade *= jogador->aceleracao;
    printf("Aceleracao: %.2f\n", jogador->aceleracao);
    printf("Velocidade: %.2f\n\n", jogador->velocidade);

    if((jogador->angulo >= 0 && jogador->angulo <=90))
    {
        if(jogador->angulo == 0)
        {
            jogador->dy = - (jogador->velocidade);
            infoTiro->dy = - (infoTiro->velocidade);
        }
        else if(jogador->angulo == 90)
        {
            jogador->dx = jogador->velocidade;
            infoTiro->dx = infoTiro->velocidade;
        }
        
        else //1º quadrante
        {
            jogador->dx = sin(radiano) * jogador->velocidade;
            jogador->dy = - (cos(radiano) * jogador->velocidade);
            infoTiro->dx = sin(radiano) * infoTiro->velocidade;
            infoTiro->dy = - (cos(radiano) * infoTiro->velocidade);
        }
    }
    else if(jogador->angulo > 90 && jogador->angulo <= 180)
    {
        if(jogador->angulo == 180)
        {
            jogador->dy = jogador->velocidade;
            infoTiro->dy = infoTiro->velocidade;
        }
        else//2º quadrante
        {
            jogador->dx = sin(radiano) * jogador->velocidade;
            jogador->dy = - (cos(radiano) * jogador->velocidade);
            infoTiro->dx = sin(radiano) * infoTiro->velocidade;
            infoTiro->dy = - (cos(radiano) * infoTiro->velocidade);
        }
    }
    else if(jogador->angulo > 180 && jogador->angulo <= 270)
    {
        if(jogador->angulo == 270)
        {
            jogador->dx = - (jogador->velocidade);
            infoTiro->dx = - (infoTiro->velocidade);
        }
        else//3º quadrante
        {
            jogador->dx = sin(radiano) * jogador->velocidade;
            jogador->dy = - (cos(radiano) * jogador->velocidade);
            infoTiro->dx = sin(radiano) * infoTiro->velocidade;
            infoTiro->dy = - (cos(radiano) * infoTiro->velocidade);
        }
    }
    else //(angulo > 270 && angulo < 360) //4º quadrante
    {
        jogador->dx = sin(radiano) * jogador->velocidade;
        jogador->dy = - (cos(radiano) * jogador->velocidade);
        infoTiro->dx = sin(radiano) * infoTiro->velocidade;
        infoTiro->dy = - (cos(radiano) * infoTiro->velocidade);
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
                jogador->rect.x = largura-jogador->rect.w;
            }
            if(x2 > largura)
            {
                jogador->rect.x = 0;
            }
            if(y1 < 0)
            {
                jogador->rect.y = altura-jogador->rect.h;
            }
            if(y2 > altura)
            {
                jogador->rect.y = 0;
            }
            break;
        }
        case 1: //Ir para direita
        {
            jogador->angulo = (int)(jogador->angulo + jogador->velAngular)%360;
            jogador->direcao = 4;
            jogador->aceleracao = 0.0f;
            break;
        }
        case 2: //Ir para esquerda
        {
            jogador->angulo = (int)((jogador->angulo - jogador->velAngular)+360) % 360;
            jogador->direcao = 4;
            jogador->aceleracao = 0.0f;
            break;
        }
    }

    //jogador->direcao = 4;

    SDL_RenderCopyEx(renderer, jogador->textura, NULL, &jogador->rect, jogador->angulo, NULL, SDL_FLIP_NONE);

    //atrito

	printf("Velocidade: %.2f\n", jogador->velocidade);

    if(jogador->velocidade > 0 && jogador->direcao == 0 && jogador->cooldownMov == 0)
    {
    	printf("Freio\n");
    	printf("Velocidade: %.2f\n", jogador->velocidade);
    	printf("-Aceleracao: %.2f\n", jogador->aceleracao);
    	jogador->aceleracao -= 0.1f;
    	printf("+Aceleracao: %.2f\n----------------------\n", jogador->aceleracao);
    }
   	if(jogador->velocidade == 0)
    	jogador->direcao = 4;

    jogador->velocidade = 10.0f;
}

float angleToRad(float angulo)
{
    return angulo*(PI/180.0f);
}

//Precisa melhorar ...
void resetJogo(SDL_Renderer* renderer, Jogador *jogador, Inimigo* inimigos, Tiro* tiros, Objeto* textoNome, TTF_Font *fontePixel, SDL_Color branco, InfoInimigo* infoInimigo)
{
    SDL_Surface* surface;
    int i;

    jogador->vida = 3;
    jogador->empty = 0;
    jogador->pontuacao = 0;
    jogador->angulo = 0.0f;
    jogador->aceleracao = 0.0f;
    jogador->velocidade = 10.0f;
    jogador->cooldownMov = 10;
    SDL_Rect aux = {(largura - jogador->rect.w)/2, (altura - jogador->rect.h)/2, jogador->rect.w, jogador->rect.h};
    jogador->rect = aux;

    surface = TTF_RenderText_Solid(fontePixel, "Nome:", branco);
    textoNome->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux2 = {0, 180, surface->w, surface->h};
    textoNome->rect = aux2;

    free(jogador->nome);
    jogador->nome = (char*) malloc(sizeof(char)*10);

    for(i = 0; i < 10; i++)
    {
        jogador->nome[i] = ' ';
    }

    Tiro* lstT;

    if(tiros != NULL)
    {
        //, então removemos tiros que colidiram
        for(lstT = tiros; lstT != NULL; lstT=lstT->prox)
        {
            lstT->visivel = 0;
        }
    }

    Inimigo* lstI;

    if(inimigos != NULL)
    {
        //, então removemos tiros que colidiram
        for(lstI = inimigos; lstI != NULL; lstI=lstI->prox)
        {
            lstI->visivel = 0;
        }
    }

    infoInimigo->cooldownSpawn = 0;
    infoInimigo->numAtualInimigos = 0;
}

//Criar
//Adicionar se atirou

Tiro* atirar(Jogador *jogador, Tiro *tiros, SDL_Texture *texturaTiro, Mix_Chunk *somTiro, InfoTiro* infoTiro)
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
    tiro->dx = infoTiro->dx;
    tiro->dy = infoTiro->dy;
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

    //Som de tiro
    Mix_PlayChannel(-1, somTiro, 0);

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
                    else
                        tiros = lst->prox;
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
                        lst = NULL;
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
Inimigo* spawnInimigos(InfoInimigo *infoInimigo, Inimigo* inimigos, int tipo)
{   
    //Se o cooldown estiver em 0 e tiver menos que o numMaxInimigos ...

    //Criamos um inimigo
    Inimigo* inimigo = (Inimigo*) malloc(sizeof(Inimigo));
    
    if(tipo == 3)//Cria aleatóriamente
        inimigo->tipo = rand() % 3;
    else//Cria um tipo específico
        inimigo->tipo = tipo;

    inimigo->lado = rand() % 4; //lado que o inimigo aparece.
    inimigo->angulo = 0;
    inimigo->flip = 0;
    inimigo->cooldownFlip = 40;
    inimigo->visivel = 1;
    inimigo->prox = NULL;
    inimigo->dx = inimigo->dy = 0;

    int tamanhoX, tamanhoY;
    tamanhoX = tamanhoY = 0;

    //Tamanho dos inimigos dependendo do tipo
    //Escolha da textura
    switch(inimigo->tipo)
    {
        case 2://Maior
        {
            tamanhoX = 80;
            tamanhoY = 80;
            inimigo->textura = infoInimigo->textura0;
            inimigo->pontos = 3;
            break;
        }
        case 1://Medio
        {
            tamanhoX = 60;
            tamanhoY = 60;
            inimigo->textura = infoInimigo->textura1;
            inimigo->pontos = 2;
            break;
        }
        case 0://Pequeno
        {
            tamanhoX = 40;
            tamanhoY = 40;
            inimigo->textura = infoInimigo->textura2;
            inimigo->pontos = 1;
            break;
        }
    }

    int aux, pos;
    //int deslocamento = 5;

    //Surgimento
    switch(inimigo->lado)
    {
        case 0://Cima
        {
            aux = largura/tamanhoX;
            pos = (rand() % aux)*tamanhoX;
            SDL_Rect aux1 = {pos,0,tamanhoX,tamanhoY};
            inimigo->rect = aux1;

            inimigo->dy = infoInimigo->velocidade;
            break;
        }
        case 1://Baixo
        {
            aux = largura/tamanhoX;
            pos = (rand() % aux)*tamanhoX;
            SDL_Rect aux2 = {pos,altura,tamanhoX,tamanhoY};
            inimigo->rect = aux2;

            inimigo->dy = -(infoInimigo->velocidade);
            break;
        }
        case 2://Direita
        {
            aux = altura/tamanhoY;
            pos = (rand() % aux)*tamanhoY;
            SDL_Rect aux3 = {largura,pos,tamanhoX,tamanhoY};
            inimigo->rect = aux3;

            inimigo->dx = -(infoInimigo->velocidade);
            break;
        }
        case 3://Esquerda
        {
            aux = altura/tamanhoY;
            pos = (rand() % aux)*tamanhoY;
            SDL_Rect aux4 = {0,pos,tamanhoX,tamanhoY};
            inimigo->rect = aux4;

            inimigo->dx = infoInimigo->velocidade;
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

    infoInimigo->numAtualInimigos++; //Foi criado um inimigo

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
        x1 = lst->rect.x;
        x2 = lst->rect.x - lst->rect.w;
        y1 = lst->rect.y;
        y2 = lst->rect.y - lst->rect.h;

        if(x1 < 0 || x2 > largura || y1 < 0 || y2 > altura)//verifica colisao
        {
            printf("2\n");
            lst->visivel = 0;
            infoInimigo->numAtualInimigos--;
        }

        if(lst->visivel == 0)
        {
            printf("2.1\n");
            if(ant == NULL) //Nó inicial
            {
                printf("2.2\n");
                temp = lst;
                if(lst->prox == NULL)
                    inimigos = NULL;
                else
                    inimigos = lst->prox;
                lst = lst->prox;
                free(temp);
                continue;
            }
            else //ant != NullNó não inicial
            {
                printf("2.3\n");
                if(lst->prox == NULL) //ultimo
                {
                    ant->prox = NULL;
                    free(lst);
                    lst = NULL;
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
        if(lst->tipo == 2 || lst->tipo == 1) //rotacionar
        {
            lst->angulo = (lst->angulo+5) % 360;

            lst->rect.x += lst->dx;
            lst->rect.y += lst->dy;
            //printf("X1: %d\t Y1: %d\t W1: %d\t H1: %d\n", lst->srect.x, lst->srect.y, lst->srect.w, lst->srect.h);
            SDL_RenderCopyEx(renderer, lst->textura, NULL, &lst->rect, lst->angulo, NULL, SDL_FLIP_NONE);
        }
        else//(lst->tipo == 0) - Espelhar
        {
            lst->rect.x += lst->dx;
            lst->rect.y += lst->dy;

            if(lst->cooldownFlip > 0 && lst->cooldownFlip <= 20)
                SDL_RenderCopyEx(renderer, lst->textura, NULL, &lst->rect, lst->angulo, NULL, SDL_FLIP_NONE);
            else if(lst->cooldownFlip > 20 && lst->cooldownFlip <= 40)
                SDL_RenderCopyEx(renderer, lst->textura, NULL, &lst->rect, lst->angulo, NULL, SDL_FLIP_HORIZONTAL); 

            if(lst->cooldownFlip > 0)
                lst->cooldownFlip--;
            else
                lst->cooldownFlip = 40;

        }
    }
    
    return inimigos;
}

void colisao(Jogador* jogador,InfoInimigo* infoInimigo, Inimigo* inimigos, Tiro* tiros, Mix_Chunk* somExplosion)
{
    Inimigo* lstI;
    Tiro* lstT;
    int colidiu = 1;
    int i;

    //Só verifica se ambos não forem NULL
    if(inimigos != NULL && tiros != NULL)
    {
        //Colisão Inimigo x Tiro
        for(lstI = inimigos; lstI != NULL; lstI = lstI->prox)
        {
            for(lstT = tiros; lstT != NULL; lstT = lstT->prox)
            {
                //Se o tiro está à esquerda
                if(lstT->rect.x + lstT->rect.w < lstI->rect.x)
                {
                    colidiu = 0;
                }
                //Se o tiro está à direita
                else if(lstT->rect.x > lstI->rect.x + lstI->rect.w)
                {
                    colidiu = 0;
                }
                //Se o tiro está à cima
                else if(lstT->rect.y + lstT->rect.h < lstI->rect.y)
                {
                    colidiu = 0;
                }
                //Se o tiro está à baixo
                else if(lstT->rect.y > lstI->rect.y + lstI->rect.h)
                {
                    colidiu = 0;
                }

                //Se eles colidiram, então ...
                if(colidiu == 1)
                {
                    lstT->visivel = 0; //O tiro some
                    lstI->visivel = 0; //O inimigo some
                    infoInimigo->numAtualInimigos--;//Diminui a quantidade de inimigos em -1
                    Mix_PlayChannel(-1, somExplosion, 0);
                    //O jogador recebe a pontuação de acordo com o tipo de inimigo atingido.
                    jogador->pontuacao += lstI->pontos;

                    //Spawnar outros menores
                    if(lstI->tipo != 0)
                    {
                        for(i = 0; i < infoInimigo->numeroSpawn; i++)
                        {
                            spawnInimigos(infoInimigo, inimigos, lstI->tipo-1);
                        }
                    }      
                }

                colidiu = 1;
            }
        }
    }

    //Só verifica se inimigos não for NULL
    if(inimigos != NULL)
    {
        //Colisão Inimigo x Nave
        for(lstI = inimigos; lstI != NULL; lstI = lstI->prox)
        {
            //Se a Nave está à esquerda
            if(jogador->rect.x + jogador->rect.w < lstI->rect.x)
            {
                colidiu = 0;
            }
            //Se a Nave está à direita
            else if(jogador->rect.x > lstI->rect.x + lstI->rect.w)
            {
                colidiu = 0;
            }
            //Se a Nave está à cima
            else if(jogador->rect.y + jogador->rect.h < lstI->rect.y)
            {
                colidiu = 0;
            }
            //Se a Nave está à baixo
            else if(jogador->rect.y > lstI->rect.y + lstI->rect.h)
            {
                colidiu = 0;
            }

            //Se eles colidiram, então ...
            if(colidiu == 1)
            {
                lstI->visivel = 0; //O inimigo some
                infoInimigo->numAtualInimigos--;//Diminui a quantidade de inimigos em -1
                Mix_PlayChannel(-1, somExplosion, 0);
                //O jogador recebe a pontuação de acordo com o tipo de inimigo atingido.
                jogador->pontuacao += lstI->pontos;
                //O jogador perde uma vida
                jogador->vida--;
                //O jogador é teleportado para o meio do jogo, ou um tempo de respawn
                SDL_Rect aux = {(largura - jogador->rect.w)/2, (altura - jogador->rect.h)/2, jogador->rect.w, jogador->rect.h};
                jogador->rect = aux;

                if(lstI->tipo != 0)
                {
                    for(i = 0; i < infoInimigo->numeroSpawn; i++)
                    {
                        spawnInimigos(infoInimigo, inimigos, lstI->tipo -1);
                    }
                }
            }

            colidiu = 1;
        }
    }
}

int escrever(SDL_Renderer* renderer, Jogador *jogador, TTF_Font *fontePixel, SDL_Color branco, Objeto *textoGameOver, Objeto *textoNome, Objeto *textoJogarNovamente, Objeto *textoYes, Objeto *textoNo, int* jogando)
{
    SDL_Event evento;
    SDL_Surface* surface;
    int escrevendo = 1;

    Objeto* textoJogador = (Objeto*) malloc(sizeof(Objeto));
    surface = TTF_RenderText_Solid(fontePixel, "-", branco);
    textoJogador->textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect aux = {180, 180, 300, surface->h};
    textoJogador->rect = aux;

    while(escrevendo)
    {
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderCopy(renderer, textoGameOver->textura, NULL, &textoGameOver->rect);
        SDL_RenderCopy(renderer, textoNome->textura, NULL, &textoNome->rect);
        SDL_RenderCopy(renderer, textoJogarNovamente->textura, NULL, &textoJogarNovamente->rect);
        SDL_RenderCopy(renderer, textoYes->textura, NULL, &textoYes->rect);
        SDL_RenderCopy(renderer, textoNo->textura, NULL, &textoNo->rect);

        while(SDL_PollEvent(&evento) != 0)
        {
            if(evento.type == SDL_QUIT)
            {
                escrevendo = 0;
                *jogando = 0;
            }
            if(evento.type == SDL_KEYUP)
            {
                switch(evento.key.keysym.sym)
                {
                    case SDLK_RETURN:
                    {
                        if(jogador->empty > 0)
                            escrevendo = 0;
                        break;
                    }
                    case SDLK_KP_ENTER:
                    {
                        if(jogador->empty > 0)
                            escrevendo = 0;
                        break;
                    }
                    case SDLK_BACKSPACE:
                    {
                        if(jogador->empty > 0)
                        {
                            jogador->empty--;
                            jogador->nome[jogador->empty] = ' ';
                            // textoJogador->rect.w -= 28;
                        }
                        break;
                    }
                    case SDLK_a:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'a';
	                        jogador->empty++;
                    	}
                        // textoJogador->rect.w += 2
	                    break;
	                }
                    case SDLK_b:
                    {
                    	if(jogador->empty < 10)
                    	{
                        jogador->nome[jogador->empty] = 'b';
                        jogador->empty++;
                    	}
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_c:
                    {
                    	if(jogador->empty < 10)
                    	{
                        jogador->nome[jogador->empty] = 'c';
                        jogador->empty++;
                    	}
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_d:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'd';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_e:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'e';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_f:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'f';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_g:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'g';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_h:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'h';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_i:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'i';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_j:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'j';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_k:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'k';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_l:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'l';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_m:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'm';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_n:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'n';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_o:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'o';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_p:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'p';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_q:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'q';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_r:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'r';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_s:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 's';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_t:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 't';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_u:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'u';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_v:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'v';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_w:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'w';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_x:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'x';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_y:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'y';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                    case SDLK_z:
                    {
                    	if(jogador->empty < 10)
                    	{
	                        jogador->nome[jogador->empty] = 'z';
	                        jogador->empty++;
	                    }
                        // textoJogador->rect.w += 28;
                        break;
                    }
                }
            }
            
            //SDL_Delay(1000/30);
        }

        printf("Nome: %s\n", jogador->nome);

        surface = TTF_RenderText_Solid(fontePixel, jogador->nome, branco);
        textoJogador->textura = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, textoJogador->textura, NULL, &textoJogador->rect);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000/30);
    }

    //Atualizar o textoNome
    char nome[] = "Nome:";
    strcat(nome, jogador->nome); 

    surface = TTF_RenderText_Solid(fontePixel, nome, branco);
    textoNome->textura = SDL_CreateTextureFromSurface(renderer, surface);
    textoNome->rect.w += textoJogador->rect.w;

    return 1; //opcaoFimDeJogo == 1
}

void salvarDadosJogador(InfoRank* infoRank, Rank* ranking, Jogador* jogador, FILE* arqRecorde)
{
    int i = 0;
    rewind(arqRecorde);

    printf("Salvando ...\n");

    //Quantidade de ranqueados
    while(0 < fread(&ranking[i].nome, sizeof(char), 10, arqRecorde)) //enquanto não chegar no final do arquivo
    {
        fread(&ranking[i].pontuacao, sizeof(int), 1, arqRecorde);
        i++;
    }

    infoRank->numRanqueados = i;

    rewind(arqRecorde);

    //Se tiver posição vazia, então adicionamos, ordenamos o vetor e salvamos.
    if(infoRank->numRanqueados < infoRank->numRanks)
    {
        strcpy(ranking[i].nome, jogador->nome);
        ranking[i].pontuacao = jogador->pontuacao;
        //Ordenar o vetor
        qsort(ranking, infoRank->numRanqueados+1, sizeof(Rank), compararRanks);

        for(i = 0; i <= infoRank->numRanqueados; i++)
        {
            fwrite(&ranking[i].nome, sizeof(char), 10, arqRecorde);
            fwrite(&ranking[i].pontuacao, sizeof(int), 1, arqRecorde);
        }

    }
    //Senão tiver posição vazia, então verificamos a pontuação do menor com a do jogador, se for menor
    //alteramos os dados, ordenamos e salvamos
    else
    {
        if(ranking[infoRank->numRanks-1].pontuacao < jogador->pontuacao)
        {
            strcpy(ranking[i-1].nome, jogador->nome);
            ranking[i-1].pontuacao = jogador->pontuacao;
            //Ordenar o vetor
            qsort(ranking, infoRank->numRanqueados, sizeof(Rank), compararRanks);

            for(i = 0; i < infoRank->numRanqueados; i++)
            {
                fwrite(&ranking[i].nome, sizeof(char), 10, arqRecorde);
                fwrite(&ranking[i].pontuacao, sizeof(int), 1, arqRecorde);
            }
        }
    }
}

int compararRanks(const void* a, const void* b)
{
    if(((Rank*)a)->pontuacao < ((Rank*)b)->pontuacao)
        return 1;
    else if(((Rank*)a)->pontuacao == ((Rank*)b)->pontuacao)
        return 0;
    else
        return -1;
}

void mostrarRanking(SDL_Renderer* renderer, Rank* ranking, InfoRank* infoRank, Objeto* textoRank, TTF_Font *fonteRecorde, SDL_Color branco, FILE* arqRecorde)
{
    SDL_Surface* surface;
    int i = 0, espaco;
    char final[40];

    rewind(arqRecorde);

    //Quantidade de ranqueados
    while(0 < fread(&ranking[i].nome, sizeof(char), 10, arqRecorde)) //enquanto não chegar no final do arquivo
    {
        fread(&ranking[i].pontuacao, sizeof(int), 1, arqRecorde);
        printf("Nome: %s\tPontuacao: %d\n", ranking[i].nome, ranking[i].pontuacao);
        i++;
    }

    infoRank->numRanqueados = i;

    printf("Numero de Rankeados: %d\n", infoRank->numRanqueados);

    // Juntar o nome com o numero em um char só.
    for(i = 0, espaco = 0; i < infoRank->numRanqueados; i++, espaco += 50)
    {
    	verificaNomes(ranking, infoRank);

        sprintf(final, "%s:%d", ranking[i].nome, ranking[i].pontuacao);

        printf("Rankeado[%d]: %s\n", i+1, final);

        surface = TTF_RenderText_Solid(fonteRecorde, final, branco);
        textoRank->textura = SDL_CreateTextureFromSurface(renderer, surface);
        textoRank->rect.y = espaco;
        textoRank->rect.w = strlen(final)*28;
        SDL_RenderCopy(renderer, textoRank->textura, NULL, &textoRank->rect);
        SDL_Delay(1000/30);
    }

    textoRank->rect.y = 100;
}

void verificaNomes(Rank* ranking, InfoRank* infoRank)
{
	int i, j, k;
	char nomeOk[11];

	for(i = 0; i < infoRank->numRanqueados; i++)
	{
		for(k = 0; k < 10; k++)
		{
			nomeOk[k] = '-';
		}
		nomeOk[k] = '\0';

		for(j = 0; ranking[i].nome[j] != '\0'; j++)
		{
			switch(ranking[i].nome[j])
			{
				case 'a':
				{
					nomeOk[j] = 'a';
					break;
				}
				case 'b':
				{
					nomeOk[j] = 'b';
					break;
				}
				case 'c':
				{
					nomeOk[j] = 'c';
					break;
				}
				case 'd':
				{
					nomeOk[j] = 'd';
					break;
				}
				case 'e':
				{
					nomeOk[j] = 'e';
					break;
				}
				case 'f':
				{
					nomeOk[j] = 'f';
					break;
				}
				case 'g':
				{
					nomeOk[j] = 'g';
					break;
				}
				case 'h':
				{
					nomeOk[j] = 'h';
					break;
				}
				case 'i':
				{
					nomeOk[j] = 'i';
					break;
				}
				case 'j':
				{
					nomeOk[j] = 'j';
					break;
				}
				case 'k':
				{
					nomeOk[j] = 'k';
					break;
				}
				case 'l':
				{
					nomeOk[j] = 'l';
					break;
				}
				case 'm':
				{
					nomeOk[j] = 'm';
					break;
				}
				case 'n':
				{
					nomeOk[j] = 'n';
					break;
				}
				case 'o':
				{
					nomeOk[j] = 'o';
					break;
				}
				case 'p':
				{
					nomeOk[j] = 'p';
					break;
				}
				case 'q':
				{
					nomeOk[j] = 'q';
					break;
				}
				case 'r':
				{
					nomeOk[j] = 'r';
					break;
				}
				case 's':
				{
					nomeOk[j] = 's';
					break;
				}
				case 't':
				{
					nomeOk[j] = 't';
					break;
				}
				case 'u':
				{
					nomeOk[j] = 'u';
					break;
				}
				case 'v':
				{
					nomeOk[j] = 'v';
					break;
				}
				case 'w':
				{
					nomeOk[j] = 'w';
					break;
				}
				case 'x':
				{
					nomeOk[j] = 'x';
					break;
				}
				case 'y':
				{
					nomeOk[j] = 'y';
					break;
				}
				case 'z':
				{
					nomeOk[j] = 'z';
					break;
				}
			}
		}

		strcpy(ranking[i].nome, nomeOk);
	}
}
// int i, posicoes = 10, espaco = 5;
//     char nome[50];
//     char numero[10];
//     // Juntar o nome com o numero em um char só.
//     for(i = 0; i < posicoes; i++)
//     {

//     }

// void ordenarRank(Jogador** jogadores, Jogador* jogador)
// {
//     int i;
//     int posicoes = 10;
//     int maior = 0;

//     for(i = 0; i < posicoes; i++)
//     {

//     }
    
//     qsort(jogadores, posicoes,sizeof(Jogador*), compararJogadores);
// }

// void lerRank(InfoRank* infoRank)
// {

// }

//Inimigo teleporta ao atingir as paredes
// int x1, x2, y1, y2;

// x1 = lst->drect.x;
// x2 = lst->drect.x + ldt->drect.w;
// y1 = lst->drect.y;
// y2 = lst->drect.y + ldt->drect.h;

// if(x1 < 0)
// {
//     lst->drect.x = largura-lst->drect.w;
// }
// if(x2 > largura)
// {
//     lst->drect.x = 0;
// }
// if(y1 < 0)
// {
//     lst->drect.y = altura-lst->drect.h;
// }
// if(y2 > altura)
// {
//     lst->drect.y = 0;
// }

//SDL_Rect aux1 = {200,200,tamanhoX,tamanhoY};
//SDL_Rect aux2 = {200,200,tamanhoX,tamanhoY};
//SDL_Rect aux3 = {200,200,tamanhoX,tamanhoY};
//SDL_Rect aux4 = {200,200,tamanhoX,tamanhoY};

//SDL_StartTextInput();
    //SDL_StopTextInput();

// switch(evento.key.keysym.sym)
// {
//     case SDLK_RETURN:
//     {
//         if(jogador->empty > 0)
//             escrevendo = 0;
//         break;
//     }
//     case SDLK_KP_ENTER:
//     {
//         if(jogador->empty > 0)
//             escrevendo = 0;
//         break;
//     }
//     case SDLK_BACKSPACE:
//     {
//         if(jogador->empty > 0)
//         {
//             jogador->empty--;
//             jogador->nome[jogador->empty] = ' ';
//             textoJogador->rect.w -= 28;
//         }
//         break;
//     }
//     case SDLK_a:
//     {
//         jogador->nome[jogador->empty] = 'a';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_b:
//     {
//         jogador->nome[jogador->empty] = 'b';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_c:
//     {
//         jogador->nome[jogador->empty] = 'c';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_d:
//     {
//         jogador->nome[jogador->empty] = 'd';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_e:
//     {
//         jogador->nome[jogador->empty] = 'e';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_f:
//     {
//         jogador->nome[jogador->empty] = 'f';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_g:
//     {
//         jogador->nome[jogador->empty] = 'g';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_h:
//     {
//         jogador->nome[jogador->empty] = 'h';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_i:
//     {
//         jogador->nome[jogador->empty] = 'i';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_j:
//     {
//         jogador->nome[jogador->empty] = 'j';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_k:
//     {
//         jogador->nome[jogador->empty] = 'k';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_l:
//     {
//         jogador->nome[jogador->empty] = 'l';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_m:
//     {
//         jogador->nome[jogador->empty] = 'm';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_n:
//     {
//         jogador->nome[jogador->empty] = 'n';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_o:
//     {
//         jogador->nome[jogador->empty] = 'o';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_p:
//     {
//         jogador->nome[jogador->empty] = 'p';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_q:
//     {
//         jogador->nome[jogador->empty] = 'q';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_r:
//     {
//         jogador->nome[jogador->empty] = 'r';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_s:
//     {
//         jogador->nome[jogador->empty] = 's';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_t:
//     {
//         jogador->nome[jogador->empty] = 't';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_u:
//     {
//         jogador->nome[jogador->empty] = 'u';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_v:
//     {
//         jogador->nome[jogador->empty] = 'v';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_w:
//     {
//         jogador->nome[jogador->empty] = 'w';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_x:
//     {
//         jogador->nome[jogador->empty] = 'x';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_y:
//     {
//         jogador->nome[jogador->empty] = 'y';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
//     case SDLK_z:
//     {
//         jogador->nome[jogador->empty] = 'z';
//         jogador->empty++;
//         textoJogador->rect.w += 28;
//         break;
//     }
// }

// else if(evento.type == SDL_TEXTINPUT && evento.key.keysym.sym != SDLK_RIGHT && evento.key.keysym.sym != SDLK_UP && evento.key.keysym.sym != SDLK_DOWN && evento.key.keysym.sym != SDLK_LEFT)
// {
//     jogador->nome[jogador->empty] = *evento.text.text;
//     //strcat(jogador->nome, evento.text.text);
//     jogador->empty++;
//     textoJogador->rect.w += 28;
// }
// else if(evento.key.keysym.sym == SDLK_BACKSPACE && jogador->empty > 0)
// {
//     jogador->empty--;
//     jogador->nome[jogador->empty] = ' ';
//     textoJogador->rect.w -= 28;
// }
// if((evento.key.keysym.sym == SDLK_RETURN || evento.key.keysym.sym == SDLK_KP_ENTER) && jogador->empty > 0)
// {
//     escrevendo = 0;
// }

// int i;

    //Limpar os ' ' do nome do jogador.
    // for(i = 0; jogador->nome[i] != '\0'; i++)
    // {
    //     if(jogador->nome[i] == ' ')
    //         break;
    // }

    // printf("Tinha %d caracteres\n", strlen(jogador->nome));

    // char string[30];
    // strncat(string, jogador->nome, i-1);

    // free(jogador->nome);
    // strcpy(jogador->nome, string);

    // printf("Ficou com %d caracteres\n", strlen(jogador->nome));