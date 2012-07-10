#include "game_map.h"
#include "video.h"
#include "scroller.h"
#include "char.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
        return -1;

    Map map;
    MapSearchNode::RefMap = &map;
    
    if (!map.Load(argv[1]))
        return -2;

    try {
        Video video(640, 480);
        Scroller scroller(video, map);
        Object::Set(video);
        Character ch("hero.xml");

        int cx = 200, cy = 200;
        int msec_per_frame = 20;
        int x = map.TileWidth(), y = map.TileHeight(), delta_x = 0, delta_y = 0;
        uint32_t wanted = SDL_GetTicks();

        ch.position(cx, cy);

        for (;;) {
            if (map.walkable(ch.feet_x() + delta_x, ch.feet_y() + delta_y)) {
                //x += delta_x;
                //y += delta_y;
                cx += delta_x;
                cy += delta_y;
            }

            if (ch.moving()) 
                ch.do_step();

//          ch.position(cx, cy);

            if (x < 0)
                x = 0;
            if (y < 0)
                y = 0;

            scroller.move(x, y);

            uint32_t now = SDL_GetTicks();
            wanted += msec_per_frame;

            if (wanted > (now + 2)) {
                SDL_Delay(wanted - now);
            }

            if (now <= wanted) {
                // frame rendering
                scroller.render(x, y);
                ch.blit(x, y);
                video.flip();
            }


            SDL_Event e;
            if (!SDL_PollEvent(&e))
                continue;

            switch (e.type) {
                case SDL_QUIT:
                    exit(0);
                case SDL_MOUSEBUTTONDOWN:
                    ch.go_to((x + e.button.x),  (y + e.button.y));
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_RIGHT:
                            ch.Set("walk_right");
                            delta_x = 4;
                            break;
                        case SDLK_LEFT:
                            ch.Set("walk_left");
                            delta_x = -4;
                            break;
                        case SDLK_UP:
                            ch.Set("walk_up");
                            delta_y = -4;
                            break;
                        case SDLK_DOWN:
                            ch.Set("walk_down");
                            delta_y = 4;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (e.key.keysym.sym) {
                        case SDLK_d:
                            map.dump_screen_map();
                            break;
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            exit(0);
                        case SDLK_RIGHT:
                            ch.Set("idle_right");
                            delta_x = 0;
                            break;
                        case SDLK_LEFT:
                            ch.Set("idle_left");
                            delta_x = 0;
                            break;
                        case SDLK_UP:
                            ch.Set("idle_up");
                            delta_y = 0; 
                            break;
                        case SDLK_DOWN:
                            ch.Set("idle_down");
                            delta_y = 0; 
                            break;
                        case SDLK_HOME:
                            x = 0; y = 0;
                            delta_x = delta_y = 0;
                            break;
                    }
                    break;
            }
        }        
    }
    catch (std::string &err) {
        std::cerr << "Terminated with exception: " << err << "\n";
    }
    catch (...) {
        std::cerr << "Terminated with unknown exception!\n";
    }
}

