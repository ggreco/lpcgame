#ifndef OBJ_H
#define OBJ_H

#include "video.h"

class Object
{
protected:    
    SDL_Surface *bitmap_;
    static Video *video_;
    int x_, y_;
public:
    Object() : bitmap_(NULL) {}

    Object(const std::string &name);

    static void Set(Video &v) { video_ = &v; }
    void position(int x, int y) { x_ = x; y_ = y; }
    std::pair<int, int> position() const { return std::make_pair(x_, y_); }

    void lowblit(SDL_Rect &rect, int mapx, int mapy); 

    virtual void blit(int mapx, int mapy) {
        if (video_ && bitmap_) {
            SDL_Rect rect = {0, 0, bitmap_->w, bitmap_->h};

            lowblit(rect, mapx, mapy);
        }
    }
    ~Object() {
        if (bitmap_)
            SDL_FreeSurface(bitmap_);
    }
};

#endif
