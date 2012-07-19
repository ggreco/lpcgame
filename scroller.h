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
    int x_, y_; // render position from the map, set through move

public:
    void refresh() {
        tw_ = map_->TileWidth();
        th_ = map_->TileHeight();
        if (background_)
            SDL_FreeSurface(background_);

        background_ = SDL_CreateRGBSurface(SDL_SWSURFACE, video_.width() + tw_ * 2, video_.height() + th_ * 2, 
                            video_.format()->BitsPerPixel, video_.format()->Rmask,  
                            video_.format()->Gmask, video_.format()->Bmask,  video_.format()->Amask);
    
        map_->Render(background_, 0, 0);
    }
    
    Scroller(Video &v, Map &m) : map_x_(0), map_y_(0), map_(&m), background_(NULL), video_(v), x_(0), y_(0) {
    }
    ~Scroller() {
        if (background_)
            SDL_FreeSurface(background_);
    }
    void render_background() {
        SDL_Rect src_rect = {x_ - map_x_, y_ - map_y_, video_.width(), video_.height()};

        if (src_rect.x < 0) 
            src_rect.x = 0;
        if (src_rect.y < 0)
            src_rect.y = 0;
        video_.blit(background_, src_rect);
    }
    void render_foreground() {
        SDL_Rect src_rect = {x_ - map_x_, y_ - map_y_, video_.width(), video_.height()};

        if (src_rect.x < 0) 
            src_rect.x = 0;
        if (src_rect.y < 0)
            src_rect.y = 0;
        map_->RenderAbove(video_.surface(), map_x_/tw_, map_y_/th_,  map_x_ - x_, map_y_ - y_);
    }

    int x() const { return x_; }
    int y() const { return y_; }

    void move(int x, int y) {
        bool need_render = false;

        x_ = x - video_.width()/2; 
        y_ = y - video_.height()/2;

        if (x_ < 0)
            x_ = 0;
        if (y_ < 0)
            y_ = 0;

        if (abs(x_ - map_x_ - tw_) > tw_) {
            map_x_ = ((x_ - tw_) / tw_) * tw_;

            need_render = true;
        }        
        if (abs(y_ - map_y_ - th_) > th_) {
            map_y_ = ((y_ - th_)/ th_) * th_;

            need_render = true;
        }        

        if (need_render) 
            map_->Render(background_, map_x_/tw_, map_y_/th_);
    }
};

#endif
