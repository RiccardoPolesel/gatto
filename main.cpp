#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <ctime>
#include <math.h>
#if defined(_WIN32)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#endif

using namespace std;

void startSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "Error while initializing SDL: " << SDL_GetError();
	if (TTF_Init() == -1)
		std::cout << "Error while initializing SDL_ttf: " << TTF_GetError();
	IMG_Init(IMG_INIT_PNG);
}


void writeText(SDL_Renderer* renderer, int x, int y, const std::string& text, TTF_Font* font) {
	int w, h;
	SDL_Color textColor = { 0x00, 0x00, 0x00 };
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	TTF_SizeText(font, text.c_str(), &w, &h);
	SDL_Rect dRect{ x, y, w, h };
	SDL_RenderCopy(renderer, textTexture, NULL, &dRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

class GrattaEVinci {
	int montepremi[12] = { 1, 5, 10, 50, 100, 1000, 5000, 10000, 100000, 500000, 1000000, 2000000 };
	int vincenti[5];
	int jolly;
	int numeri[15][2];
	bool scoperti[21];
	SDL_Window* window;
	SDL_Texture* texture;
	

	bool controlla(int x, int y, int d, int d2);
public:
	TTF_Font* font;
	SDL_Renderer* renderer;
	GrattaEVinci();
	void gratta(int x, int y, int r);
	int vincita();
	void render();
	bool finito();
};

GrattaEVinci::GrattaEVinci()
	: window(NULL), font(NULL) {
	window = SDL_CreateWindow("Gratta e Vinci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 800, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Error while creating SDL_Window: " << SDL_GetError();
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "Error while creating SDL_Renderer: " << SDL_GetError();
		return;
	}
	font = TTF_OpenFont("Sans.ttf", 15);
	if (font == NULL) {
		std::cout << "Error while loading TTF_Font: " << TTF_GetError();
		return;
	}
	SDL_Surface* surface = IMG_Load("texture.jpg");
	if (surface == NULL) {
		std::cout << "Error while loading texture: " << SDL_GetError();
		return;
	}
	

	

	SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);

	int pitch;
	void* pixels;
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
	SDL_UnlockTexture(texture);
	pixels = NULL;
	SDL_FreeSurface(surface);
	SDL_FreeSurface(formattedSurface);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	

	srand(time(NULL));
	for (int i = 0; i < 5; i++) {
		vincenti[i] = rand() % 100;
	}
	jolly = rand() % 100;
	for (int i = 0; i < 15; i++) {
		numeri[i][0] = rand() % 100;
		numeri[i][1] = montepremi[rand() % 12];
	}
	for (int i = 0; i < 21; i++) {
		scoperti[i] = false;
	}
	render();
}

int GrattaEVinci::vincita() {
	int vincita = 0;
	for (int i = 0; i < 15; i++) {
		if (std::find(vincenti, vincenti + 5, numeri[i][0]) != vincenti + 5) {
			vincita += numeri[i][1];
		}
	}
	return vincita;
}

void GrattaEVinci::gratta(int x, int y, int r) {
	int pitch=0, w, h;
	void *pixels;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
	Uint32 *upixels = (Uint32*)pixels;
	SDL_PixelFormat* mappingFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
	Uint32 transparent = SDL_MapRGBA(mappingFormat, 0x00, 0x00, 0x00, 0x00);

	for(int a=0;a<360;a+=2){
		for (int i = 0; i < r; i++) {
			int X = x + cos(a*3.14/180)*i;
			int Y = y + sin(a*3.14/180)*i;
			
			upixels[(Y)*pitch/sizeof(unsigned int) + X] = transparent;
		}
	}
	

	SDL_UnlockTexture(texture);
	SDL_FreeFormat(mappingFormat);
}

void GrattaEVinci::render() {
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	

	for (int i = 0; i < 5; i++) {
		writeText(renderer, (80 + i * 120) / 2, 900 / 2, std::to_string(vincenti[i]), font);
	}
	writeText(renderer, 680 / 2, 900 / 2, std::to_string(jolly), font);
	for (int i = 0; i < 5; i++) {
		for (int l = 0; l < 3; l++) {
			writeText(renderer, (100 + i*(140)) / 2, (1100 + 135 * l) / 2, std::to_string(numeri[i * 3 + l][0]), font);
			writeText(renderer, (60 + i*(140)) / 2, (1160 + 135 * l) / 2, std::to_string(numeri[i * 3 + l][1]) + "€", font);
		}
	}

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool GrattaEVinci::controlla(int x, int y, int d, int d2) {


	int pitch, w, h;
	void *pixels;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	Uint32 *upixels = (Uint32*)pixels;
	SDL_UnlockTexture(texture);
	for (int i = 0; i < d; i += 15) {
		for (int j = 0; j < d2; j += 15) {
			if (upixels[((y+j) * w) + x+1] != 0) {
				return false;
			}
		}
	}
	return true;
}

bool GrattaEVinci::finito() {
	for (int i = 0; i < 6; i++) {
		if (!scoperti[i]) {
			scoperti[i] = controlla(50 + i * 115, 880, 90, 90);
			if (!scoperti[i]) return false;
		}
	}
	for (int i = 0; i < 5; i++) {
		for (int l = 0; l < 3; l++) {
			if (!scoperti[5 + i * 3 + l]) {
				scoperti[5 + i * 3 + l] = controlla(70 + i*(140), 1070 + 135 * l, 90, 130);
				if (!scoperti[5 + i * 3 + l]) return false;
			}
		}
	}
	return true;
}

int main(int argc, char* args[]) {
	startSDL();
	bool button_down = false;
	GrattaEVinci a;
	bool esci = false;
	SDL_Event u;
	while (!esci)
	{
		a.render();
		if (button_down) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			a.gratta(x*2, y*2, 60);
		}


		while (SDL_PollEvent(&u) != 0)
		{
			switch (u.type) {
			case SDL_QUIT:
				esci = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				button_down = true;
				break;
			case SDL_MOUSEBUTTONUP:
				button_down = false;
				break;
			}
		}

		if (a.finito()) {
			esci = true;
		}
	}

	SDL_SetRenderDrawColor(a.renderer, 128, 128, 128, 0xFF);
	SDL_RenderClear(a.renderer);
	SDL_SetRenderDrawColor(a.renderer, 0x00, 0x00, 0x00, 0xFF);
	writeText(a.renderer, 10, 10, std::to_string(a.vincita()), a.font);
	SDL_RenderPresent(a.renderer);

	SDL_Delay(5000);
	return 0;
}
