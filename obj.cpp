#include "obj.h"
#include "SDL_image.h"
#include <math.h>

Video *Object::video_ = NULL;

StaticObject::
StaticObject(const std::string &name) {
    SDL_Surface *s = IMG_Load(name.c_str());

    if (!s)
        throw std::string("Unable to create object from skin " + name);

    bitmap_ = SDL_DisplayFormatAlpha(s);
    SDL_FreeSurface(s);
}

int Object::
distance(int x, int y) const {
    return sqrt((x_ - x) * (x_ - x) + (y_ - y) * (y_ - y));
}

// low level blitting code used by every object
void Object::
lowblit(SDL_Surface *src, SDL_Rect rect, int mapx, int mapy) {
// do not render if not visible
    if (mapx < (x_ - rect.w - video_->width()) ||
        mapy < (y_ - rect.h - video_->height()) ||
        mapx > (x_ + rect.w) ||
        mapy > (y_ + rect.h))
        return;

    int posx = x_ - mapx, posy = y_ - mapy;

    // check if we render the whole object or only a piece of it
    if (posx < 0) {
        rect.x -= posx;
        rect.w += posx;
        posx = 0;
    }
    else if (posx + rect.w > video_->width())
        rect.w = video_->width() - posx;

    if (posy < 0) {
        rect.y -= posy;
        rect.h += posy;
        posy = 0;
    }
    else if (posy + rect.h > video_->height())
        rect.h = video_->height() - posy;

    // do the render
    video_->blit(src, rect, posx, posy);
}
