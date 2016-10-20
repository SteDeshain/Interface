////solid_h.h
//created by SteDeshain
//created on 2016/08/16
//latest updated on 2016/08/16

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "game.h"
#include "status.h"
#include "scene1.h"
#include "label.h"
#include "drawable_comp.h"

namespace gameTest
{

class Game1: public steg::Game
{

public:
    SDL_Renderer* render;
    Scene1* testScene;
    TTF_Font* debugFont;
    char buffer[128];
    Label* fps;

    virtual DBG_Status InitGame();

    //for debug use
    virtual DBG_Status Run();

};

}
