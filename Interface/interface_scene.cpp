#include "interface_scene.h"

namespace interface
{

InterfaceScene::InterfaceScene(const char* name, int mapWidthNum, int mapHeightNum, SDL_Renderer* render)
    :steg::Scene(mapWidthNum, mapHeightNum, render)
{
    this->name = name;
}

}
