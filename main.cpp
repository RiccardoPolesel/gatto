#include <algorithm>
#include <iostream>
#include <random>
#include <ctime>
#if defined(_WIN32)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#endif

void startSDL() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "Error while initializing SDL: " << SDL_GetError();
	if(TTF_Init() == -1)
		std::cout << "Error while initializing SDL_ttf: " << TTF_GetError();
	IMG_Init(IMG_INIT_PNG);
}

Uint32 getPixelColor(SDL_Texture* texture, int X, int Y) {
	int pitch, w, h;
	void *pixels;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	Uint32 *upixels = (Uint32*) pixels;
	return upixels[(Y * w) + X];
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
}

class GrattaEVinci {
  int montepremi[12] = {1, 5, 10, 50, 100, 1000, 5000, 10000, 100000, 500000, 1000000, 2000000};
  int vincenti[5];
  int jolly;
  int numeri[15][2];
	bool scoperti[21];
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	TTF_Font* font;

	bool controlla(int x, int y, int d, int d2);
public:
  GrattaEVinci();
  void gratta(int x, int y, int r);
  int vincita();
  void render();
	bool finito();
};

GrattaEVinci::GrattaEVinci()
: window(NULL), font(NULL) {
	window = SDL_CreateWindow("Gratta e Vinci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 800, SDL_WINDOW_SHOWN);
	if(window == NULL ) {
		std::cout << "Error while creating SDL_Window: " << SDL_GetError();
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL) {
		std::cout << "Error while creating SDL_Renderer: " << SDL_GetError();
		return;
	}
	font = TTF_OpenFont("OpenSans-Regular.ttf", 12);
	if(font == NULL) {
		std::cout << "Error while loading TTF_Font: " << TTF_GetError();
		return;
	}
	SDL_Surface* surface = SDL_LoadBMP("texture.bmp");
	if(surface == NULL) {
		std::cout << "Error while loading texture: " << SDL_GetError();
		return;
	}
	SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( surface, SDL_PIXELFORMAT_RGBA8888, 0 );
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);

	int pitch;
	void* pixels;
	SDL_LockTexture( texture, NULL, &pixels, &pitch );
	memcpy( pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );
	SDL_UnlockTexture( texture );
	pixels = NULL;
	SDL_FreeSurface(surface);
	SDL_FreeSurface(formattedSurface);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  srand(time(NULL));
  for(int i = 0; i < 5; i++) {
    vincenti[i] = rand()%100;
  }
	jolly = rand()%100;
  for(int i = 0; i < 15; i++) {
    numeri[i][0] = rand()%100;
    numeri[i][1] = montepremi[rand()%12];
  }
	for(int i = 0; i < 21; i++) {
		scoperti[i] = false;
	}
  render();
}

int GrattaEVinci::vincita() {
  int vincita = 0;
  for(int i = 0; i < 15; i++) {
    if(std::find(vincenti, vincenti+5, numeri[i][0]) != vincenti+5) {
      vincita += numeri[i][1];
    }
  }
  return vincita;
}

void GrattaEVinci::gratta(int x, int y, int r) {
	int pitch, w, h;
	void *pixels;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	Uint32 *upixels = (Uint32*) pixels;
	SDL_PixelFormat* mappingFormat = SDL_AllocFormat( SDL_PIXELFORMAT_RGBA8888 );
	Uint32 transparent = SDL_MapRGBA(mappingFormat, 0x00, 0x00, 0x00, 0x00);

	for(int i = 0; i < r; i++) {
		for(int xp = 0; xp <= i; xp++) {
			int yp = sqrt(pow(i, 2) - pow(xp, 2));
			upixels[(yp+y)*(pitch / sizeof(unsigned int)) + xp + x] = transparent;
			upixels[(-yp+y)*(pitch / sizeof(unsigned int)) + xp + x] = transparent;
			upixels[(yp+y)*(pitch / sizeof(unsigned int)) - xp + x] = transparent;
			upixels[(-yp+y)*(pitch / sizeof(unsigned int)) - xp + x] = transparent;
		}
	}

	memcpy(pixels, upixels, pitch * h);
	SDL_UnlockTexture(texture);
}

void GrattaEVinci::render() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	for(int i = 0; i < 5; i++) {
		writeText(renderer, (80 + i*120) / 2, 900/2, std::to_string(vincenti[i]), font);
	}
	writeText(renderer, 680/2, 900/2, std::to_string(jolly), font);
	for(int i = 0; i < 5; i++) {
    for(int l = 0; l < 3; l++) {
      writeText(renderer, (100 + i*(140)) / 2, (1100 + 135*l) / 2, std::to_string(numeri[i*3+l][0]), font);
			writeText(renderer, (60 + i*(140)) / 2, (1160 + 135*l) / 2, std::to_string(numeri[i*3+l][1]) + "€", font);
  	}
  }

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool GrattaEVinci::controlla(int x, int y, int d, int d2) {
  for(int i = 0; i < d; i += 5) {
    for(int j = 0; j < d2; j += 5) {
      if(getPixelColor(texture,x+i,y+j) != 0) {
    		return false;
      }
  	}
  }
  return true;
}

bool GrattaEVinci::finito() {
  for(int i = 0; i < 6; i++) {
    if(!scoperti[i]) {
      scoperti[i] = controlla(50 + i*115, 880, 90, 90);
      if(!scoperti[i]) return false;
    }
  }
  for(int i = 0; i < 5; i++) {
    for(int l = 0; l < 3; l++) {
      if(!scoperti[5+i*3+l]) {
        scoperti[5+i*3+l] = controlla(70 + i*(140), 1070 + 135*l, 90, 130);
        if(!scoperti[5+i*3+l]) return false;
	 		}
  	}
  }
  return true;
}

int main() {
	startSDL();

	GrattaEVinci a;
	bool esci = false;
	SDL_Event u;
	while (!esci)
	{
		a.render();
		while (SDL_PollEvent(&u) != 0)
		{
			switch (u.type) {
			case SDL_QUIT:
				esci = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				a.gratta(u.button.x, u.button.y, 10);
				a.render();
			}
		}
	}
}
