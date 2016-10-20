#include "log.h"

namespace steg
{

static FILE *LogFile;

static const char *(LogCateString[LOG_LOGCATES_CT]) =
{
    [SDL_LOG_CATEGORY_APPLICATION] = "application module",
    [SDL_LOG_CATEGORY_ERROR] = "error module",
    [SDL_LOG_CATEGORY_ASSERT] = "assert module",
    [SDL_LOG_CATEGORY_SYSTEM] = "system module",
    [SDL_LOG_CATEGORY_AUDIO] = "audio module",
    [SDL_LOG_CATEGORY_VIDEO] = "video module",
    [SDL_LOG_CATEGORY_RENDER] = "render module",
    [SDL_LOG_CATEGORY_INPUT] = "input module",
    [SDL_LOG_CATEGORY_TEST] = "test module",

    [SDL_LOG_CATEGORY_RESERVED1] = "",
    [SDL_LOG_CATEGORY_RESERVED2] = "",
    [SDL_LOG_CATEGORY_RESERVED3] = "",
    [SDL_LOG_CATEGORY_RESERVED4] = "",
    [SDL_LOG_CATEGORY_RESERVED5] = "",
    [SDL_LOG_CATEGORY_RESERVED6] = "",
    [SDL_LOG_CATEGORY_RESERVED7] = "",
    [SDL_LOG_CATEGORY_RESERVED8] = "",
    [SDL_LOG_CATEGORY_RESERVED9] = "",
    [SDL_LOG_CATEGORY_RESERVED10] = "",

	//Add log cates here
    [ENG_LOG_CATEGORY] = "engine module",
    [LUA_LOG_CATEGORY] = "lua module",
    [GAME_LOG_CATEGORY] = "game module"
};

static const char *(LogPrioString[LOG_LOGPRIOS_CT]) =
{
    [0] = "",
    [SDL_LOG_PRIORITY_VERBOSE] = "verbos",
    [SDL_LOG_PRIORITY_DEBUG] = "debug",
    [SDL_LOG_PRIORITY_INFO] = "info",
    [SDL_LOG_PRIORITY_WARN] = "warn",
    [SDL_LOG_PRIORITY_ERROR] = "error",
    [SDL_LOG_PRIORITY_CRITICAL] = "critical",
};

static char *TimeStringProc(char *time)
{
    SDL_assert(time != NULL);

    int i = 0;
    while(time[i] != '\0')
    {
        if(time[i] == '\n')
        {
            time[i] = '\0';
            return time;
        }
        i++;
    }
    return time;
}

DBG_Status LogInit()
{
    LogFile = fopen(LOG_FILENAME, "a+");
    if(NULL == LogFile)
        return DBG_NULL_PTR | DBG_FILE_ERR;

    time_t now = time(NULL);
    fprintf(LogFile,
            "\n>> ******************************* logs on %s ******************************* <<\n",
            TimeStringProc(ctime(&now)));

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);		//to make all log_cats working whose priority is bigger than info!

    SDL_LogSetOutputFunction(LogOutPut, NULL);

    return DBG_OK;
}

void LogOutPut(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    SDL_assert(NULL != LogFile);

    time_t now = time(NULL);
    char *timeString = TimeStringProc(ctime(&now));
    if(priority >= SDL_LOG_PRIORITY_WARN)			//CRITICAL is bigger than WARN
    {
        fprintf(LogFile, "(%s)An Error occured in \"%s\". Info: %s\n",
                timeString,
                LogCateString[category],
                message);
#ifdef DEBUG
        fprintf(stdout, "(%s)An Error occured in \"%s\". Info: %s\n",
                timeString,
                LogCateString[category],
                message);
#endif
    }
    else
    {
        fprintf(LogFile, "(%s)Info: %s (In : \"%s\" by priority: %s)\n",
                timeString,
                message,
                LogCateString[category],
                LogPrioString[priority]);
#ifdef DEBUG
        fprintf(stdout, "(%s)Info: %s (In : \"%s\" by priority: %s)\n",
                timeString,
                message,
                LogCateString[category],
                LogPrioString[priority]);
#endif
    }
    fflush(LogFile);
}

void LogQuit()
{
    fprintf(LogFile, "\n");

    fclose(LogFile);
}

}
