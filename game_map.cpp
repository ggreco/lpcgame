#include <SDL.h>
#include "SDL_image.h"
#include "Tmx.h"
#include <iostream>
#include "game_map.h"

bool Map::
load_tileset(const Tmx::Tileset *tileset)
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
        ds = SDL_DisplayFormatAlpha(s);
        SDL_FreeSurface(s);
        std::cerr << "Loaded tileset " <<  tileset->GetImage()->GetSource() << '\n';
        const_cast<Tmx::Image *>(tileset->GetImage())->SetData(ds);
        return true;
    }
    else
        std::cerr << "ERROR loading tileset " <<  tileset->GetImage()->GetSource() << '\n';

    return false;
}

void Map::Render(SDL_Surface *dest, int startx, int starty)
{
    SDL_FillRect(dest, NULL, 0);

    for (int i = 0; i < map_.GetNumLayers(); ++i) 
    {
        // Get a layer.
        const Tmx::Layer *layer = map_.GetLayer(i);

//        std::cerr << "Layer " << i << ", size " << layer->GetWidth() << 'x' << layer->GetHeight() << ", rendering from " << startx << ',' << starty << "\n";

        for (int x = startx; x < layer->GetWidth(); ++x) 
        {
            for (int y = starty; y < layer->GetHeight(); ++y) 
            {
                const Tmx::MapTile &tile =  layer->GetTile(x, y);

                if(tile.id == 0)
                    continue;

                const Tmx::Tileset *tileset = map_.GetTileset(tile.tilesetId);

                if ((y - starty) * tileset->GetTileHeight() >= dest->h ||
                    (x - startx) * tileset->GetTileWidth() >= dest->w)
                    break;

                if (!tileset->GetImage()->GetData())
                    if (!load_tileset(tileset)) {
                        std::cerr << "Unable to load " << path_ << tileset->GetImage()->GetSource() << "\n";
                        continue;
                    }

                int tileset_col = (tile.id % (tileset->GetImage()->GetWidth() / tileset->GetTileWidth()));
                int tileset_row = (tile.id / (tileset->GetImage()->GetWidth() / tileset->GetTileWidth()));

                SDL_Rect rect, destRect;
                rect.x = (tileset->GetMargin() + (tileset->GetTileWidth() + tileset->GetSpacing()) * tileset_col);
                rect.y = (tileset->GetMargin() + (tileset->GetTileHeight() + tileset->GetSpacing()) * tileset_row);
                rect.w = tileset->GetTileWidth();
                rect.h = tileset->GetTileHeight();
                
                destRect.x = (x - startx) * tileset->GetTileWidth();
                destRect.y = (y - starty) * tileset->GetTileHeight(); 

                SDL_BlitSurface( (SDL_Surface*)tileset->GetImage()->GetData(), &rect, dest, &destRect);
            }

        }
    }
}

