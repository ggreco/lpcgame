#include <SDL.h>
#include "SDL_image.h"
#include "Tmx.h"
#include <iostream>
#include "game_map.h"

void Map::load_tileset(const Tmx::Tileset *tileset)
{
    SDL_Surface *ds = NULL;

    if (SDL_Surface *s = IMG_Load((path_ + tileset->GetImage()->GetSource()).c_str())) {
        std::string transparent = tileset->GetImage()->GetTransparentColor();

        if (!transparent.empty()) {
            uint32_t c;
            c = strtol(transparent.substr(0, 2).c_str(), NULL, 16) << s->format->Rshift;
            c |= strtol(transparent.substr(2, 2).c_str(), NULL, 16) << s->format->Gshift;
            c |= strtol(transparent.substr(4, 2).c_str(), NULL, 16) << s->format->Bshift;
            SDL_SetColorKey(s,  SDL_SRCCOLORKEY|SDL_RLEACCEL, c);
        }    
        ds = SDL_DisplayFormat(s);
        SDL_FreeSurface(s);
    }
    tiles_[tileset] = ds;

}

void Map::Render(SDL_Surface *dest, int startx, int starty)
{
    for (int i = 0; i < map_.GetNumLayers(); ++i) 
    {
        // Get a layer.
        const Tmx::Layer *layer = map_.GetLayer(i);

 //       std::cerr << "Layer " << i << ", size " << layer->GetWidth() << 'x' << layer->GetHeight() << ", rendering from " << startx << ',' << starty << "\n";

        for (int x = startx; x < layer->GetWidth(); ++x) 
        {
            for (int y = starty; y < layer->GetHeight(); ++y) 
            {
                int CurTile = layer->GetTileId(x, y);

                if(CurTile == 0)
                    continue;

                const Tmx::Tileset *tileset = map_.FindTileset(CurTile);

                if ((y - starty) * tileset->GetTileHeight() >= dest->h ||
                    (x - startx) * tileset->GetTileWidth() >= dest->w)
                    break;

                TMapIt it = tiles_.find(tileset);

                if (it == tiles_.end()) {         
                    load_tileset(tileset);
                    it = tiles_.find(tileset);
                }

                if (!it->second) {
                    std::cerr << "Unable to load " << path_ << tileset->GetImage()->GetSource() << "\n";
                    continue;
                }

                int tileset_col = (CurTile % (tileset->GetImage()->GetWidth() / tileset->GetTileWidth()));
                int tileset_row = (CurTile / (tileset->GetImage()->GetWidth() / tileset->GetTileWidth()));

                SDL_Rect rect_CurTile, destRect;
                rect_CurTile.x = (tileset->GetMargin() + (tileset->GetTileWidth() + tileset->GetSpacing()) * tileset_col);
                rect_CurTile.y = (tileset->GetMargin() + (tileset->GetTileHeight() + tileset->GetSpacing()) * tileset_row);
                rect_CurTile.w = tileset->GetTileWidth();
                rect_CurTile.h = tileset->GetTileHeight();
                
                destRect.x = (x - startx) * tileset->GetTileWidth();
                destRect.y = (y - starty) * tileset->GetTileHeight(); 

                SDL_BlitSurface( it->second, &rect_CurTile, dest, &destRect);
            }

        }
    }
}

