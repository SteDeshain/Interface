#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "game.h"
#include "status.h"
#include "scene1.h"
#include "label.h"
#include "drawable_comp.h"
#include "stel.h"

namespace gameTest
{

class InterfaceGame: public steg::Game
{

public:
    ~InterfaceGame();

    SDL_Renderer* render;
    Scene1* testScene;
    TTF_Font* debugFont;
    char buffer[128];
    Label* fps;

    virtual DBG_Status InitGame();

    //for debug use
    virtual DBG_Status Run();

private:
    LuaResult* conf;

};

}
