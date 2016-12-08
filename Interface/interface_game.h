#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "game.h"
#include "status.h"
#include "scene1.h"
#include "label.h"
#include "drawable_comp.h"
#include "stel.h"
#include <list>

namespace interface
{

class InterfaceGame: public steg::Game
{

public:
    //when using a std::string to assign to a const char*,
    //the origin std::string will be invalid after its scope(like a funciton)
    //so I use a list to store them
    //temp: remember to delete it and its all strings at game quit
    static std::list<std::string*> usedStrings;

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
