#include "game.h"
#include <iostream>
#include "SDL2/SDL_ttf.h"
#include "log.h"
#include "event_handler.h"
#include "color.h"
#include "collide_type.h"
#include "font.h"
#include "stel.h"

namespace steg
{

int FPS = 100;

SDL_Window* Game::mainWindow = NULL;
int Game::windowWidth = 800;
int Game::windowHeight = 480;
const char* Game::windowName = "Steg Engine from SteDeshain";
int Game::tileWidth = 64;

DBG_Status Game::EngineInit()
{
    //temp
	SDL_Init(SDL_INIT_EVERYTHING);
	//...
	LogInit();
	LuaInit();
	FontInit();
	EveInit();
	CollideInit();
}

DBG_Status Game::EngineQuit()
{
    CollideQuit();
	EveQuit();
	FontQuit();
	LuaQuit();
	LogQuit();
	//...
	SDL_Quit();
}

Game::Game()
{
	Game::mainWindow = SDL_CreateWindow(Game::windowName,
										SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										Game::windowWidth, Game::windowHeight,
										SDL_WINDOW_SHOWN);
	if(NULL == Game::mainWindow)
	{
		ENG_LogWarns("Cannot create window!", SDL_GetError());
		SDL_ClearError();
	}
}

Game::~Game()
{
	if(Game::mainWindow)
	{
		SDL_DestroyWindow(Game::mainWindow);
	}
}

DBG_Status Game::Run()
{
	DBG_Status status = DBG_OK;

	status |= InitGame();

    Uint32 startTime = 0;
    Uint32 endTime = 0;
	bool quit = false;
    SDL_Event event;
    while(!quit)
    {
    	startTime = SDL_GetTicks();

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
		UpdatePhysics(reminingTicks);

//		Draw();

		endTime = SDL_GetTicks();
        if(endTime - startTime < TICKS_PER_FRAME)
        {
            SDL_Delay(TICKS_PER_FRAME - (endTime - startTime));
        }
    }

	return status;
}

DBG_Status Game::HandleEvent(SDL_Event event)
{
	DBG_Status status = DBG_OK;

	status |= (scenes.top())->HandleEvent(event);

	return status;
}

DBG_Status Game::HandleInput()
{
	DBG_Status status = DBG_OK;

	status |= (scenes.top())->HandleInput();

	return status;
}

DBG_Status Game::Update()
{
	DBG_Status status = DBG_OK;
	deltUpdateTick = SDL_GetTicks() - lastUpdateTick;
	lastUpdateTick = SDL_GetTicks();

	status |= (scenes.top())->Update(deltUpdateTick);

	return status;
}

DBG_Status Game::UpdatePhysics(Uint32 reminingTick)
{
    DBG_Status status = DBG_OK;

    status |= (scenes.top())->UpdatePhysics(reminingTick);

    return status;
}

DBG_Status Game::Draw()
{
	DBG_Status status = DBG_OK;
	deltDrawTick = SDL_GetTicks() - lastDrawTick;
	lastDrawTick = SDL_GetTicks();

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor((scenes.top())->render, &r, &g, &b, &a);
	SDL_SetRenderDrawColor((scenes.top())->render, GET_R(LightGray), GET_G(LightGray), GET_B(LightGray), 0xFF);
	SDL_RenderClear((scenes.top())->render);
	SDL_SetRenderDrawColor((scenes.top())->render, r, g, b, a);

	status |= (scenes.top())->Draw(deltDrawTick);
	status |= (scenes.top())->DrawExtra(deltDrawTick);
	SDL_RenderPresent((scenes.top())->render);

	return status;
}

}
