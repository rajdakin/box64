/*******************************************************************
 * File automatically generated by rebuild_wrappers.py (v2.0.1.14) *
 *******************************************************************/
#ifndef __wrappedflacTYPES_H_
#define __wrappedflacTYPES_H_

#ifndef LIBNAME
#error You should only #include this file inside a wrapped*.c file
#endif
#ifndef ADDED_FUNCTIONS
#define ADDED_FUNCTIONS() 
#endif

typedef int64_t (*iFppPPPPPP_t)(void*, void*, void*, void*, void*, void*, void*, void*);
typedef int64_t (*iFppppppppp_t)(void*, void*, void*, void*, void*, void*, void*, void*, void*);

#define SUPER() ADDED_FUNCTIONS() \
	GO(FLAC__metadata_chain_read_with_callbacks, iFppPPPPPP_t) \
	GO(FLAC__stream_decoder_init_stream, iFppppppppp_t)

#endif // __wrappedflacTYPES_H_
