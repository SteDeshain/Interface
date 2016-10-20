//status.h
//created by SteDeshain
//created on 2016/08/01
//latest updated on 2016/08/01
//function return STATUS value to observe the status of the function running

#ifndef __STATUS_H__
#define __STATUS_H__

#define DBG_Status          Uint32

#define DBG_OK              0x00000000
#define DBG_NULL_PTR        0x00000001
#define DBG_ARG_ERR         0x00000002
#define DBG_OUT_OF_RANGE    0x00000004
#define DBG_MEM_ERR         0x00000008				//memory error
#define DBG_FILE_ERR        0x00000010
#define DBG_REP_OPR         0x00000020              //repeat operation
//...
#define DBG_SDL_ERR         0x00800000
#define DBG_GFX_ERR     	0x01000000
#define DBG_MIX_ERR         0x02000000
#define DBG_IMG_ERR         0x04000000
#define DBG_TTF_ERR         0x08000000
#define DBG_NET_ERR         0x10000000
#define DBG_GAME_ERR       	0x20000000
#define DBG_LUA_ERR         0x40000000
#define DBG_FATAL_ERR       0x80000000

#endif
