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

/*! \file */ 

#ifdef KWL_IPHONE

#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudioTypes.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    

/**
 * The callback for processing a new buffer of input samples.
 */
OSStatus coreAudioInputCallback(void *inRefCon,
                                AudioUnitRenderActionFlags *ioActionFlags,
                                const AudioTimeStamp *inTimeStamp,
                                UInt32 inBusNumber,
                                UInt32 inNumberFrames,
                                AudioBufferList *ioData);

/**
 * The callback for rendering a new buffer of output samples.
 */
OSStatus coreAudioOutputCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData);

/** Creates the singleton remote I/O unit instance. */
void createRemoteIOInstance(void);

/** 
 * Stops and uninitializes the remote I/O unit.
 */
void stopAndDeinitRemoteIO(void);

/**
 * Initializes and starts the remote I/O unit.
 */
void initAndStartRemoteIO(void);

/** 
 * Initializes the AVAudioSession.
 */
void initAudioSession(void);

void kwlSuspend(void);

/**
 * Returns non-zero if successful, zero otherwise.
 */
int kwlResume(void);

/** 
 * Called when the audio session gets interrupted.
 */
void audioSessionInterruptionCallback(void *inClientData,  UInt32 inInterruptionState);

/** 
 * Called when audio input availability changes.
 */
void inputAvailableChangeCallback(void *inUserData,
                                  AudioSessionPropertyID inPropertyID,
                                  UInt32 inPropertyValueSize,
                                  const void *inPropertyValue);

/**
 * Called when the audio route changes.
 */
void audioRouteChangeCallback(void *inUserData,
                              AudioSessionPropertyID inPropertyID,
                              UInt32 inPropertyValueSize,
                              const void *inPropertyValue);


/**
 * 
 */
void serverDiedCallback(void *inUserData,
                        AudioSessionPropertyID inPropertyID,
                        UInt32 inPropertyValueSize,
                        const void *inPropertyValue);

/**
 * Helper function that initializes an AudioStreamBasicDescription corresponding
 * to linear PCM with a given number of channels and a given sample rate
 * @param asbd The AudioStreamBasicDescription to initialize.
 * @param numChannels The number of channels.
 * @param sampleRate The sample rate.
 */
void setASBD(AudioStreamBasicDescription* asbd, int numChannels, float sampleRate);

/**
 * Generates a meaningful assert if the result of an audio unit operation
 * is not successful.
 * @param result The error code to check.
 */
void ensureNoAudioUnitError(OSStatus result);

/**
 * Generates a meaningful assert if the result of an audio session operation
 * is not successful.
 * @param result The error code to check.
 */
void ensureNoAudioSessionError(OSStatus result);

/** 
 * Prints some info about the current audio session to the console.
 */
void debugPrintAudioSessionInfo();

/** 
 * Prints some info about the remote I/O unit to the console.
 */
void debugPrintRemoteIOInfo();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif //KWL_IPHONE
