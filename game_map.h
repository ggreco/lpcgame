#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "Tmx.h"
#include <map>
#include <string>
#include <stdlib.h>

struct SDL_Surface;

class Map
{
    Tmx::Map map_;
    std::string path_;
    char *navigation_map_;
    int navigation_width_, navigation_height_;
   
    bool load_tileset(const Tmx::Tileset *);
    void build_navigation_map();
    void render_layer(const Tmx::Layer *layer, SDL_Surface *dest, int startx, int offset_x, int starty, int offset_y);

public:    
    Map() : navigation_map_(NULL), navigation_height_(0), navigation_width_(0) {}
    ~Map() { if (navigation_map_) ::free(navigation_map_); }
    int TileWidth() const { return map_.GetTileWidth(); }
    int TileHeight() const { return map_.GetTileHeight(); }
    bool Load(const std::string &name) {
        std::string::size_type p = name.find_last_of('/');
        if (p != std::string::npos)
            path_ = name.substr(0, p + 1);

        map_.ParseFile(name);
        if(map_.HasError())
            return false;

        build_navigation_map();
        return true;
    }
    bool SpawnPoint(int &x, int &y);
    int navigation_map(int, int) const;
    void dump_screen_map() const;
    bool walkable(int, int) const;
    void Render(SDL_Surface *, int, int, bool above = false);
    void RenderAbove(SDL_Surface *, int, int, int, int);
    int map_width() const { return navigation_width_ * TileWidth(); }
    int map_height() const { return navigation_height_ * TileHeight(); }
};
#endif
