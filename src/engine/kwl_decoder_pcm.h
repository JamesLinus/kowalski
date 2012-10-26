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

#ifndef KWL_DECODER_PCM_H
#define KWL_DECODER_PCM_H

/*! \file */ 

#include "kwl_decoder.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * A struct encapsulating the state of a PCM stream decoder. 
 */
typedef struct
{
    /** PCM format metadata*/
    kwlAudioData pcmDataDescription;
    /** */
    char* scratchBuffer;
    int scratchBufferNumBytes;
    int bytesPerSample;
} kwlPCMDecoderData;
    
/** 
 * Initializes a given PCM decoder.
 * @param decoder The decoder to initialize.
 * @return \c KWL_ERROR_DECODING_AUDIO_DATA if the audio data stream of the decoder is
 * not a valid PCM stream, \c KWL_NO_ERROR otherwise.
 */
kwlError kwlInitDecoderPCM(kwlDecoder* decoder);

/** 
 * Deinitializes a given PCM decoder, releasing all associated resoures.
 * @param decoder The decoder to deinitialize.
 */
void kwlDeinitDecoderPCM(kwlDecoder* decoder);

/** 
 * Fills the output buffer of the decoder with new decoded samples.
 * @param decoder The decoder to use.
 * @return Non-zero if the end of the stream is reached, zero otherwise.
 */
int kwlDecodeBufferPCM(kwlDecoder* decoder);
    
int kwlRewindDecoderPCM(kwlDecoder* decoder);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*KWL_DECODER_PCM_H*/
