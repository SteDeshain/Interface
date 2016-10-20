#ifndef __GAME_COMP_H__
#define __GAME_COMP_H__

#include "SDL2/SDL.h"
#include "status.h"

namespace steg
{

class Scene;

class GameComp
{
	friend class Scene;

public:
	GameComp();
	virtual ~GameComp();

	void DisableComp();
	void EnableComp();

protected:
	Scene *motherScene;
	bool enable;			//whether this comp be updated in Scene::Update()
	bool updateErrored;		//to set or reset it to keep it not to log errors in every update frame

	virtual DBG_Status Update(Uint32 deltTick);
	virtual DBG_Status HandleEvent(SDL_Event event);

	virtual void ResetErrors();
	virtual DBG_Status InitInScene(Scene *scene);	//called after inserted into a Scene
	virtual DBG_Status DumpOutOfScene();			//called after throwed out by a Scene

private:
};

class InputHandler: public GameComp
{
    friend class Scene;

public:
    InputHandler();
    virtual ~InputHandler();

    bool KeyDown(SDL_Scancode scanCode);
    bool KeyUp(SDL_Scancode scanCode);
    bool KeyPressed(SDL_Scancode scanCode);
    bool KeyReleased(SDL_Scancode scanCode);

    SDL_Point GetMousePosition(int* x = NULL, int* y = NULL);

    bool MouseLeftDown(int* x = NULL, int* y = NULL);
    bool MouseLeftPressed(int* x = NULL, int* y = NULL);
    bool MouseLeftReleased(int* x = NULL, int* y = NULL);
    bool MouseLeftPressedHere(SDL_Rect here);
    bool MouseLeftReleasedHere(SDL_Rect here);

    bool MouseMiddleDown(int* x = NULL, int* y = NULL);

    bool MouseRightDown(int* x = NULL, int* y = NULL);

    virtual DBG_Status Update(Uint32 deltTick);     //make it public to be accessable to everyone...

protected:

private:
    const Uint8* tmpKeyState;    //get keyState from SDL, no need to alloc memory for it
    Uint8* currentKeyState;      //memcpy from keyState to currentKeyState in this frame, need to alloc memory for it
    Uint8* lastKeyState;         //memcpy from keyState to lastKeyState in last frame, need to alloc memory for it
    int keyNum;

    SDL_Point mousePosition;
    Uint32 currentMouseState;
    Uint32 lastMouseState;

};

class GUIOperateHandler: public GameComp
{

public:
    GUIOperateHandler(Scene* motherScene);

    virtual DBG_Status Update(Uint32 deltTick);

};

}

#endif	//__GAME_COMP_H__
