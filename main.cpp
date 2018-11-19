#include <iostream>
#include <random>
#include <ctime>

void startSDL() {
//...
}

void SetPixelColor(SDL_Surface* pSurface, int X, int Y, Uint32 Pixel)
{
	Uint32* pPixels = (Uint32*)pSurface->pixels;
	pPixels[(Y * pSurface->w) + X] = Pixel;
}

class GrattaEVinci {
    int montepremi[12] = {1, 5, 10, 50, 100, 1000, 5000, 10000, 100000, 500000, 1000000, 2000000};
    int vincenti[5];
    int jolly;
    int numeri[15][2];
    SDL_Surface* surface;
    void render();
public:
    GrattaEVinci();
    void gratta(int x, int y, int r);
    int vincita();
    void render();
};

GrattaEVinci::GrattaEVinci() {
    srand(time(NULL));
    for(int i = 0; i < 5) {
        vincenti[i] = rand()%100;
    }
    for(int i = 0; i < 15) {
        numeri[i][0] = rand()%100;
        numeri[i][1] = montempremi[rand()%12];
    }
    render();
}

int GrattaEVinci::vincita() {
    int vincita = 0;
    for(int i = 0; i < 15; i++) {
        if(find(vincenti, vincenti+5, numeri[i][0]) != vincenti+5) {
            vincita += numeri[i][1];
        }
    }
    return vincita;
}

void GrattaEVinci::gratta(int x, int y, int r) {
    for(int i = 0; i < 360; i++) {
        for(int j = 0; j < r; j++) {
            SetPixelColor(surface, cos(i*PI/180)*r+x, sin(i*PI/180)*r+y, 0);
        }
    }
}

int main() {

}






