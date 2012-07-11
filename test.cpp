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

        int msec_per_frame = 20;
        uint32_t wanted = SDL_GetTicks();

        ch.position(200, 200);

        for (;;) {
            if (ch.moving()) 
                ch.do_step();

            scroller.move(ch.x(), ch.y());

            uint32_t now = SDL_GetTicks();
            wanted += msec_per_frame;

            if (wanted > (now + 2)) {
                SDL_Delay(wanted - now);
            }

            if (now <= wanted) {
                // frame rendering
                scroller.render();
                ch.blit(scroller.x(), scroller.y());
                video.flip();
            }

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        exit(0);
                    case SDL_MOUSEBUTTONDOWN:
                        ch.go_to((scroller.x() + e.button.x),  (scroller.y() + e.button.y));
                        break;
                    case SDL_KEYUP:
                        switch (e.key.keysym.sym) {
                            case SDLK_d:
                                map.dump_screen_map();
                                break;
                            case SDLK_ESCAPE:
                            case SDLK_q:
                                exit(0);
                                break;
                        }
                        break;
                }
            }
        }        
    }
    catch (std::string &err) {
        std::cerr << "Terminated with exception: " << err << "\n";
    }
    catch (std::exception &err) {
        std::cerr << "Terminated STD with exception: " << err.what() << "\n";
    }
    catch (...) {
        std::cerr << "Terminated with unknown exception!\n";
    }
}

