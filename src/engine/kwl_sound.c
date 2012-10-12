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

#include "kwl_memory.h"
#include "kwl_sound.h"

#include "kwl_assert.h"
#include <stdlib.h>

void kwlSound_init(kwlSound* sound)
{
    kwlMemset(sound, 0, sizeof(kwlSound));
    
    sound->audioDataEntries = NULL;
    sound->numAudioDataEntries = 0;
    sound->playbackMode = KWL_RANDOM;
    sound->playbackCount = 0;
    sound->gain = 0;
    sound->gainVariation = 0;
    sound->pitch = 0;
    sound->pitchVariation = 0;
}

int kwlSound_pickNextBufferForEvent(kwlSound* sound, kwlEventInstance* event, int firstBuffer)
{
    KWL_ASSERT(event->currentPCMFrameIndex >= 0);
    
    if (event->numBuffersPlayed >= sound->playbackCount && 
        sound->playbackCount >= 0)
    {
        /*If the number of buffers played exceeds the playback count, return 1 to indicate
         that playback should end.*/
        return 1;
    }
    else if (event->playbackState == KWL_STOP_REQUESTED)
    {
        /* The event has been requested to stop. Don't pick a new buffer.*/
        return 1;
    }
    else if (event->playbackState == KWL_PLAYING_LAST_BUFFER)
    {
        /* Done playing the last buffer. Nothing further.*/
        return 1;
    }
    
    int lastBuffer = event->numBuffersPlayed == sound->playbackCount - 1 && 
                     sound->playbackCount >= 0;
    
    /*compute new pitch*/
    float randVal = -1 + 0.0002f * (rand() % 10000);
    float newPitch = sound->pitch + randVal * 0.01f * sound->pitchVariation;
    if (newPitch < PITCH_EPSILON)
    {
        /*Set the pitch to a small positive number to avoid the event getting stuck in the mixer.*/
        newPitch = PITCH_EPSILON; 
    }
    event->soundPitch = newPitch;
    
    /*compute new gain*/
    randVal = -1 + 0.0002f * (rand() % 10000);
    float newGain = sound->gain + randVal * 0.01f * sound->gainVariation;
    if (newGain < 0.0f)
    {
        newGain = 0.0f;
    }

    /*pick a new piece of audio data based on the playback mode of the sound*/
    int newIndex = 0;
    if (sound->playbackMode == KWL_RANDOM)
    {
        /*Pick a new random audio data index.*/
        newIndex = rand() % sound->numAudioDataEntries;
    }
    else if (sound->playbackMode == KWL_RANDOM_NO_REPEAT)
    {
        /*Pick a new random audio data index and make sure it's not the same
         as the last one (it will be in the degenerate case of 1 item).*/
        newIndex = rand() % sound->numAudioDataEntries;
        if (newIndex == event->currentAudioDataIndex)
        {
            newIndex = (newIndex + 1) % sound->numAudioDataEntries;
        }
    }
    else if (sound->playbackMode == KWL_SEQUENTIAL)
    {
        /*Pick the next audio data index and wrap around if we reach the end
         of the list. Start at the first buffer if the event was just triggered. */
        newIndex = firstBuffer != 0 ? 0 : (event->currentAudioDataIndex + 1) % sound->numAudioDataEntries;
    }
    else if (sound->playbackMode == KWL_SEQUENTIAL_NO_RESET)
    {
        /*Increment whatever index was last used and wrap around if we reach the end
         of the list.*/
        newIndex = (event->currentAudioDataIndex + 1) % sound->numAudioDataEntries;
    }
    else if (sound->playbackMode == KWL_IN_RANDOM_OUT)
    {
        if (firstBuffer != 0)
        {
            /*Always start with the first item.*/
            newIndex = 0;
        }
        else
        {
            if (event->playbackState == KWL_PLAY_LAST_BUFFER_AND_STOP_REQUESTED || 
                lastBuffer != 0)
            {
                newIndex = sound->numAudioDataEntries - 1;
                event->playbackState = KWL_PLAYING_LAST_BUFFER;
            }
            else if (sound->numAudioDataEntries < 2)
            {
                //degenerate case. keep playing the first item.
                newIndex = 0;
            }
            else
            {
                newIndex = 1 + rand() % (sound->numAudioDataEntries - 2);
            }
        }
    }
    else if (sound->playbackMode == KWL_IN_RANDOM_NO_REPEAT_OUT)
    {
        if (firstBuffer != 0)
        {
            /*Always start with the first item.*/
            newIndex = 0;
        }
        else
        {
            if (event->playbackState == KWL_PLAY_LAST_BUFFER_AND_STOP_REQUESTED || 
                lastBuffer != 0)
            {
                newIndex = sound->numAudioDataEntries - 1;
                event->playbackState = KWL_PLAYING_LAST_BUFFER;
            }
            else if (sound->numAudioDataEntries < 2)
            {
                //degenerate case. keep playing the first item.
                newIndex = 0;
            }
            else
            {
                newIndex = 1 + rand() % (sound->numAudioDataEntries - 2);
                if (newIndex == event->currentAudioDataIndex)
                {
                    newIndex = (newIndex + 1) % (sound->numAudioDataEntries - 2);
                }
            }
        }
    }
    else if (sound->playbackMode == KWL_IN_SEQUENTIAL_OUT)
    {
        if (firstBuffer != 0)
        {
            /*Always start with the first item.*/
            newIndex = 0;
        }
        else
        {
            if (event->playbackState == KWL_PLAY_LAST_BUFFER_AND_STOP_REQUESTED || 
                lastBuffer != 0)
            {
                newIndex = sound->numAudioDataEntries - 1;
                event->playbackState = KWL_PLAYING_LAST_BUFFER;
            }
            else if (sound->numAudioDataEntries < 2)
            {
                //degenerate case. keep playing the first item.
                newIndex = 0;
            }
            else
            {
                newIndex = (event->currentAudioDataIndex + 1) % (sound->numAudioDataEntries - 2);
            }
        }
    }
    else
    {
        KWL_ASSERT(NULL && "invalid sound playback mode");
    }
    
    kwlAudioData* nextAudioData = sound->audioDataEntries[newIndex];
    if (nextAudioData->bytes == NULL)
    {
        /*If the new piece of audio data has not been loaded, return 1 to indicate that
         playback should end.*/
        return 1;
    }
    KWL_ASSERT(nextAudioData->numChannels > 0);
    
    /*Set the new event state*/
    int numFrames = nextAudioData->numBytes / (nextAudioData->numChannels * 2); /*2 for 2 bytes per 16 bit sample*/
    
    int shouldResetFrameIndex = firstBuffer != 0 ||
                                ((sound->playbackMode == KWL_IN_SEQUENTIAL_OUT ||
                                 sound->playbackMode == KWL_IN_RANDOM_OUT ||
                                 sound->playbackMode == KWL_IN_RANDOM_NO_REPEAT_OUT) && 
                                    sound->deferStop == 0);
    if (shouldResetFrameIndex != 0)
    {
        event->currentPCMFrameIndex = 0;
    }
    else
    {
        /*Set the start index to be the number of samples that the current index overshoots the buffer size.
         This ensures smooth playback over buffer boundaries for pitch shifted sounds. 
         NOTE: The pitch accumulator must not be reset or audible glitches may occur at buffer boundaries.*/
        event->currentPCMFrameIndex = event->currentPCMFrameIndex - event->currentPCMBufferSize;
    }
    
    event->currentAudioDataIndex = newIndex;
    event->currentPCMBuffer = (short*)nextAudioData->bytes;
    event->currentPCMBufferSize = numFrames - 1;
    event->currentNumChannels = nextAudioData->numChannels;
    
    /*Finally, return 0 to indicate that playback should continue.*/
    return 0;
}
