#include "game1.h"
#include "scene1.h"
#include "color.h"
//#include "vector3.h"

namespace gameTest
{

using namespace steg;

DBG_Status Game1::InitGame()
{
    debugFont = TTF_OpenFont("calibri.ttf", 12);
    render = SDL_CreateRenderer(Game::mainWindow, -1, SDL_RENDERER_ACCELERATED);
    testScene = new Scene1(15, 10, render);
    testScene->debugFont = this->debugFont;
    testScene->InitScene();
    fps = new Label(0, 0, LeftTop, "hello world", debugFont, GetColor(Black), DebugUI, 0);
    *testScene << fps;

    scenes.push(testScene);
}

DBG_Status Game1::Run()
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

		HandleInput();

		Update();

		UpdatePhysics();

		Draw();

		sprintf(buffer, "fps: %4.2f(%d)", 1000 / (float)(SDL_GetTicks() - endTime), FPS);
		fps->ReloadTexture(buffer);
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
