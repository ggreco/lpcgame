#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "Tmx.h"
#include <map>
#include <string>

struct SDL_Surface;

class Map
{
    Tmx::Map map_;
    std::string path_;

    bool load_tileset(const Tmx::Tileset *);
public:    
    int TileWidth() const { return map_.GetTileWidth(); }
    int TileHeight() const { return map_.GetTileHeight(); }
    bool Load(const std::string &name) {
        std::string::size_type p = name.find_last_of('/');
        if (p != std::string::npos)
            path_ = name.substr(0, p + 1);

        map_.ParseFile(name);
        return !map_.HasError();
    }
    void Render(SDL_Surface *, int, int);
};
#endif
