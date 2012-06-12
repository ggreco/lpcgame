#ifndef VIDEO_H
#define VIDEO_H

#include "SDL.h"
#include <string>

class Video
{
    SDL_Surface *screen_;
public:
    Video(int w, int h, bool fullscreen = false) {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))
            throw std::string("Unable to initialize SDL");

        if ((screen_ = SDL_SetVideoMode(640, 480, 0, fullscreen ? SDL_FULLSCREEN : 0)) == NULL)
            throw std::string("Unable to open SDL window");
    }
    int width() { return screen_->w; }
    int height() { return screen_->h; }
    SDL_PixelFormat *format() { return screen_->format; }
    void blit(SDL_Surface *b, SDL_Rect &src) {
        SDL_BlitSurface(b, &src, screen_, NULL);
    }
    void blit(SDL_Surface *b, SDL_Rect &src, int x, int y) {
        SDL_Rect dest = {x, y, 0, 0};
        SDL_BlitSurface(b, &src, screen_, &dest);
    }
    void flip() { SDL_Flip(screen_); }
};
#endif
