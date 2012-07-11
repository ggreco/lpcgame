#include "anim.h"
#include "tinyxml.h"
#include "SDL_image.h"
#include "game_map.h"

void AnimObj::
do_step()
{
   if (step_) {
       step_->PrintNodeInfo();
       step_ = astar_.GetSolutionNext();
   }
}

bool AnimObj::
go_to(int x, int y)
{
    delta_x_ = 0;
    delta_y_ = 0;
    astar_.FreeSolutionNodes();
    astar_.EnsureMemoryFreed();
    MapSearchNode ns(base_x() / MapSearchNode::RefMap->TileWidth(), base_y() / MapSearchNode::RefMap->TileHeight());
    MapSearchNode ne(x / MapSearchNode::RefMap->TileWidth(), y / MapSearchNode::RefMap->TileHeight());

    astar_.SetStartAndGoalStates(ns, ne);

    std::cerr << "Start: ";
    ns.PrintNodeInfo();
    std::cerr << "End: ";
    ne.PrintNodeInfo();

    unsigned int SearchState;
    unsigned int SearchSteps = 0;

    do {
        SearchState = astar_.SearchStep();

        SearchSteps++;
    } while( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING );

    if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED ) 
        step_ = astar_.GetSolutionStart();
    else if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_FAILED ) 
    {
        std::cerr << "Search terminated. Did not find goal state\n";
        step_ = NULL;
    }
    std::cerr << "SearchSteps : " << SearchSteps << "\n";
}

bool AnimObj::
Set(const std::string &anim)
{
   AnimCIt it = animations_.find(anim);

   if (it == animations_.end())
       return false;

   if (actual_ == anim)
       return true;

   actual_ = anim;
   frame_ = 0;
   next_frame_ = it->second.frames[0].persistence + SDL_GetTicks();
   return true;
}

AnimObj::AnimObj(const std::string &xmlname) : frame_(0), step_(NULL)
{
   TiXmlDocument doc(xmlname);
   if (!doc.LoadFile())
        throw std::string("Unable to load animobj from " + xmlname + ": " + doc.ErrorDesc());

   if (doc.Error()) 
       throw std::string("Error parsing " + xmlname + ": " + doc.ErrorDesc());
   
   TiXmlNode *baseNode = doc.FirstChild("character");
   if (!baseNode)
       throw std::string("Invalid XML " + xmlname);

   if (const TiXmlNode *n = baseNode->FirstChild("name")) 
       name_ = n->ToElement()->GetText();

   for (const TiXmlNode *sheetNode = baseNode->FirstChild("sheet"); sheetNode; 
                         sheetNode = baseNode->IterateChildren("sheet", sheetNode)) {
       int id;
       const TiXmlElement* e = sheetNode->ToElement();
       e->Attribute("id", &id);

       std::map<std::string, std::string> layers;

       for (const TiXmlNode *layer = sheetNode->FirstChild("layer"); layer; 
                             layer = sheetNode->IterateChildren("layer", layer)) {
            const TiXmlElement* e = layer->ToElement();

            layers[e->Attribute("id")] = e->GetText();
       }

       if (SDL_Surface *s = load_sheets(layers))
           sheets_[id] = s;
       else
           throw std::string("Unable to create animation sheet for " + xmlname);

       std::cerr << "Loaded sheet " << id << " for " << xmlname <<  " with " << layers.size() << " layers\n";
      
   }

   for (const TiXmlNode *animNode = baseNode->FirstChild("anim"); animNode;
                         animNode = baseNode->IterateChildren("anim", animNode)) {
       int s;
       const TiXmlElement* e = animNode->ToElement();
       Anim anim;
       anim.name = e->Attribute("id");
       e->Attribute("sheet", &s);
      
       SheetCIt it = sheets_.find(s);
       if (it == sheets_.end()) 
           throw std::string("Unavailable sheet for animation " + anim.name);
      
       anim.sheet = it->second;
       const TiXmlNode *frameNode = animNode->FirstChild("frame");
       while (frameNode) {
           Frame frame;
           const TiXmlElement *e = frameNode->ToElement();
           int x, y, w, h;
           e->Attribute("x", &x);
           e->Attribute("y", &y);
           e->Attribute("w", &w);
           e->Attribute("h", &h);
           frame.box.x = x;
           frame.box.y = y;
           frame.box.w = w;
           frame.box.h = h;
           e->Attribute("time", &frame.persistence);
           anim.frames.push_back(frame);
           frameNode = animNode->IterateChildren("frame", frameNode);
       }
       animations_[anim.name] = anim;
       if (actual_.empty())
           actual_ = anim.name;

       std::cerr << "Loaded animation " << anim.name << " with " << anim.frames.size() << " frames.\n";
   }

   next_frame_ = SDL_GetTicks();
}


void AnimObj::
blit(int x, int y) 
{
    uint32_t now = SDL_GetTicks();
    Anim &a = animations_[actual_];

//    std::cerr << "frame: " << frame_ << " next:"<< next_frame_ << " now:" << now << '\n';
    if (now >= next_frame_) {
        frame_++;
        if (frame_ >= a.frames.size())
            frame_ = 0;
        next_frame_ += a.frames[frame_].persistence;
    }

    lowblit(a.sheet, a.frames[frame_].box, x, y);
}

SDL_Surface *AnimObj::
load_sheets(const std::map<std::string, std::string> &sheets)
{
    SDL_Surface *temp = NULL;

    for (std::map<std::string, std::string>::const_iterator it =  sheets.begin();
                                                            it != sheets.end(); ++it) {
        SDL_Surface *b = IMG_Load(it->second.c_str());

        if (!b) 
            throw std::string("Unable to load character sheet layer id " + it->first + ", filename " + it->second);

        if (!temp)
            temp = b;
        else {
            if (b->w != temp->w ||
                b->h != temp->h)
                    throw std::string("Invalid layer " + it->first + ", filename " + it->second);
            SDL_BlitSurface(b, NULL, temp, NULL);
            SDL_FreeSurface(b);
        }
    }

    SDL_Surface * bitmap =  SDL_DisplayFormatAlpha(temp);
    SDL_FreeSurface(temp);
    return bitmap;
}
