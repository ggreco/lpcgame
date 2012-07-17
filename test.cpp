#include "game_map.h"
#include "video.h"
#include "scroller.h"
#include "char.h"
#include <iostream>

int main(int argc, char *argv[])
{
    int x = 200, y = 200;
    if (argc < 2)
        return -1;

    if (argc >= 3)
        x = atoi(argv[2]);
    if (argc >=4)
        y = atoi(argv[3]);

    Map map;
    MapSearchNode::RefMap = &map;
    
    if (!map.Load(argv[1]))
        return -2;

    if (map.SpawnPoint(x, y))
        std::cerr << "Using map defined spawn point: " << x << ',' << y << '\n';

    try {
        Video video(640, 480);
        Scroller scroller(video, map);
        Object::Set(video);
        Character ch("hero.xml");

        int msec_per_frame = 20;
        uint32_t wanted = SDL_GetTicks();

        ch.position(x, y);

        for (;;) {
            uint32_t now = SDL_GetTicks();
            wanted += msec_per_frame;

            if (ch.moving()) 
                ch.do_step(now);

            scroller.move(ch.x(), ch.y());


            if (wanted > (now + 2)) {
                SDL_Delay(wanted - now);
            }

            if (now <= wanted) {
                // frame rendering
                scroller.render_background();
                ch.blit(scroller.x(), scroller.y());
                scroller.render_foreground();
                video.flip();
            }

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        exit(0);
                    case SDL_MOUSEBUTTONDOWN:
                        ch.go_to((scroller.x() + e.button.x),  (scroller.y() + e.button.y), now);
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

