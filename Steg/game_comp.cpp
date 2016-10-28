#include "game_comp.h"
#include "scene.h"
#include "log.h"
#include <cstring>
#include "gui.h"
#include "event_handler.h"

namespace steg
{

//*************************// GameComp //*************************//
GameComp::GameComp()
{
	motherScene = NULL;
	enable = true;
	updateErrored = false;
}

GameComp::~GameComp()
{

}

void GameComp::DisableComp()
{
	enable = false;
}

void GameComp::EnableComp()
{
	enable = true;
}

DBG_Status GameComp::Update(Uint32 deltTick)
{
	DBG_Status status = DBG_OK;

	return status;
}

DBG_Status GameComp::HandleEvent(SDL_Event event)
{
 	DBG_Status status = DBG_OK;

	return status;
}

void GameComp::ResetErrors()
{
	updateErrored = false;
}

DBG_Status GameComp::InitInScene(Scene *scene)
{
	DBG_Status status = DBG_OK;

	motherScene = scene;
	std::list<GameComp*>& sceneComps = scene->comps;

	bool alreadyInScene = false;
    for(std::list<GameComp*>::iterator comp = sceneComps.begin();
        comp != sceneComps.end();
        comp++)
    {
        if((*comp) == this)
        {
            alreadyInScene = true;
            break;
        }
    }
    if(alreadyInScene)
    {
        status |= DBG_REP_OPR;
        return status;
    }

    sceneComps.push_back(this);

	return status;
}

DBG_Status GameComp::DumpOutOfScene()
{
	DBG_Status status = DBG_OK;
	motherScene = NULL;
	return status;
}

//*************************// InputHandler //*************************//
InputHandler::InputHandler()
    :GameComp()
{
    keyNum = 0;
    tmpKeyState = SDL_GetKeyboardState(&keyNum);
    //该块内存，会一直都在，且由SDL自己更新，不需要每一帧都调用这个函数

    currentKeyState = new Uint8[keyNum];
    lastKeyState = new Uint8[keyNum];
    if(NULL == currentKeyState || NULL == lastKeyState)
    {
        ENG_LogError("Cannot malloc memory for \"Uint8*\"");
        currentKeyState = lastKeyState = NULL;
    }
    else
    {
        memcpy(currentKeyState, tmpKeyState, keyNum * sizeof(Uint8));
        memcpy(lastKeyState, tmpKeyState, keyNum * sizeof(Uint8));
    }

    mousePosition = SDL_Point{0, 0};
    lastMousePosition = SDL_Point{0, 0};
    currentMouseState = 0;
    lastMouseState = 0;
}

InputHandler::~InputHandler()
{
    if(currentKeyState)
        delete[] currentKeyState;
    currentKeyState = NULL;

    if(lastKeyState)
        delete[] lastKeyState;
    lastKeyState = NULL;
}

DBG_Status InputHandler::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    Uint8* tmpPointer = currentKeyState;
    currentKeyState = lastKeyState;
    lastKeyState = tmpPointer;
    memcpy(currentKeyState, tmpKeyState, keyNum * sizeof(Uint8));

    lastMouseState = currentMouseState;
    lastMousePosition = mousePosition;
    currentMouseState = SDL_GetMouseState(&(mousePosition.x), &(mousePosition.y));

    return status;
}

bool InputHandler::KeyDown(SDL_Scancode scanCode)
{
    if(NULL == currentKeyState)
        return false;

    return currentKeyState[scanCode];
}

bool InputHandler::KeyUp(SDL_Scancode scanCode)
{
    if(NULL == currentKeyState)
        return false;

    return !currentKeyState[scanCode];
}

bool InputHandler::KeyPressed(SDL_Scancode scanCode)
{
    if(NULL == currentKeyState || NULL == lastKeyState)
        return false;

    return !lastKeyState[scanCode] && currentKeyState[scanCode];
}

bool InputHandler::KeyReleased(SDL_Scancode scanCode)
{
    if(NULL == currentKeyState || NULL == lastKeyState)
        return false;

    return lastKeyState[scanCode] && !currentKeyState[scanCode];
}

SDL_Point InputHandler::GetMousePosition(int* x, int* y)
{
    if(x)
        *x = mousePosition.x;
    if(y)
        *y = mousePosition.y;

    return mousePosition;
}

SDL_Point InputHandler::GetMouseMove(int* x, int* y)
{
    SDL_Point mouseMove = SDL_Point{mousePosition.x - lastMousePosition.x,
                                    mousePosition.y - lastMousePosition.y};

    if(x)
        *x = mouseMove.x;
    if(y)
        *y = mouseMove.y;

    return mouseMove;
}

bool InputHandler::MouseLeftDown(int* x, int* y)
{
    if(x)
        *x = mousePosition.x;
    if(y)
        *y = mousePosition.y;

    if(currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
        return true;
    else
        return false;
}
bool InputHandler::MouseLeftPressed(int* x, int* y)
{
    if(x)
        *x = mousePosition.x;
    if(y)
        *y = mousePosition.y;

    if((currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
       !(lastMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)))
        return true;
    else
        return false;
}
bool InputHandler::MouseLeftReleased(int* x, int* y)
{
    if(x)
        *x = mousePosition.x;
    if(y)
        *y = mousePosition.y;

    if(!(currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
       (lastMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)))
        return true;
    else
        return false;
}
bool InputHandler::MouseLeftPressedHere(SDL_Rect here)
{
    return (currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
           !(lastMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
           SDL_PointInRect(&mousePosition, &here) == SDL_TRUE;
}
bool InputHandler::MouseLeftReleasedHere(SDL_Rect here)
{
    return !(currentMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
           (lastMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&
           SDL_PointInRect(&mousePosition, &here) == SDL_TRUE;
}

//*************************// GUIOperateHandler //*************************//
GUIOperateHandler::GUIOperateHandler(Scene* motherScene)
    :GameComp()
{
    this->motherScene = motherScene;
}

DBG_Status GUIOperateHandler::Update(Uint32 deltTick)
{
    DBG_Status status = DBG_OK;

    SDL_Rect absRect;
    SDL_Point mousePos;
    for(std::list<GUI*>::iterator g = motherScene->GUIComps.begin();
        g != motherScene->GUIComps.end();
        g++)
    {
        //cannot operate hidden gui
        if(!((*g)->IsSelectable()))
            continue;

        if(motherScene->mouseOccupiedGUIComp != NULL &&
           motherScene->mouseOccupiedGUIComp != (*g))
            continue;

        if(!((*g)->IsVisible()))
        {
            if(motherScene->selectedGUIComp == (*g))
                PushUnSelectGUICompEvent(*g);
            continue;
        }

        absRect = (*g)->GetAbsDestRect();
        mousePos = motherScene->inputHandler->GetMousePosition();
        if(SDL_PointInRect(&mousePos, &absRect) == SDL_TRUE &&
           motherScene->selectedGUIComp != (*g))
        {
            PushSelectGUICompEvent(*g);
        }
        else if(SDL_PointInRect(&mousePos, &absRect) == SDL_FALSE &&
                motherScene->selectedGUIComp == (*g))
        {
            PushUnSelectGUICompEvent(*g);
        }
    }

    //temp: need to improve
    if(motherScene->selectedGUIComp != NULL)
    {
        if(motherScene->inputHandler->MouseLeftPressed())
            PushPressedButtonEvent(motherScene->selectedGUIComp);
        else if(motherScene->inputHandler->MouseLeftReleased())
            PushReleasedButtonEvent(motherScene->selectedGUIComp);
    }

    return status;
}

}
