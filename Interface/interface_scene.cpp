#include "interface_scene.h"

InterfaceScene::InterfaceScene(int mapWidthNum, int mapHeightNum, SDL_Renderer* render, const char* name)
    :steg::Scene(mapWidthNum, mapHeightNum, render), name(name)
{
}
