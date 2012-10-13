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

#include "../../kwl_assert.h"
#include "../../kwl_engine.h"

#include "include/portaudio.h"

PaStream *stream;

/**
 * This method gets called by PortAudio when it's 
 * time to fill another output buffer.
 * @param inputBuffer
 * @param outputBuffer The buffer to write to.
 * @param framesPerBuffer The number of frames per buffer.
 * @param timeInfo 
 * @param statusFlags 
 * @param userData The user data passed to PortAudio during initialization. 
 *                 In this case a pointer to a kwlMixer struct.
 * @return Non-zero if an error occured, zero otherwise.
 */ 
static int paCallback(const void *inputBuffer, 
                      void *outputBuffer,
                      long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    /*Get a pointer to the Kowalski mixer responsible for producing
      final output buffers.*/
    kwlMixer *mixer = (kwlMixer*)userData;    
    
    /*Get the number of output channels.*/
    const int numOutChannels = mixer->numOutChannels;
    
    /*Fill the output buffer. For efficiency reasons, the 
      mixer has an internal maximum buffer size determined 
      by KWL_TEMP_BUFFER_SIZE_IN_FRAMES. If the size of the 
      requested output buffer exceeds the internal buffer 
      size, multiple calls to kwlMixer_render 
      are made. This is typically not the case.*/
    int currFrame = 0;
    while (currFrame < framesPerBuffer)
    {
        /*Compute the number of frames to mix.*/
        int numFramesToMix = framesPerBuffer - currFrame;
        if (numFramesToMix > KWL_TEMP_BUFFER_SIZE_IN_FRAMES)
        {
            numFramesToMix = KWL_TEMP_BUFFER_SIZE_IN_FRAMES;
        }
        
        /*Perform mixing.*/
        kwlMixer_render(mixer, 
                                          mixer->outBuffer, 
                                          numFramesToMix);
        
        /*Copy mixed samples to the output buffer*/
        int outIdx = currFrame * numOutChannels;
        
        kwlMemcpy(&((float*)outputBuffer)[outIdx], 
                  mixer->outBuffer, 
                  sizeof(float) * numOutChannels * numFramesToMix);
        
        kwlMixer_processInputBuffer(mixer, 
                                         &((float*)inputBuffer)[outIdx], 
                                         numFramesToMix);
        
        /*Increment write position*/
        currFrame += numFramesToMix;
    }
    
    /*Return 0 to indicate that everything went well.*/
    return 0;
}

/** 
 * Initializes PortAudio with a callback that fills output buffers mixed by the Kowalski Engine.
 * @param engine
 * @param sampleRate
 * @param numChannels
 * @param bufferSize
 * @return A Kowalski error code.
 */
kwlError kwlEngine_hostSpecificInitialize(kwlEngine* engine, int sampleRate, int numOutChannels, int numInChannels, int bufferSize)
{
    PaError err = Pa_Initialize();
    KWL_ASSERT(err == paNoError && "error initializing portaudio");
    
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream,
                               numInChannels,
                               numOutChannels,
                               paFloat32,   /* 32 bit floating point output. */
                               sampleRate,
                               bufferSize, /* frames per buffer, i.e. the number
                                            of sample frames that PortAudio will
                                            request from the callback. Many apps
                                            may want to use
                                            paFramesPerBufferUnspecified, which
                                            tells PortAudio to pick the best,
                                            possibly changing, buffer size.*/
                               (PaStreamCallback*)&paCallback,
                               engine->mixer);
    //printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    KWL_ASSERT(err == paNoError);
    
    err = Pa_StartStream(stream);
    //printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    KWL_ASSERT(err == paNoError);

	const PaStreamInfo* si = Pa_GetStreamInfo(stream);

    return KWL_NO_ERROR;
}

/**
 * Shuts down PortAudio.
 * @param engine
 */
kwlError kwlEngine_hostSpecificDeinitialize(kwlEngine* engine)
{
    
    PaError err = Pa_StopStream(stream);
    //printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    KWL_ASSERT(err == paNoError);
    
    err = Pa_CloseStream(stream);
    //printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    KWL_ASSERT(err == paNoError);
    
    err = Pa_Terminate();
    //printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    KWL_ASSERT(err == paNoError);
    return KWL_NO_ERROR;
}
