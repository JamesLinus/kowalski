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

#include "kwl_assert.h"
#include "kwl_audiodata.h"
#include "kwl_decoder.h"
#include "kwl_decoder_imaadpcm.h"
#include "kwl_decoder_pcm.h"
#ifdef KWL_IPHONE
#include "kwl_decoder_iphone.h"
#endif /*KWL_IPHONE*/
#include "kwl_decoder_oggvorbis.h"
#include "kwl_memory.h"

void kwlDecoder_swapBuffers(kwlDecoder* decoder)
{
    short* temp = decoder->currentDecodedBufferFront;
    decoder->currentDecodedBufferFront = decoder->currentDecodedBuffer;
    decoder->currentDecodedBuffer = temp;
}

kwlError kwlDecoder_init(kwlDecoder* decoder, kwlEventInstance* event)
{
    kwlAudioData* audioData = event->definition_engine->streamAudioData;
    /*reset the decoder struct.*/
    kwlMemset(decoder, 0, sizeof(kwlDecoder));
    
    decoder->loop = event->definition_engine->loopIfStreaming;
    
    /*
     * Hook up audio data, that could either be from a file or from an already loaded buffer
     */
    if (audioData->streamFromDisk != 0)
    {
        KWL_ASSERT(audioData->fileOffset >= 0);
        kwlError result = kwlInputStream_initWithFileRegion(&decoder->audioDataStream,
                                                            audioData->waveBank->waveBankFilePath,
                                                            audioData->fileOffset,
                                                            audioData->numBytes);
        KWL_ASSERT(result == KWL_NO_ERROR);
    }
    else
    {   
        kwlInputStream_initWithBuffer(&decoder->audioDataStream,
                                      audioData->bytes,
                                      0, 
                                      audioData->numBytes);
    }
    
    /*
     * do codec specific initialization.
     */
    kwlError result = KWL_UNSUPPORTED_ENCODING;
    
    if (audioData->encoding == KWL_ENCODING_IMA_ADPCM)
    {
        result = kwlInitDecoderIMAADPCM(decoder);
    }
    else if (audioData->encoding == KWL_ENCODING_VORBIS)
    {
        result = kwlInitDecoderOggVorbis(decoder);
    }
    else if (audioData->encoding == KWL_ENCODING_UNSIGNED_8BIT_PCM ||
             audioData->encoding == KWL_ENCODING_SIGNED_8BIT_PCM ||
             audioData->encoding == KWL_ENCODING_SIGNED_16BIT_PCM ||
             audioData->encoding == KWL_ENCODING_SIGNED_24BIT_PCM ||
             audioData->encoding == KWL_ENCODING_SIGNED_32BIT_PCM)
    {
        result = kwlInitDecoderPCM(decoder);
    }
    #ifdef KWL_IPHONE
    else if (audioData->encoding == KWL_ENCODING_AAC ||
             audioData->encoding == KWL_ENCODING_UNKNOWN)
    {
        result = kwlInitDecoderIPhone(decoder);
    }
    #endif /*KWL_IPHONE*/
    
    if (result != KWL_NO_ERROR)
    {
        //TODO: deinit gracefully
    }
    
    decoder->currentDecodedBuffer = 
        (short*)KWL_MALLOC(sizeof(short) * decoder->maxDecodedBufferSize, "decoder back buffer");
    decoder->currentDecodedBufferFront = 
        (short*)KWL_MALLOC(sizeof(short) * decoder->maxDecodedBufferSize, "decoder front buffer");
    decoder->currentDecodedBufferSizeInBytes = 0;
    
    /*
     * Before starting the decoding thread, call the decode function 
     * synchronously to get the first buffer of decoded samples.
     */
    int decodingResult = decoder->decodeBuffer(decoder);
    
    kwlDecoder_swapBuffers(decoder);
    
    /*TODO: check the decoding result. the event could be done playing here.*/
    event->currentPCMFrameIndex = 0;
    event->currentPCMBuffer = decoder->currentDecodedBufferFront;
    event->currentPCMBufferSize = decoder->currentDecodedBufferSizeInBytes / (2 * decoder->numChannels);
    
    event->currentNumChannels = decoder->numChannels;
    
    
    /*Create a semaphore with a unique name based on the addess of the decoder*/
    sprintf(decoder->semaphoreName, "decoder%d", (int)decoder);
    decoder->semaphore = kwlSemaphoreOpen(decoder->semaphoreName);
    kwlSemaphorePost(decoder->semaphore);
    
    /*Fire up the decoding thread.*/
    kwlThreadCreate(&decoder->decodingThread, kwlDecoder_decodingLoop, decoder);
    
    return result;
}

void kwlDecoder_deinit(kwlDecoder* decoder)
{
    /*Shut down the decoder.*/
    decoder->threadJoinRequested = 1;
    kwlSemaphorePost(decoder->semaphore);
    kwlThreadJoin(&decoder->decodingThread);
    
    /*Dispose of the semaphore.*/
    kwlSemaphoreDestroy(decoder->semaphore, decoder->semaphoreName);
    
    /*Free back and front buffers.*/
    KWL_FREE(decoder->currentDecodedBuffer);
    KWL_FREE(decoder->currentDecodedBufferFront);
    
    /*Close input stream*/
    kwlInputStream_close(&decoder->audioDataStream);
    
    /*Codec specific cleanup.*/
    decoder->deinit(decoder);
    
    decoder->codecData = NULL;
}

void* kwlDecoder_decodingLoop(void* data)
{
    kwlDecoder* decoder = (kwlDecoder*)data;
    KWL_ASSERT(decoder->numChannels > 0);
    
    while (1)
    {   
        kwlSemaphoreWait(decoder->semaphore);

        if (decoder->threadJoinRequested != 0)
        {
            /*if stop is requested from outside the thread*/
            return NULL;
        }
        decoder->isDecoding = 1;
        int endOfData = decoder->decodeBuffer(decoder);
        
        kwlDecoder_swapBuffers(decoder);
        
        decoder->isDecoding = 0;
        
        if (endOfData != 0)
        {
            if (decoder->loop == 0)
            {
                decoder->threadJoinRequested = 1;
            }
            else
            {
                /*This is a looping decoder. Try to rewind the stream*/
                int rewindResult = decoder->rewind(decoder);
                if (rewindResult == 0)
                {
                    /*rewind failed, stop playing*/
                    decoder->threadJoinRequested = 1;
                }
            }
        }
        
        //printf("decoded buffer, result %d. waiting for semaphore...\n", decoder->threadJoinRequested);
        if (decoder->threadJoinRequested != 0)
        {
            /*if the end of the audio data was reached*/
            return NULL;
        }
    }
    
    return NULL;
}

int kwlDecoder_decodeNewBufferForEvent(kwlDecoder* decoder, kwlEventInstance* event)
{
    if (decoder->isDecoding)
    {
        printf("still decoding, missed buffer!\n");
        return 0;
    }

    KWL_ASSERT(decoder->numChannels > 0);
    
    event->currentPCMBuffer = decoder->currentDecodedBufferFront;
    event->currentPCMFrameIndex = event->currentPCMFrameIndex - event->currentPCMBufferSize;
    KWL_ASSERT(event->currentPCMFrameIndex >= 0); /*Could be greater than zero for events with non-unit pitch*/
    
    event->currentPCMBufferSize = decoder->currentDecodedBufferSizeInBytes / (2 * decoder->numChannels);
    event->currentNumChannels = decoder->numChannels;
    
    kwlSemaphorePost(decoder->semaphore);
    //printf("assigned front buffer %d\n", (int)decoder->currentDecodedBufferFront);
    return decoder->threadJoinRequested; //TODO: proper return value
}
