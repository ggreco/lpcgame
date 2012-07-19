#include "engine.h"
#include "anim.h"

Engine::
Engine(int w, int h, int fps) : 
    video_(w, h), scroller_(video_, map_), 
    msec_per_frame_(1000/fps), hero_(NULL)
{
    MapSearchNode::RefMap = &map_;
    Object::Set(video_);
}

bool Engine::
iteration()
{
    uint32_t now = SDL_GetTicks();
    wanted_ += msec_per_frame_;

    // let's move the objects if needed
    for (std::set<Object *>::const_iterator it = objs_.begin(); it != objs_.end(); ++it)
        if (AnimObj *a = dynamic_cast<AnimObj *>(*it)) 
            if (a->moving())
                a->do_step(now);

    // then move the scroll area to the hero position
    if (hero_)
        scroller_.move(hero_->x(), hero_->y());

    // wait some time
    if (wanted_ > (now + 2)) {
        SDL_Delay(wanted_ - now);
    }

    if (now <= wanted_) {
        // frame rendering, first the map
        scroller_.render_background();
        // then the object
        for (std::set<Object *>::const_iterator it = objs_.begin(); it != objs_.end(); ++it)
            (*it)->blit(scroller_.x(), scroller_.y());
        // and finally the overlay objects
        scroller_.render_foreground();

        video_.flip();
    }

    // SDL event handling
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                return false;
            case SDL_MOUSEBUTTONDOWN:
                if (hero_)
                    hero_->go_to((scroller_.x() + e.button.x),  (scroller_.y() + e.button.y), now);
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_d:
                        map_.dump_screen_map();
                        break;
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        return false;
                        break;
                }
                break;
        }
    }
    return true;
}

