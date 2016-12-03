#ifndef __INTERFACE_SCENE_H___
#define __INTERFACE_SCENE_H___

#include "scene.h"
#include <string>

class InterfaceScene: public steg::Scene
{

public:
    InterfaceScene(int mapWidthNum, int mapHeightNum, SDL_Renderer* render, const char* name);

protected:
    std::string name;   //lua load scripts according to this name

};

#endif // __INTERFACE_SCENE_H___
