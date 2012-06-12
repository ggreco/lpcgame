#include "char.h"
#include "SDL_image.h"

Character::
Character(const std::string &base, const std::string &cloth, const std::string &hair)
{
    SDL_Surface *b = IMG_Load(base.c_str()), *c = NULL, *h = NULL;

    if (!b) 
        throw std::string("Unable to load character with skin " + base);

    if (!cloth.empty()) {
        c = IMG_Load(cloth.c_str());    


        if (c && (c->w != b->w ||
                  c->h != b->h))
            throw std::string("Invalid cloth " + cloth + " for character base " + base);
    }
    if (!hair.empty()) {
        h = IMG_Load(hair.c_str());
        if (h && (h->w != b->w ||
                  h->h != b->h))
            throw std::string("Invalid hair " + hair + " for character base " + base);
    }

    if (c) {
        SDL_BlitSurface(c, NULL, b, NULL);
        SDL_FreeSurface(c);
    } 
    if (h) {
        SDL_BlitSurface(h, NULL, b, NULL);
        SDL_FreeSurface(h);
    }
    bitmap_ =  SDL_DisplayFormatAlpha(b);
    SDL_FreeSurface(b);
}

