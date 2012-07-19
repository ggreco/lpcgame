#ifndef OBJ_H
#define OBJ_H

#include "video.h"

class Object
{
protected:    
    static Video *video_;
    int x_, y_;
public:

    static void Set(Video &v) { video_ = &v; }
    int x() const { return x_; }
    int y() const { return y_; }
    int distance(int x, int y) const;
    int distance(Object *o) const { return distance(o->x(), o->y()); }
    void position(int x, int y) { x_ = x; y_ = y; }
    std::pair<int, int> position() const { return std::make_pair(x_, y_); }

    void lowblit(SDL_Surface *bitmap, SDL_Rect rect, int mapx, int mapy); 

    virtual void blit(int mapx, int mapy) = 0;
};

class StaticObject : public Object
{
    SDL_Surface *bitmap_;
public:    
    StaticObject() : bitmap_(NULL) {}

    StaticObject(const std::string &name);
    virtual ~StaticObject() {
        if (bitmap_)
            SDL_FreeSurface(bitmap_);
    }
    void blit(int mapx, int mapy) {
        if (video_ && bitmap_) {
            SDL_Rect rect = {0, 0, bitmap_->w, bitmap_->h};

            lowblit(bitmap_, rect, mapx, mapy);
        }
    }
};

#endif
