/*******************************************************************
 * File automatically generated by rebuild_wrappers.py (v2.4.0.20) *
 *******************************************************************/
#ifndef __wrappedlibcupsTYPES_H_
#define __wrappedlibcupsTYPES_H_

#ifndef LIBNAME
#error You should only #include this file inside a wrapped*.c file
#endif
#ifndef ADDED_FUNCTIONS
#define ADDED_FUNCTIONS() 
#endif

typedef int32_t (*iFuipuupp_t)(uint32_t, int32_t, void*, uint32_t, uint32_t, void*, void*);

#define SUPER() ADDED_FUNCTIONS() \
	GO(cupsEnumDests, iFuipuupp_t)

#endif // __wrappedlibcupsTYPES_H_
