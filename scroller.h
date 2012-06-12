#ifndef SCROLLER_H
#define SCROLLER_H

#include "game_map.h"
#include "video.h"

class Scroller
{
    int tw_, th_; // tile size
    int map_x_, map_y_; // map offset in background
    Map *map_;
    SDL_Surface *background_;
    Video &video_;

    void init(Map &map) {
        tw_ = map.TileWidth();
        th_ = map.TileHeight();
        if (background_)
            SDL_FreeSurface(background_);

        background_ = SDL_CreateRGBSurface(SDL_SWSURFACE, video_.width() + tw_ * 2, video_.height() + th_ * 2, 
                            video_.format()->BitsPerPixel, video_.format()->Rmask,  
                            video_.format()->Gmask, video_.format()->Bmask,  video_.format()->Amask);
        map_ = &map;
        map_->Render(background_, 0, 0);
    }
public:
    Scroller(Video &v, Map &m) : map_x_(0), map_y_(0), background_(NULL), video_(v) {
        init(m);
    }
    ~Scroller() {
        if (background_)
            SDL_FreeSurface(background_);
    }
    void render(int x, int y) {
        SDL_Rect src_rect = {x - map_x_, y - map_y_, video_.width(), video_.height()};
        if (src_rect.x < 0) 
            src_rect.x = 0;
        if (src_rect.y < 0)
            src_rect.y = 0;
        video_.blit(background_, src_rect);
    }

    void move(int x, int y) {
        bool need_render = false;

        if (abs(x - map_x_ - tw_) > tw_) {
            map_x_ = ((x - tw_) / tw_) * tw_;

            need_render = true;
        }        
        if (abs(y - map_y_ - th_) > th_) {
            map_y_ = ((y - th_)/ th_) * th_;

            need_render = true;
        }        

        if (need_render)
            map_->Render(background_, map_x_/tw_, map_y_/th_);
    }
};

#endif
