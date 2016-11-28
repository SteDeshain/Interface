//game.h
//created by SteDeshain
//created on 2016/08/02
//latest updated on 2016/08/03
//Game class to run a game for SteDeshain engine driven by SDL2.0

#ifndef __GAME_H__
#define __GAME_H__

#include <stack>
#include "SDL2/SDL.h"
#include "status.h"
#include "scene.h"
#include "log.h"
#include "config.h"

//#define FPS                 100
#define TICKS_PER_FRAME     (1000 / steg::FPS)

namespace steg
{

extern int FPS;

class Game
{
public:
	Game();
	virtual ~Game();

    virtual DBG_Status InitGame() = 0;  //init game sources and scenes etc.

	virtual DBG_Status Run();

	static SDL_Window* mainWindow;
	static int windowWidth, windowHeight;
	static const char* windowName;
	static int tileWidth;

    static DBG_Status EngineInit();     //init the whole system
    static DBG_Status EngineQuit();     //shut down the whole system

protected:
	virtual DBG_Status HandleEvent(SDL_Event event);
    virtual DBG_Status HandleInput();
	virtual DBG_Status Update();
	virtual DBG_Status UpdatePhysics(Uint32 reminingTick);
	virtual DBG_Status Draw();

	std::stack<Scene*> scenes;
//	Scene* currentScene;
	Uint32 lastUpdateTick, deltUpdateTick;
	Uint32 lastDrawTick, deltDrawTick;

private:

};

}

#endif
