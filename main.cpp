#include <algorithm>
#include <iostream>
#include <random>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

void startSDL() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "Error while initializing SDL: " << SDL_GetError();
	if(TTF_Init() == -1)
		std::cout << "Error while initializing SDL_ttf: " << TTF_GetError();
}

void setPixelColor(SDL_Surface* surface, int X, int Y, Uint32 pixel) {
	Uint32* pixels = (Uint32*)surface->pixels;
	pixels[(Y * surface->w) + X] = pixel;
}

void writeText(SDL_Renderer* renderer, int x, int y, const std::string& text, TTF_Font* font) {
	int w, h;
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	TTF_SizeText(font, text.c_str(), &w, &h);
	SDL_Rect dRect{ x, y, w, h };
	SDL_RenderCopy(renderer, textTexture, NULL, &dRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

class GrattaEVinci {
  int montepremi[12] = {1, 5, 10, 50, 100, 1000, 5000, 10000, 100000, 500000, 1000000, 2000000};
  int vincenti[5];
  int jolly;
  int numeri[15][2];
  bool scoperti[21];
  SDL_Window* window;
  SDL_Renderer* renderer;
  TTF_Font* font;
  SDL_Texture* texture;
public:
  GrattaEVinci();
  void gratta(int x, int y, int r);
  int vincita();
  void render();
  bool controlla(int X, int Y, int d, int d2);
  bool vittoria();
};

GrattaEVinci::GrattaEVinci() {
	window = SDL_CreateWindow("Gratta e Vinci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 200, 800, SDL_WINDOW_SHOWN);
	if(window == NULL ) {
		std::cout << "Error while creating SDL_Window: " << SDL_GetError();
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer == NULL) {
		std::cout << "Error while creating SDL_Renderer: " << SDL_GetError();
	}
	font = TTF_OpenFont("../res/OpenSans-Regular.ttf", 20);
	if(font == NULL) {
		std::cout << "Error while loading TTF_Font: " << TTF_GetError();
	}
	texture = IMG_LoadTexture(renderer, " ");

  srand(time(NULL));
  for(int i = 0; i < 5; i++) {
    vincenti[i] = rand()%100;
  }
	jolly = rand()%100;
  for(int i = 0; i < 15; i++) {
    numeri[i][0] = rand()%100;
    numeri[i][1] = montepremi[rand()%12];
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
  for(int i = 0; i < 360; i++) {
    for(int j = 0; j < r; j++) {
      setPixelColor(SDL_GetWindowSurface(window), cos(i*M_PI/180)*r+x, sin(i*M_PI/180)*r+y, 0);
    }
  }
}

void GrattaEVinci::render() {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	for(;;) { //scrive il testo

	}
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool GrattaEVinci::controlla(int X, int Y, int d, int d2){
        for(int x=0;x<d;x+=5){
             for(int y=0;y<d2;y+=5){
                if(GetPixelColor(surface,X+x,Y+y)!=0){
                   return false;
                }
             }
        }
        return true;
}

bool GrattaEVinci::vittoria(){
  for(int i=0;i<6;i++){
    if(!scoperti[i]){
        scoperti[i]=controlla(50+i*(115), 880,90,90);
        if(!scoperti[i])return false;
    }
  }

    for(int i=0;i<5;i++){
       for(int l=0;l<3;l++){
         if(!scoperti[5+i*3+l]){
            scoperti[5+i*3+l]=controlla(70+i*(140), 1070+135*l,90,130);
            if(!scoperti[5+i*3+l])return false;
	 }
       }
    }
  return true;
}

int main() {
	startSDL();
}
