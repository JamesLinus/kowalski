/*
Copyright (c) 2010-2012 Per Gantelius

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifndef KWL__DECODER_H
#define KWL__DECODER_H

/*! \file */ 

#include "kowalski.h"
#include "kwl_audiodata.h"
#include "kwl_eventinstance.h"
#include "kwl_synchronization.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
/** An audio decoder. */
typedef struct kwlDecoder
{
    /** */
    kwlThread decodingThread;
    /** */
    kwlSemaphore* semaphore;
    /** The unique name of this decoder's semaphore.*/
    char* semaphoreName[256];
    /** */
    int isDecoding;
    /** An input stream providing the decoder with data.*/
    kwlInputStream audioDataStream;
    /** A buffer of the most recently decoded samples (interleaved, 16 bit).*/
    short* currentDecodedBuffer;
    short* currentDecodedBufferFront;
    int threadJoinRequested;
    /** */
    int loop;
    /** The number of decoded bytes in the temporary buffer.*/
    int currentDecodedBufferSizeInBytes;    
    /** In bytes. */
    int maxDecodedBufferSize;    
    /** The number of decoded audio channels.*/
    int numChannels;
    /** Codec specific state data.*/
    void* codecData;
    /** A codec specific callback that fills the decoder's buffer of decoded samples. */
    int (*decodeBuffer)(struct kwlDecoder* decoder);
    /** A pointer to a codec specific cleanup function. */
    void (*deinit)(struct kwlDecoder* decoder);
    /** 
     * A pointer to a codec specific method that rewinds the decoder 
     * to the start of the audio stream. A non-zero return value indicates success.
     */
    int (*rewind)(struct kwlDecoder* decoder);
} kwlDecoder;

/** 
 * Initializes a given decoder instance. The decoder type is determined by the encoding of the 
 * audio data provided.
 * @param decoder
 * @param audioData
 */
kwlError kwlDecoder_init(kwlDecoder* decoder, struct kwlEventInstance* event);
    
void kwlDecoder_deinit(kwlDecoder* decoder);

void* kwlDecoder_decodingLoop(void*);
    
int kwlDecoder_decodeNewBufferForEvent(kwlDecoder* decoder, struct kwlEventInstance* event);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__DECODER_H*/
