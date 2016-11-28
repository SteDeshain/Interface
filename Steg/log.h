//log.h
//created by SteDeshain
//created on 2016/08/01
//latest updated on 2016/08/02
//simple log system in SteDeshain game engine driven by SDL2.0
//output to a file named by "err.log", and at the same time output to stderr when defined DEBUG

#ifndef __LOG_H__
#define __LOG_H__

#include <time.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_log.h"
#include "status.h"

//Add log cates here
#define LOG_LOGCATES_CT			22
#define LOG_LOGPRIOS_CT			7

#define LOG_FILENAME			"err.log"

#define ENG_LogError(MESSAGE)                                    	\
        SDL_LogCritical(ENG_LOG_CATEGORY,			        		\
                        "%s (File: %s, Function: %s, Line: %d)",	\
                        MESSAGE,				        			\
                        __FILE__,				        			\
                        __FUNCTION__,								\
                        __LINE__)

#define ENG_LogErrors(MESSAGE1, MESSAGE2)							\
        SDL_LogCritical(ENG_LOG_CATEGORY,			        		\
                        "%s. %s (File: %s, Function: %s, Line: %d)",\
                        MESSAGE1,				        			\
                        MESSAGE2,				        			\
                        __FILE__,				        			\
                        __FUNCTION__,								\
                        __LINE__)

#define ENG_LogWarn(MESSAGE)										\
        SDL_LogWarn(ENG_LOG_CATEGORY,								\
                    "%s (File: %s, Function: %s, Line: %d)",		\
                    MESSAGE,										\
                    __FILE__,										\
                    __FUNCTION__,									\
                    __LINE__)

#define ENG_LogWarns(MESSAGE1, MESSAGE2)							\
		SDL_LogWarn(ENG_LOG_CATEGORY,								\
					"%s. %s (File: %s, Function: %s, Line: %d)",	\
					MESSAGE1,										\
					MESSAGE2,										\
					__FILE__,										\
					__FUNCTION__,									\
					__LINE__)

#define ENG_LogInfo(MESSAGE)										\
        SDL_LogInfo(ENG_LOG_CATEGORY,								\
                    MESSAGE)

#define LUA_LogError(MESSAGE)										\
        SDL_LogCritical(LUA_LOG_CATEGORY,							\
                        MESSAGE)

#define LUA_LogErrors(MESSAGE1, MESSAGE2)							\
        SDL_LogCritical(LUA_LOG_CATEGORY,			        		\
                        "%s. %s (File: %s, Function: %s, Line: %d)",\
                        MESSAGE1,				        			\
                        MESSAGE2,				        			\
                        __FILE__,				        			\
                        __FUNCTION__,								\
                        __LINE__)

#define GAME_LogError(MESSAGE)										\
        SDL_LogCritical(GAME_LOG_CATEGORY,							\
                        "%s (File: %s, Function: %s, Line: %d)",	\
                        MESSAGE,									\
                        __FILE__,									\
                        __FUNCTION__,								\
                        __LINE__)

#define GAME_LogInfo(MESSAGE)										\
        SDL_Log(MESSAGE)

//下列enum不能放在namespace，为了有全局作用域，和原生的SDL_LOG_CATEGORY行为一致
//况且有DBG_等前缀，也就不必放在namespace中以防止命名冲突了
enum
{
	//Add log cates here
    ENG_LOG_CATEGORY = SDL_LOG_CATEGORY_CUSTOM,     //ENG == engine
    LUA_LOG_CATEGORY,
    GAME_LOG_CATEGORY,
};

namespace steg
{

DBG_Status LogInit();

void LogQuit();			//To ensure the log system works fine, rember to quit the log system at last!

void LogOutPut(void *file, int category, SDL_LogPriority priority, const char *message);

}

#endif
