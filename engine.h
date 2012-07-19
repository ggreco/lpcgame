#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "scroller.h"
#include <set>

class Object;
class AnimObj;

class Engine
{
    std::set<Object *> objs_;
    Map map_;
    Video video_;
    Scroller scroller_;
    int msec_per_frame_, wanted_;
    AnimObj *hero_;
public:
    Engine(int w = 640, int h = 480, int fps = 50);
    void hero(AnimObj *o) { hero_ = o; }
    Map &map() { return map_; }
    void start() { wanted_ = SDL_GetTicks(); }
    bool load_map(const std::string &name) {
        if (map_.Load(name)) {
            scroller_.refresh();
            return true;
        }
        return false;
    }
    void add(Object *o) { objs_.insert(o); }
    bool iteration();
};

#endif
