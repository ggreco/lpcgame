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
    if (!map.Load(argv[1]))
        return -2;

    try {
        Video video(640, 480);
        Scroller scroller(video, map);
        Object::Set(video);
        Character ch("male_walkcycle.png", "female_hair.png", "male_pants.png");

        ch.position(200, 200);
        int msec_per_frame = 20;
        int x = map.TileWidth(), y = map.TileHeight(), delta_x = 0, delta_y = 0;
        uint32_t wanted = SDL_GetTicks();

        for (;;) {
            x += delta_x;
            y += delta_y;

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
                ch.blit(x, y, 2);
                video.flip();
            }

            SDL_Event e;
            if (!SDL_PollEvent(&e))
                continue;

            switch (e.type) {
                case SDL_QUIT:
                    exit(0);
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_RIGHT:
                            delta_x = 4;
                            break;
                        case SDLK_LEFT:
                            delta_x = -4;
                            break;
                        case SDLK_UP:
                            delta_y = -4;
                            break;
                        case SDLK_DOWN:
                            delta_y = 4;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            exit(0);
                        case SDLK_RIGHT:
                        case SDLK_LEFT:
                            delta_x = 0;
                            break;
                        case SDLK_UP:
                        case SDLK_DOWN:
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

