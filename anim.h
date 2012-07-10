#ifndef ANIM_OBJ_H
#define ANIM_OBJ_H

#include <vector>
#include <map>
#include <string>
#include "obj.h"
#include "stlastar.h"

// 1) every animated object is an anim object
// 2) every animated object MUST have a generic idle animation
// 3) every character MUST have walk_left, walk_right, walk_up, walk_down animations as well as idle ones (idle_left, _right, _up and _down)

class AnimObj : public Object
{

    struct Frame {
        SDL_Rect box;
        int persistence; // time to show in msecs
    };


    typedef std::map<int, SDL_Surface *> SheetMap;
    typedef SheetMap::const_iterator SheetCIt;
    SheetMap sheets_;
protected:    
    AStarSearch<MapSearchNode> astar_;
    MapSearchNode *step_;
    int delta_x_, delta_y_;
    typedef std::vector<Frame> FrameList;
    struct Anim {
        SDL_Surface *sheet;
        std::string name;
        FrameList frames;
    };
    typedef std::map<std::string, Anim> AnimMap;
    typedef AnimMap::const_iterator AnimCIt;
    AnimMap animations_;

    std::string actual_;
    int frame_;
    // time in msec to show next frame
    uint32_t next_frame_;

    // object name
    std::string name_;
    virtual int base_x() const {
        AnimCIt it = animations_.find(actual_);

        if (it != animations_.end()) 
            return x_ + it->second.frames[frame_].box.w / 2;
        return x_;
    }
    virtual int base_y() const {
        AnimCIt it = animations_.find(actual_);
        if (it != animations_.end()) 
            return y_ + it->second.frames[frame_].box.h;
        return y_;
    }    
public:    
    AnimObj(const std::string &);

    // a sheet can be build on three different levels
    SDL_Surface *load_sheets(const std::map<std::string, std::string> &);

    /// check if an object has a specific animation
    bool Has(const std::string &anim) { return animations_.find(anim) != animations_.end(); }
    bool Set(const std::string &);
    std::string Get() { return actual_; }

    void blit(int x, int y);

    // movement
    bool go_to(int x, int y);
    bool moving() const { return step_ != NULL; }
    virtual void do_step();
};

#endif
