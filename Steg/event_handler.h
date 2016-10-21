//event_handler.h
//created by SteDeshain
//created on 2016/08/01
//latest updated on 2016/08/02
//simple event handler system for SteDeshain engine driven by SDL2.0

#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "status.h"
#include "log.h"

namespace steg
{

//Add custom event here
#define CUSTOM_GAMEEVENT		0
#define CUSTOM_CONTROLEVENT		1
#define CUSTOM_SCENEEVENT		2
#define CUSTOM_GUIEVENT         3
#define CUSTOM_EVENTS_CT		4

//>>************************* GAME EVENT *************************<<//
//enum event code
enum gameEventCode
{
	evcPauseGame,
	evcResumeGame,
};
//push event macro functions
//...


//>>************************ CONTROL EVENT ************************<<//
//enum event code
//push event macro functions


//>>************************ ENGINE  EVENT ************************<<//
//enum event code
enum sceneEventCode
{
    evcResortSolidObjs,
};
//push event macro functions
#define PushResortSolidObjsEvent(PSCENE)    \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_SCENEEVENT], steg::evcResortSolidObjs, PSCENE, NULL)


//>>************************ ENGINE  EVENT ************************<<//
//enum event code
enum guiEventCode
{
    evcSelectGUIComp,
    evcUnSelectGUIComp,
    evcPressedButton,
    evcReleasedButton,
    evcShowCanvas,
    evcHideCanvas,
};
//push event macro functions
#define PushSelectGUICompEvent(PCOMP)       \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_GUIEVENT], steg::evcSelectGUIComp, PCOMP, NULL)

#define PushUnSelectGUICompEvent(PCOMP)     \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_GUIEVENT], steg::evcUnSelectGUIComp, PCOMP, NULL)

#define PushPressedButtonEvent(PCOMP)       \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_GUIEVENT], steg::evcPressedButton, PCOMP, NULL)

#define PushReleasedButtonEvent(PCOMP)       \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_GUIEVENT], steg::evcReleasedButton, PCOMP, NULL)

#define PushShowCanvasEvent(PCANVAS)        \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_GUIEVENT], steg::evcShowCanvas, PCANVAS, NULL)

#define PushHideCanvasEvent(PCANVAS)        \
        steg::PushEvent(steg::CustomEventsType[CUSTOM_GUIEVENT], steg::evcHideCanvas, PCANVAS, NULL)

//...


extern Uint32 CustomEventsType[CUSTOM_EVENTS_CT];

DBG_Status EveInit();

void EveQuit();

DBG_Status PushEvent(Uint32 eventType, Uint32 eventCode, void *data1, void *data2);

}

#endif
