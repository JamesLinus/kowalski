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
#ifndef KWL_DECODER_IMAADPCM_H
#define KWL_DECODER_IMAADPCM_H

/*! \file */ 

#include "kwl_decoder.h"
#include "kwl_inputstream.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * A struct encapsulating the state of an IMA ADPCM (aka DVI ADPCM or IMA4) decoder.
 * @see http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/Docs/RIFFNEW.pdf , page 30.
 */
typedef struct kwlIMAADPCMCodecData
{
    kwlAudioData adpcmDataDescription;
    /** The size in bytes of one datablock.*/
    int nBlockAlign;
    /** */
    int firstDataBlockByte;
    /** */
    int dataSize;
    /** */
    int currentByte;
    /** A buffer containing the current encoded datablock. */
    unsigned char* currentDatablock;
    
} kwlIMAADPCMCodecData;
    
    
static const int KWL_IMA_ADPCM_INDEX_TABLE[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
}; 

static const int KWL_IMA_ADPCM_STEP_TABLE[89] = { 
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 
};
    
kwlError kwlInitDecoderIMAADPCM(kwlDecoder* decoder);

void kwlDeinitDecoderIMAADPCM(kwlDecoder* decoder);
    
int kwlDecodeBufferIMAADPCM(kwlDecoder* decoder);
    
int kwlRewindDecoderIMAADPCM(kwlDecoder* decoder);

/** 
 * Decodes an IMA ADPCM nibble to a 16 bit pcm sample.
 */
static inline int decodeNibble(int nibble, int *predictor, int*stepIndex)
{
    int p = *predictor;
    int s = *stepIndex;
    
    /*compute a delta to add to the predictor value*/
    int diff = 0;
    if (nibble & 4)
    {
        diff += KWL_IMA_ADPCM_STEP_TABLE[s];
    }
    if (nibble & 2)
    {
        diff += KWL_IMA_ADPCM_STEP_TABLE[s] >> 1;
    }
    if (nibble & 1)
    {
        diff += KWL_IMA_ADPCM_STEP_TABLE[s] >> 2;
    }
    diff += KWL_IMA_ADPCM_STEP_TABLE[s] >> 3;
    
    if (nibble & 8)
    {
        diff = -diff;
    }
    
    /*add the delta and clamp the predictor*/
    p += diff;
    if (p > 32767)
    {   
        p = 32767;
    }
    else if (p < -32768) 
    {
        p = -32768; 
    }
    
    /*compute a new step index*/
    s += KWL_IMA_ADPCM_INDEX_TABLE[nibble];
    if (s > 88) 
    {
        s = 88;
    }
    else if (s < 0) 
    {
        s = 0;
    }
    
    *predictor = p;
    *stepIndex = s;
    
    return p;
}
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    


#endif /*KWL_DECODER_IMAADPCM_H*/

