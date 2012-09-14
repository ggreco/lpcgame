#include "anim.h"
#include "tinyxml.h"
#include "SDL_image.h"
#include "game_map.h"
#include <math.h>

void AnimObj::
set_direction(int sx, int sy, int next_x, int next_y, int tw, int th)
{
/*    if (UserState *next = PeekSolutionNext()) {

    }
    else*/
    {

        next_x *= tw; next_x += tw/2;
        next_y *= th; next_y += th/2;
    }

    distance_ = sqrt((sx  - next_x) * (sx - next_x) + (sy - next_y) * (sy - next_y));
//    std::cerr << "Distance between (" << sx << ',' << sy << ") and (" << next_x << ',' << next_y << ") is " << distance_ << ".\n";

    double delta_step = distance_ / speed_;
    microstep_x_ = ((double)(next_x - sx)) / delta_step;
    microstep_y_ = ((double)(next_y - sy)) / delta_step;

    if (microstep_x_ > 0.0 && microstep_y_ > 0.0) {
        if (microstep_x_ > microstep_y_)
            Set("walk_right");
        else 
            Set("walk_down");
    }
    else if (microstep_x_ < 0.0 && microstep_y_ < 0.0) {
        if (microstep_y_ > microstep_x_)
            Set("walk_left");
        else
            Set("walk_up");
    }
    else if (microstep_x_ < 0.0 && microstep_y_ > 0.0) {
        if (microstep_x_ < -microstep_y_)
            Set("walk_left");
        else
            Set("walk_down");
    }
    else {
        if (microstep_x_ > -microstep_y_)
            Set("walk_right");
        else
            Set("walk_up");
    }
}

void AnimObj::
do_step(uint32_t now)
{
    if (!step_)
        return;

    int tw = MapSearchNode::RefMap->TileWidth(),
        th = MapSearchNode::RefMap->TileHeight();

    double delta = (now - last_) / 1000.0;
    last_ = now;
    int actual_x = base_x(),
        actual_y = base_y();

    if (distance_ <= 0.0) {    
        step_ = astar_.GetSolutionNext();

//        std::cerr << "Next step:";

        if (!step_) {
//            std::cerr << "GOAL REACHED\n";
            // arrived!
            if (microstep_x_ > 0 && microstep_y_ > 0) {
                if (microstep_y_ > microstep_x_)
                    Set("idle_down");
                else 
                    Set("idle_right");
            }
            else if (microstep_x_ < 0 && microstep_y_ < 0) {
                if (microstep_y_ > microstep_x_)
                    Set("idle_left");
                else
                    Set("idle_up");
            }
            else if (microstep_x_ < 0 && microstep_y_ > 0) {
                if (microstep_x_ < -microstep_y_)
                    Set("idle_left");
                else
                    Set("idle_down");
            }
            else if (microstep_x_ > -microstep_y_)
                Set("idle_right");
            else
                Set("idle_up");

            return;
        }
//        step_->PrintNodeInfo();
        set_direction(actual_x, actual_y,  step_->x, step_->y, tw, th);
    }
/*
    std::cerr << "Dir: " << direction_ << " X:" << actual_x << " Y:" << actual_y << " d:" << delta
              << " distance:"  << distance_ 
              << " dx:" << microstep_x_ << " dy:" << microstep_y_ <<'\n';
              */
    distance_ -= delta * speed_;

    /*
    if we are arrived to the next step I force it to be in the EXACT position <- not needed
    if (distance_ <= 0.0) {
        x_ = step_->x * tw + tw/2 + actual_x - x_;
        y_ = step_->y * th + th/2 + actual_y - y_;
    }
   */
    x_ += delta * microstep_x_; 
    y_ += delta * microstep_y_;
}

bool AnimObj::
go_to(int x, int y, uint32_t msec)
{
    int tw = MapSearchNode::RefMap->TileWidth(),
        th = MapSearchNode::RefMap->TileHeight();
    
    distance_ = 0.0;
    astar_.FreeSolutionNodes();
    astar_.EnsureMemoryFreed();
    MapSearchNode ns(base_x() /tw , base_y() /th);
    MapSearchNode ne(x / tw, y / th);

    astar_.SetStartAndGoalStates(ns, ne);

    /*
    std::cerr << "Start: ";
    ns.PrintNodeInfo();
    std::cerr << "End: ";
    ne.PrintNodeInfo();
    */

    unsigned int SearchState;
    unsigned int SearchSteps = 0;

    do {
        if (SearchSteps > 600) {
//            std::cerr << "Search interrupted. Did not find goal state\n";
            astar_.CancelSearch();
        }
        SearchState = astar_.SearchStep();

        SearchSteps++;

    } while( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING );

    if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED ) {
        step_ = astar_.GetSolutionStart();
        // always skip first step, it's the position we are in
        step_ = astar_.GetSolutionNext();

        if (step_)
            set_direction(base_x(), base_y(), step_->x, step_->y, tw, th);
    }
    else if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_FAILED ) 
    {
//        std::cerr << "Search terminated. Did not find goal state\n";
        step_ = NULL;
    }
//    std::cerr << "SearchSteps : " << SearchSteps << "\n";
    last_ = msec;

    return step_ != NULL;
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

void AnimObj::
parse_animations(const TiXmlNode *baseNode)
{
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
   }
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

   if (const TiXmlNode *n = baseNode->FirstChild("speed")) 
       speed_ = atof(n->ToElement()->GetText());
   else
       speed_ = 100.0f; // speed of 100 to objects with undefined speed

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

   // for each "animation" instance execute the parse of the file contained in the tag src
   for (const TiXmlNode *n = baseNode->FirstChild("animation"); n; n = baseNode->IterateChildren("animation", n)) {
       const TiXmlElement* e = n->ToElement();

       std::string name = e->Attribute("src");

       TiXmlDocument anim_doc(name);
       if (anim_doc.LoadFile()) {
            if (TiXmlNode *an = anim_doc.FirstChild("animation"))
                parse_animations(an);
       }
   }
   // executing the parse of the local animations
   parse_animations(baseNode);

   std::cerr << "Loaded " << animations_.size() << " animations for " << xmlname << '\n';
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
