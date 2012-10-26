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

#ifndef KWL_DECODER_OGGVORBIS_H
#define KWL_DECODER_OGGVORBIS_H

/*! \file */ 

#include "kwl_decoder.h"

#include "ivorbisfile.h"
#include "ivorbiscodec.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//TODO: set properly
static const int KWL_OGG_NUM_BUFFERED_FRAMES = 4096 << 5;

/** 
 * A struct encapsulating the state of an Ogg Vorbis (http://www.vorbis.com/faq/ ) stream decoder. 
 * The tremor library (http://wiki.xiph.org/index.php/Tremor ) is used to do the 
 * actual decoding.
 */
typedef struct
{
    /** The Ogg Vorbis file providing encoded data.*/
    OggVorbis_File oggVorbisFile;
} kwlOggVorbisDecoderData;

/** 
 * Initializes a given ogg vorbis decoder.
 * @param decoder The decoder to initialize.
 * @return \c KWL_ERROR_DECODING_AUDIO_DATA if the audio data stream of the decoder is
 * not a valid ogg vorbis stream, \c KWL_NO_ERROR otherwise.
 */
kwlError kwlInitDecoderOggVorbis(kwlDecoder* decoder);

/** 
 * Deinitializes a given ogg vorbis decoder, releasing all associated resoures.
 * @param decoder The decoder to deinitialize.
 */
void kwlDeinitDecoderOggVorbis(kwlDecoder* decoder);
    
/** 
 * Fills the output buffer of the decoder with new decoded samples.
 * @param decoder The decoder to use.
 * @return Non-zero if the end of the stream is reached, zero otherwise.
 */
int kwlDecodeBufferOggVorbis(kwlDecoder* decoder);
    

/** 
 * A callback used by \c ov_read to read bytes from the kwlInputStream providing ogg vorbis data.
 * @param ptr A pointer to the buffer to read to.
 * @param size The size in bytes of each element to read.
 * @param nmemb The number of elements to read.
 * @param datasource A pointer to the kwlInputStream providing ogg vorbis data. 
 * @return 
 */
size_t ovReadCallback(void *ptr, size_t size, size_t nmemb, void *datasource);

/** 
 * A callback used by \c ov_read to seek in the kwlInputStream providing ogg vorbis data.
 * @param datasource A pointer to the kwlInputStream providing ogg vorbis data. 
 * @param offset The seek offset.
 * @param whence The seek origin: \c SEEK_CUR, \c SEEK_SET or \c SEEK_END.
 * @return 
 */
int ovSeekCallback(void *datasource, ogg_int64_t offset, int whence);

/** 
 * A callback used by \c ov_read to close the kwlInputStream providing ogg vorbis data.
 * @param datasource A pointer to the kwlInputStream providing ogg vorbis data. 
 * @return 
 */
int ovCloseCallback(void *datasource);

/** 
 * A callback used by \c ov_read to get the current position in the kwlInputStream providing ogg vorbis data.
 * @param datasource A pointer to the kwlInputStream providing ogg vorbis data. 
 * @return 
 */
long ovTellCallback(void *datasource);

int kwlRewindDecoderOggVorbis(kwlDecoder* decoder);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */
        
#endif /*KWL_DECODER_OGGVORBIS_H*/
