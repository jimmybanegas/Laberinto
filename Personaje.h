#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

class Personaje
{
    public:
        int x,y,vida;
        bool activo;
        SDL_Surface*imagen;
        Personaje(int x,int y,SDL_Surface*personaje);
        void dibujar(SDL_Surface *screen);
        virtual ~Personaje();
    protected:
    private:
};

#endif // PERSONAJE_H
