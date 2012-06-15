#ifndef CHARACTER_H
#define CHARACTER_H

#include "video.h"
#include "anim.h"

class Character : public AnimObj
{
public:
    Character(const std::string &xml); 
};

#endif
