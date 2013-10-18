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


    void set_direction(int, int, int, int, int, int);
    typedef std::map<int, SDL_Surface *> SheetMap;
    typedef SheetMap::const_iterator SheetCIt;
    SheetMap sheets_;
    void parse_animations(const TiXmlNode *);
protected:    
    AStarSearch<MapSearchNode> astar_;
    MapSearchNode *step_;
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
    int32_t last_;
    double speed_;
    double distance_, microstep_x_, microstep_y_;
public:    
    AnimObj(const std::string &);

    // a sheet can be build on three different levels
    SDL_Surface *load_sheets(const std::map<std::string, std::string> &);

    /// check if an object has a specific animation
    bool Has(const std::string &anim) { return animations_.find(anim) != animations_.end(); }
    bool Set(const std::string &);
    std::string Get() { return actual_; }

    // speed of the objects in pixels/sec
    float speed();
    void blit(int x, int y) const;
    void update_animation(uint32_t ts);
    bool on_screen(int mapx, int mapy) const;
    virtual int sort_y() const { return base_y(); }
    // movement
    bool go_to(int x, int y, uint32_t msec);
    bool moving() const { return step_ != NULL; }
    void do_step(uint32_t msec);
};

#endif
