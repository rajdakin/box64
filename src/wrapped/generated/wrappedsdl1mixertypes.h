/*******************************************************************
 * File automatically generated by rebuild_wrappers.py (v2.0.1.14) *
 *******************************************************************/
#ifndef __wrappedsdl1mixerTYPES_H_
#define __wrappedsdl1mixerTYPES_H_

#ifndef LIBNAME
#error You should only #include this file inside a wrapped*.c file
#endif
#ifndef ADDED_FUNCTIONS
#define ADDED_FUNCTIONS() 
#endif

typedef void (*vFp_t)(void*);
typedef void* (*pFp_t)(void*);
typedef void (*vFpp_t)(void*, void*);
typedef int64_t (*iFip_t)(int64_t, void*);
typedef void* (*pFpi_t)(void*, int64_t);
typedef void* (*pFpii_t)(void*, int64_t, int64_t);
typedef int64_t (*iFippp_t)(int64_t, void*, void*, void*);

#define SUPER() ADDED_FUNCTIONS() \
	GO(Mix_ChannelFinished, vFp_t) \
	GO(Mix_HookMusicFinished, vFp_t) \
	GO(Mix_LoadMUS_RW, pFp_t) \
	GO(Mix_HookMusic, vFpp_t) \
	GO(Mix_SetPostMix, vFpp_t) \
	GO(Mix_UnregisterEffect, iFip_t) \
	GO(Mix_LoadWAV_RW, pFpi_t) \
	GO(Mix_LoadMUSType_RW, pFpii_t) \
	GO(Mix_RegisterEffect, iFippp_t)

#endif // __wrappedsdl1mixerTYPES_H_
