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

#include "../../kwl_memory.h"
#include "../../kwl_engine.h"
#include "../../kwl_mixer.h"
#include <SDL/SDL.h>

void kwlSDLAudioCallback(void *userData, Uint8 *stream, int numBytes)
{   
    KWL_ASSERT(0 && "the following code is not up to date");
    kwlMixer* mixer = (kwlMixer*)userData;
    mixer->numChannels;
    int bytesPerOutSample = 2; /*Always 16 bit = 2 bytes samples.*/
    /*compute the number of frames corresponding to the requested number of bytes*/
    int numFrames = numBytes / numChannels / bytesPerOutSample;

    /*get the next buffer from the software mixer*/
    
    kwlMixer_render(mixer, 
                                      (float*)&mixer->outBuffer, 
                                      numFrames, 
                                      mixer->numChannels);
    
    /*convert the 32 bit float samples of the mixer to 16 bit ints for output.*/
    short* streamShort = (short*)stream;
    
    kwlFloatToInt16(mixer->outBuffer, streamShort, numChannels * numFrames);
    /*
    for (int ch = 0; ch < numChannels; ch++)
    {
        int sampleIdx = ch;
        for (int currFrame = 0; currFrame < numFrames; currFrame++)
        {
            //printf("sampleIdx %d, v %f\n", sampleIdx, mixer->outBuffer[sampleIdx]);
            streamShort[sampleIdx] = (short)(((1 << 15) - 1) * mixer->outBuffer[sampleIdx]);
            sampleIdx += numChannels;
        }
    }*/
}

/** 
 * Initializes the SDL audio system with a callback that fills output buffers 
 * mixed by the Kowalski Engine.
 * @param engine
 * @param sampleRate
 * @param numChannels
 * @param bufferSize
 * @return A Kowalski error code.
 */
kwlError kwlEngine_hostSpecificInitialize(kwlEngine* engine, int sampleRate, int numOutChannels, int numInChannels, int bufferSize)
{
    /*specify the requested parameters*/
    SDL_AudioSpec desiredSpec;
    kwlMemset(&desiredSpec, 0, sizeof(SDL_AudioSpec));
    desiredSpec.freq = sampleRate;
    desiredSpec.channels = numChannels;
    desiredSpec.format = AUDIO_S16LSB;
    desiredSpec.samples = bufferSize;
    desiredSpec.callback = &kwlSDLAudioCallback;
    desiredSpec.userdata = engine->mixer;
    
    /*create a spec to hold the actual parameter values*/
    SDL_AudioSpec obtainedSpec;
    kwlMemset(&obtainedSpec, 0, sizeof(SDL_AudioSpec));
    
    /*init the audio system...*/
    int result = SDL_OpenAudio(&desiredSpec, &obtainedSpec);
    /*... and see how it went*/
    if (desiredSpec.channels != obtainedSpec.channels ||
        desiredSpec.samples != obtainedSpec.samples ||
        desiredSpec.freq != obtainedSpec.freq ||
        desiredSpec.format != obtainedSpec.format)
    {
        /*TODO report errors*/
    }
    
    SDL_PauseAudio(0);
        
    return KWL_NO_ERROR;
}

/**
 * Shuts down the SDL audio system.
 * @param engine
 */
kwlError kwlEngine_hostSpecificDeinitialize(kwlEngine* engine)
{
    SDL_CloseAudio();
    return KWL_NO_ERROR;
}
