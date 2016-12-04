#ifndef __INTERFACE_SCENE_H___
#define __INTERFACE_SCENE_H___

#include "scene.h"
#include "interface_comp.h"
#include <string>

namespace interface
{

class InterfaceScene: public steg::Scene, public Name
{

public:
    InterfaceScene(const char* name, int mapWidthNum, int mapHeightNum, SDL_Renderer* render);

protected:
    //std::string name;   //lua load scripts according to this name
    std::list<std::string> stringList;  //stores strings used by scripts(get from lua stack)

};

}

#endif // __INTERFACE_SCENE_H___
