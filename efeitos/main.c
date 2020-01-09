#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define FPS 60

typedef struct jogador
{
	char nome[30];

}Jogador;

typedef struct inimigo
{


}Inimigo;

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Erro: Nao inicializou!");
		return -1;
	}

	Mix_Init(MIX_INIT_MP3);

	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
    {
    	printf("Erro: Nao inicializou o SDL Mixer!");
        return -1;    
    }

    Mix_Music *musica = Mix_LoadMUS("./musicas/Menu.mp3");

							/*----------VARIÁVEIS----------*/
	int width = 640, height = 480;
	int jogando = 1;
	SDL_DisplayMode DM;
	SDL_Surface *surface;
	SDL_Texture *textura;
	SDL_Event evento;
	int menu = 0;

					/*----------PEGAR AS DIMENSÕES DA TELA----------*/
	if(SDL_GetDesktopDisplayMode(0,&DM) != 0)
	{
		printf("Erro: Nao foi possivel pegar as dimensoes da tela");
	}

					/*----------CRIAR JANELA----------*/

	SDL_Window *janela = SDL_CreateWindow("Air Battle", (DM.w-width)/2, (DM.h-height)/2, width, height, SDL_WINDOW_RESIZABLE);
	if(janela == NULL)
	{
		printf("Erro: Nao foi possivel criar a janela");
		return 1;
	}
					/*----------CRIAR RENDERER----------*/

	SDL_Renderer *renderer = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(renderer == NULL)
	{
		printf("Erro: Nao foi possivel criar o renderer");
		return 1;
	}

					/*----------CRIAR TEXTURA----------*/
	
	surface = IMG_Load("images/recorde.png");
	textura = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect sRect = {0, 0, 137, 183};
	SDL_Rect dRect = {width/2, height/2, 32, 48};

	Mix_PlayMusic(musica, -1);	

					/*----------LOOP JOGO----------*/
	while(jogando)
	{
		SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, textura, NULL, NULL);

        while(SDL_PollEvent(&evento) != 0)
        {
            if(evento.type == SDL_QUIT)
                return 0;

            if(evento.type == SDL_KEYDOWN)
            {
                jogando = 1;
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(1000/30);
		// unsigned comeco = SDL_GetTicks();

		// //SDL_SetRenderDrawColor(renderer, 111, 133, 255, 255);
		// SDL_RenderClear(renderer);

		// while(SDL_PollEvent(&evento))
		// {			
		// 	if(evento.type == SDL_QUIT)
		// 		jogando = 0; //Encerra o Game Loop
		// 	if(evento.type == SDL_KEYDOWN)
		// 	{
		// 		// Se a tecla for ESC ...
		// 		if(evento.key.keysym.sym == SDLK_ESCAPE)
		// 		{
		// 			jogando = 0; //Encerre o Game Loop
		// 		}
		// 	}
		// }

		// SDL_RenderCopy(renderer, textura, &sRect, &dRect);
		// SDL_RenderPresent(renderer);

		// //Regula o FPS do jogo.
		// unsigned fim = SDL_GetTicks();
		// unsigned delta = fim - comeco;
		// if(delta < 1000/FPS)
		// 	SDL_Delay(1000/60 - delta);
	}

    SDL_FreeSurface(surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(janela);
	Mix_Quit();
    SDL_Quit();
 
	return 0;
}