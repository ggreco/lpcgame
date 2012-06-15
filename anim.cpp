#include "anim.h"
#include "tinyxml.h"
#include "SDL_image.h"

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

AnimObj::AnimObj(const std::string &xmlname) : frame_(0)
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
       std::string base, cloth, hair;

       if (const TiXmlNode *n = sheetNode->FirstChild("base")) 
           base = n->ToElement()->GetText();
       if (const TiXmlNode *n = sheetNode->FirstChild("cloth")) 
           cloth = n->ToElement()->GetText();
       if (const TiXmlNode *n = sheetNode->FirstChild("hair")) 
           hair = n->ToElement()->GetText();
       
       if (SDL_Surface *s = load_sheet(base, cloth, hair))
           sheets_[id] = s;
       else
           throw std::string("Unable to create animation sheet for " + base);

       std::cerr << "Loaded sheet " << id << " from " << base << "\n";
      
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
load_sheet(const std::string &base, const std::string &cloth, const std::string &hair)
{
    SDL_Surface *b = IMG_Load(base.c_str()), *c = NULL, *h = NULL;

    if (!b) 
        throw std::string("Unable to load character with skin " + base);

    if (!cloth.empty()) {
        c = IMG_Load(cloth.c_str());    


        if (c && (c->w != b->w ||
                  c->h != b->h))
            throw std::string("Invalid cloth " + cloth + " for character base " + base);
    }
    if (!hair.empty()) {
        h = IMG_Load(hair.c_str());
        if (h && (h->w != b->w ||
                  h->h != b->h))
            throw std::string("Invalid hair " + hair + " for character base " + base);
    }

    if (c) {
        SDL_BlitSurface(c, NULL, b, NULL);
        SDL_FreeSurface(c);
    } 
    if (h) {
        SDL_BlitSurface(h, NULL, b, NULL);
        SDL_FreeSurface(h);
    }
    SDL_Surface * bitmap =  SDL_DisplayFormatAlpha(b);
    SDL_FreeSurface(b);
    return bitmap;
}
