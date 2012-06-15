#ifndef ANIM_OBJ_H
#define ANIM_OBJ_H

#include <vector>
#include <map>
#include <string>
#include "obj.h"

// 1) every animated object is an anim object
// 2) every animated object MUST have a generic idle animation
// 3) every character MUST have walk_left, walk_right, walk_up, walk_down animations as well as idle ones (idle_left, _right, _up and _down)

class AnimObj : public Object
{
    struct Frame {
        SDL_Rect box;
        int persistence; // time to show in msecs
    };

    typedef std::vector<Frame> FrameList;
    struct Anim {
        SDL_Surface *sheet;
        std::string name;
        FrameList frames;
    };

    typedef std::map<std::string, Anim> AnimMap;
    typedef AnimMap::const_iterator AnimCIt;
    typedef std::map<int, SDL_Surface *> SheetMap;
    typedef SheetMap::const_iterator SheetCIt;
    SheetMap sheets_;
    AnimMap animations_;

    std::string actual_;
    int frame_;
    // time in msec to show next frame
    uint32_t next_frame_;

    // object name
    std::string name_;
public:    
    AnimObj(const std::string &);

    // a sheet can be build on three different levels
    SDL_Surface *load_sheet(const std::string &, const std::string & = "", const std::string & = "");

    /// check if an object has a specific animation
    bool Has(const std::string &anim) { return animations_.find(anim) != animations_.end(); }
    bool Set(const std::string &);
    std::string Get() { return actual_; }

    void blit(int x, int y);
};

#endif
