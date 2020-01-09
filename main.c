#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char *argv[])
{
	//VARIAVEIS
	int largura = 640, altura = 480, jogando = 1;

	//INICIALIZADORES DO SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Event evento;
    SDL_Window *janela = SDL_CreateWindow("jogo",50,50, largura, altura,SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(janela,-1,0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

    SDL_Surface *surface = IMG_Load("imagens/recorde.jpg");
	SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, surface);

	Mix_Music *musica = Mix_LoadMUS("musicas/menu.ogg");
	Mix_PlayMusic(musica, -1);

	while(jogando == 1)
    {
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, textura, NULL, NULL);

        while(SDL_PollEvent(&evento) != 0)
        {
            if(evento.type == SDL_QUIT)
                jogando = 0;

            if(evento.type == SDL_KEYDOWN)
            {
            	jogando = 0;
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(1000/30);
    }

	return 0;
}