#include "interface_game.h"
#include "scene1.h"
#include "color.h"
#include "interface_scene.h"
#include "gui_scene.h"
//#include <iostream>

namespace interface
{

using namespace steg;

InterfaceGame::~InterfaceGame()
{
    delete conf;
}

DBG_Status InterfaceGame::InitGame()
{
    DBG_Status status = DBG_OK;

    PLuaReadData_J("config.lua",
                   "Config|window|width&Config|window|height&Config|window|caption&Config|FPS&Config|tileWidth",
                   &conf);

//    conf->PrintToStanderdOut();
    Game::windowWidth = *((double*)((*conf)[0]->value));
    Game::windowHeight = *((double*)((*conf)[1]->value));
    Game::windowName = ((std::string*)((*conf)[2]->value))->c_str();
    Game::FPS = *((double*)((*conf)[3]->value));
    Game::tileWidth = *((double*)((*conf)[4]->value));

    status |= Game::InitGame(); //create window

    debugFont = TTF_OpenFont("calibri.ttf", 12);
    render = SDL_CreateRenderer(Game::mainWindow, -1, SDL_RENDERER_ACCELERATED);

    InterfaceScene* welcome;
    welcome = new GuiScene("welcomeScene_start", render);
    welcome->InitScene();

//    testScene = new Scene1(15, 10, render);
//    testScene->debugFont = this->debugFont;
//    testScene->InitScene();
//    fps = new Label(0, 0, LeftTop, "hello world", debugFont, GetColor(Black), DebugUI, 0);
//    *testScene << fps;

//    scenes.push(testScene);
    scenes.push(welcome);
//    scenes.push(testScene);

    return status;
}

DBG_Status InterfaceGame::Run()
{
	DBG_Status status = DBG_OK;

	status |= InitGame();

    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 lastEnd = 0;
	bool quit = false;
    SDL_Event event;
    while(!quit)
    {
    	startTime = SDL_GetTicks();

//		Draw();

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                quit = true;

			HandleEvent(event);
        }

		Draw();

		HandleInput();

		Update();

		//temp
		Uint32 takingTicks = SDL_GetTicks() - startTime;
		Uint32 reminingTicks;
		if(takingTicks > TICKS_PER_FRAME)
            reminingTicks = 0;
        else
            reminingTicks = TICKS_PER_FRAME - takingTicks;
//		UpdatePhysics(reminingTicks);
		UpdatePhysics(0);

//		Draw();

//		sprintf(buffer, "fps: %3.2f(%d)", 1000 / (float)(SDL_GetTicks() - endTime), FPS);
//		fps->ReloadTexture(buffer);
		endTime = SDL_GetTicks();
        if(endTime - startTime < TICKS_PER_FRAME)
        {
            lastEnd = endTime;
            SDL_Delay(TICKS_PER_FRAME - (endTime - startTime));
        }
    }

	return status;
}

}
