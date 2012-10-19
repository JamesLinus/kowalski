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

#ifndef KWL__SOUND_H
#define KWL__SOUND_H

/*! \file */ 

#include "kwl_audiodata.h"
#include "kwl_eventinstance.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * <p>Valid sound playback modes, describing how the next audio 
 * data item is selected from the sound's audio data list 
 * when the current item has finished playing.</p>
 * The behaviour when the referencing event is stopped is determined
 * by the combination of the playback mode and the value of the \c deferStop
 * flag of the \c kwlSoundDefinition struct. The following timeline diagram shows the behaviour
 * for different combinations of playback modes and \deferStop values.
 * A sound with five audio data items, each one second long, is used in this example.
 * The \c playbackCount is -1, which means new items are selected until the referencing
 * event is requested to stop.
 * \code
 * |0s         |1s         |2s         |3s         |4s         |5s
 * |Start                                   |Stop   
 * |called                                  |called 
 * |                                        |       
 * |                                        |       
 * V                                        V
 * KWL_RANDOM:
 * [1.........][2.........][2.........][4...]
 * KWL_RANDOM_NO_REPEAT:
 * [1.........][4.........][3.........][4...]
 * KWL_SEQUENTIAL/KWL_SEQUENTIAL_NO_RESET:
 * [1.........][2.........][3.........][4...]
 * KWL_IN_RANDOM_OUT:
 * [1.........][2.........][2.........][4...][5.........]
 * KWL_IN_RANDOM_NO_REPEAT_OUT:
 * [1.........][4.........][3.........][4...][5.........]
 * KWL_IN_SEQUENTIAL_OUT:
 * [1.........][2.........][3.........][4...][5.........]
 * 
 * KWL_RANDOM, defer stop:
 * [1.........][2.........][2.........][4.........]
 * KWL_RANDOM_NO_REPEAT, defer stop:
 * [1.........][4.........][3.........][4.........]
 * KWL_SEQUENTIAL/KWL_SEQUENTIAL_NO_RESET, defer stop:
 * [1.........][2.........][3.........][4.........]
 * KWL_IN_RANDOM_OUT, defer stop
 * [1.........][3.........][3.........][2.........][5.........]
 * KWL_IN_RANDOM_NO_REPEAT_OUT, defer stop
 * [1.........][4.........][2.........][3.........][5.........]
 * KWL_IN_SEQUENTIAL_OUT, defer stop
 * [1.........][2.........][3.........][4.........][5.........]
 *         
 * \endcode
 */
typedef enum
{
    /** 
     * An audio data item is chosen randomly. Example sequence from a sound with 5 audio data items:
     * \code
     * 5, 2, 3, 4, 3, 2, 2, 2, 3, 5, 4, 3, 1, 2, 3....
     * \endcode
     */
    KWL_RANDOM = 0,
    /** 
     * An audio data item is chosen randomly among all items except the one that just finished playing.
     * Example sequence from a sound with 5 audio data items:
     * \code
     * 1, 4, 5, 2, 4, 1, 5, 2, 4, 1, 3, 1, 2, 5, 4....
     * \endcode     
     */
    KWL_RANDOM_NO_REPEAT,
    /** 
     * The next audio data item in the list is chosen. If the end of the list is reached, the first item is chosen. 
     * Example sequence from a sound with 5 audio data items:
     * \code
     * 1, 2, 3, 4, 5, 1, 2, ....
     * \endcode
     */
    KWL_SEQUENTIAL,
    /**
     * The same as KWL_SEQUENTIAL, except the audio data index is not reset between 
     * playbacks. So whereas KWL_SEQUENTIAL generates the same sequences each time
     * the event is triggered, KWL_SEQUENTIAL_NO_RESET would generate for example
     * \code
     * 1, 2, 3, 4, 5, 1, 2 (first playback)
     * 3, 4, 5, 1, 2, 3, 4 (second playback)
     * \endcode
     */
    KWL_SEQUENTIAL_NO_RESET,
    /** 
     * Example sequence from a sound with 5 audio data items:
     * \code
     * 1, 2, 2, 4, 2, 3, 2, 4, 2, 2, 4, 2, 3, 2, 4....5
     * \endcode     
     */
    KWL_IN_RANDOM_OUT,
    /** 
     * Example sequence from a sound with 5 audio data items:
     * \code
     * 1, 2, 3, 4, 3, 4, 4, 3, 2, 3, 3, 2, 4, 3, 4....5
     * \endcode     
     */
    KWL_IN_RANDOM_NO_REPEAT_OUT,
    /** 
     * Example sequence from a sound with 5 audio data items:
     * \code
     * 1, 2, 4, 3, 4, 2, 4, 2, 3, 4, 3, 2, 3, 2, 4....5
     * \endcode     
     */
    KWL_IN_SEQUENTIAL_OUT

    
} kwlSoundPlaybackMode;

/** 
 * A sound definition associated with an non-streaming event. Defines
 * a set of PCM waveforms and how they should be played back.
 */
typedef struct kwlSoundDefinition
{
    /** The audio data entries associated with this sound*/
    kwlAudioData** audioDataEntries;
    /** The number of audio data entries associated with this sound*/
    int numAudioDataEntries;
    /** The playback mode, describing how to select what audio data entries to play. */
    kwlSoundPlaybackMode playbackMode;
    /** The number of audio data entries to play in total. */
    int playbackCount;
    /** 
     * Non-zero if the currently playing buffer should not be stopped when the referencing
     * event is stopped.
     */
    char deferStop;
    /** The gain factor */
    float gain;
    /** The current random variation of the gain*/
    float gainVariation;
    /** The pitch value */
    float pitch;
    /** The current random variation of the pitch factor */
    float pitchVariation;
} kwlSoundDefinition;

/** */
void kwlSoundDefinition_init(kwlSoundDefinition* sound);

/**
 * Initializes a given event according to a sound definition.
 * @return Non-zero if the event should stop playing, zero otherwise.
 */
int kwlSoundDefinition_pickNextBufferForEvent(kwlSoundDefinition* sound, struct kwlEventInstance* event, int firstBuffer);


#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__SOUND_H*/
