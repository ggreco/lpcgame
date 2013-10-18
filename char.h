#ifndef CHARACTER_H
#define CHARACTER_H

#include "video.h"
#include "anim.h"

class Character : public AnimObj
{
public:
    int feet_x() const {
        return base_x();
    }
    int feet_y() const {
        return base_y() - 5;
    }
    int sort_y() const { return feet_y(); }

    Character(const std::string &xml); 
};

#endif
