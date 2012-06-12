#ifndef CHARACTER_H
#define CHARACTER_H

#include "video.h"
#include "obj.h"

class Character : public Object
{
public:
    Character(const std::string &base, const std::string &cloth = "", const std::string &hair = ""); 

    void blit(int x, int y, int direction) {
        SDL_Rect src = { 0, direction * 64, 64, 64};
        lowblit(src, x, y);
    }
};

#endif
