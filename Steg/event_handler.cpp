#include "event_handler.h"

namespace steg
{

Uint32 CustomEventsType[CUSTOM_EVENTS_CT];

DBG_Status EveInit()
{
	ENG_LogInfo("Event Handler system initializing...");

	//Obtine user_event code
	for(int i = 0; i < CUSTOM_EVENTS_CT; i++)
	{
		CustomEventsType[i] = SDL_RegisterEvents(1);

		if(((Uint32)-1) == CustomEventsType[i])
		{
			ENG_LogWarn("Cannot obtine the user_event code! Event Handler system initialize failure!");
			return DBG_FATAL_ERR | DBG_SDL_ERR;
		}
	}

	ENG_LogInfo("Event Handler system initialize successfully!");

	return DBG_OK;

}

void EveQuit()
{
	ENG_LogInfo("Event Handler system shutting down...");
	ENG_LogInfo("Event Handler system shut down successfully!");
}

DBG_Status PushEvent(Uint32 eventType, Uint32 eventCode, void *data1, void *data2)
{
	DBG_Status status = DBG_OK;

	SDL_Event *pEvent = (SDL_Event *)malloc(sizeof(SDL_Event));
    if(NULL == pEvent)
	{
		ENG_LogError("Cannot malloc a SDL_Event structure when attempting pushing an event!");
        return DBG_NULL_PTR | DBG_MEM_ERR;
	}

    pEvent->user.type = eventType;
    pEvent->user.code = eventCode;
    pEvent->user.data1 = data1;
    pEvent->user.data2 = data2;
    pEvent->user.timestamp = SDL_GetTicks();

    if(SDL_PushEvent(pEvent) < 0)
    {
        status |= DBG_SDL_ERR;
        ENG_LogError(SDL_GetError());
		SDL_ClearError();
    }

    free(pEvent);
    return status;
}

}
