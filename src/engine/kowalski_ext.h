/*
Copyright (c) 2010-2011 Per Gantelius

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

#ifndef KWL__KOWALSKI_EXT_H
#define KWL__KOWALSKI_EXT_H

/*! \file 
 This header contains extensions to the API defined in kowalski.h.
 These extensions are kept in a separate file because they use 
 C style constructs (like structs and pointers to variables and functions) and
 are not part of the language bindings.
 */ 

#include "kowalski.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
/************************************************************************/
/**
 * @name DSP units
 *  An API allowing DSP units to be attached at various points in the signal chain.
 */
/** @{ */
    
/**
 * A callback that gets invoked to pass sample buffers to a DSP unit. Only
 * called from the mixer thread. Perform manipulation of variables shared
 * between the engine and mixer threads only in the \c updateDSPMixerCallback
 * and \c updateDSPEngineCallback callbacks set in the \c kwlDSPUnit struct.
 * @param numChannels The number of channels of the buffer to process.
 * @param numFrames The number of frames of the buffer to process.
 * @param data The user data associated with the DSP unit.
 */
typedef void (*kwlDSPCallback)(float* inBuffer, 
                               int numChannels, 
                               int numFrames, 
                               void* data);
    
/**
 * Called to notify a DSP unit to update its parameters.
 * @param data The user data associated with the DSP unit.
 */
typedef void (*kwlDSPUpdateCallback)(void* data);

/**
 */
typedef void (*kwlDSPCleanupCallback)(void* data);
    
    
/**
 * A handle to a DSP unit that can be attached to various points
 * in the signal chain to perform custom processing.
 */
typedef struct kwlDSPUnit* kwlDSPUnitHandle;


/** 
 * <p>Attaches a given DSP unit to an event instance corresponding to a given handle.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * <li>\c KWL_INVALID_EVENT_INSTANCE_HANDLE If the event handle does not correspond to an event instance.</li>
 * </ul>
 * </p>
 * @param dspUnit The DSP unit to attach. Pass \c NULL to remove any current DSP unit. 
 * @param eventHandle A handle to the event to which to attach the DSP unit.
 * @see kwlDSPUnitAttachToInput
 * @see kwlDSPUnitAttachToMixBus
 * @see kwlDSPUnitAttachToOutput
 */
void kwlDSPUnitAttachToEvent(kwlDSPUnitHandle dspUnit, kwlEventHandle eventHandle);

/** 
 * <p>Attaches a given DSP unit for processing audio input (e.g from a microphone).</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * </ul>
 * </p>
 * @param dspUnit The DSP unit to attach. Pass \c NULL to remove any current DSP unit.
 * @see kwlDSPUnitAttachToEvent
 * @see kwlDSPUnitAttachToMixBus
 * @see kwlDSPUnitAttachToOutput
 * @see kwlIsInputEnabled
 */    
void kwlDSPUnitAttachToInput(kwlDSPUnitHandle dspUnit);
    
/** 
 * <p>Attaches a given DSP unit to a mix bus corresponding to a given handle.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * <li>\c KWL_INVALID_MIX_BUS_HANDLE If the mix bus handle does not correspond to a mix bus.</li>
 * </ul>
 * </p>
 * @param dspUnit The DSP unit to attach. Pass \c NULL to remove any current DSP unit.
 * @param mixBusHandle A handle to the mix bus to which to attach the DSP unit
 * @see kwlDSPUnitAttachToEvent
 * @see kwlDSPUnitAttachToInput
 * @see kwlDSPUnitAttachToOutput
 */
void kwlDSPUnitAttachToMixBus(kwlDSPUnitHandle dspUnit, kwlMixBusHandle mixBusHandle);

/** 
 * <p>Attaches a given DSP to the master output. This is a suitable entry
 * point for programmers who want to perform custom synthesis without relying
 * on events and engine data.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * </ul>
 * </p>
 * @param dspUnit The DSP unit to attach. Pass \c NULL to remove any current DSP unit.
 * @see kwlDSPUnitAttachToEvent
 * @see kwlDSPUnitAttachToMixBus
 * @see kwlDSPUnitAttachToInput
 */    
void kwlDSPUnitAttachToOutput(kwlDSPUnitHandle dspUnit);
    
/**
 * <p>Creates and returns a handle to a custom DSP unit.</p>
 */
kwlDSPUnitHandle kwlDSPUnitCreateCustom(void* userdata, 
                                        kwlDSPCallback process, 
                                        kwlDSPUpdateCallback updateEngine, 
                                        kwlDSPUpdateCallback updateMixer, 
                                        kwlDSPCleanupCallback cleanup);
    
/**
 *<p>Returns a non-zero integer if audio input is currently enabled and zero otherwise</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * </ul>
 * </p> 
 * @see kwlDSPUnitAttachToInput
 */
int kwlIsInputEnabled();
    
/** @} */ /*End of DSP units group*/
    
    
/************************************************************************/
/**
 * @name Audio file utilities
 *  Functions and data structures for loading audio files.
 */
/** @{ */
    
/**
 * A buffer of linear PCM audio data.
 */
typedef struct kwlPCMBuffer
{
    /** 
     * The number of frames of audio data. A frame of stereo data, for example,
     * consists of the left channel sample followed
     * by the right channel sample.
     */
    int numFrames;
    /** The number of channels.*/
    int numChannels;
    /** 
     * An array of interleaved signed 16 bit samples. 
     * The number of samples is numChannels * numFrames.
     * The sample for channel i in frame j is at index
     * j * numChannels + i.
     */
    short* pcmData;
} kwlPCMBuffer;

/**
 *<p>Releases any audio data associated with a given PCM buffer.</p>
 * @see kwlPCMBufferLoad
 */
void kwlPCMBufferFree(kwlPCMBuffer* buffer);
    
/**
 *<p>Loads PCM (i.e uncompressed) audio data from a file. Supported file formats 
 * are au, wav and aiff. This function can be called when the engine
 * is not initialized. Audio data loaded using this function can be released
 * with kwlPCMBufferFree.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_FILE_NOT_FOUND if the specified audio file cannot be found.</li>
 * <li>\c KWL_UNKNOWN_FILE_FORMAT if the given file is of an unknown format.</li>
 * <li>\c KWL_UNSUPPORTED_ENCODING if the audio file format is recognized but the encoding is unsupported.</li> 
 * </ul>
 * </p> 
 * @param audioFilePath The path of the audio file to create the event from.
 * @param buffer A pointer to a kwlPCMBuffer struct to populate with audio data.
 * @see kwlPCMBufferFree
 */
kwlError kwlPCMBufferLoad(const char* const audioFilePath, kwlPCMBuffer* buffer);    
    
/** @} */ /* End of audio file utilities block */
    

/************************************************************************/
/**
 * @name Event interface extensions.
 *  Additions to the event API defined in kowalski.h.
 */
/** @{ */
    
/** 
 * <p>Called when an event instance stops playing. The callback is invoked
 * on the application thread. An event instance could stop playing for any of
 * the following reasons:
 * <ul>
 * <li>The end of the last audio data item was reached.</li>
 * <li>\c kwlEventStop was called.</li>
 * <li>Data referenced by the event was unloaded.</li>
 * <li>The event instance was stolen during one-shot playback.</li>
 * </ul> 
 * </p>
 * @param userData Optional user data.
 * @see kwlEventSetCallback
 * @see kwlEventStartOneShotWithCallback
 * @see kwlEventStartOneShotWithCallbackAt
 */
typedef void (*kwlEventStoppedCallack)(void* userData);
    
/**
 * <p>Creates a freeform event from a given PCM buffer.
 * The buffer passed to this method is not released along with the event. 
 * Events created using this function
 * exist in parallel with any loaded engine data and wave banks.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * <li>\c KWL_POSITIONAL_EVENT_MUST_BE_MONO if the specified audio file is stereo and \c eventType is \c KWL_POSITIONAL.</li>
 * <li>\c KWL_INVALID_PARAMETER_VALUE if the number of frames is less than 1, if the number
 * of channels is not 1 or 2 or if buffer the audio buffer is \c NULL.</li>
 * </ul>
 * </p> 
 * @param buffer A pointer to the PCM buffer to create the event from.
 * @return An event handle corresponding to the created event or \c KWL_INVALID_HANDLE if an error occurred.
 * @see kwlEventCreateWithFile
 * @see kwlEventGetHandle
 * @see kwlGetError
 */    
kwlEventHandle kwlEventCreateWithBuffer(kwlPCMBuffer* buffer, kwlEventType eventType);

/**
 * <p>Specifies a callback to invoke when a given event instance stops playing.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * <li>\c KWL_INVALID_EVENT_INSTANCE_HANDLE if the provided handle does not correspond to an event instance.</li>
 * <li>\c KWL_INVALID_PARAMETER_VALUE if \c callback is \c NULL.</li>
 * </ul>
 * </p> 
 * @param event A handle to the event instance to attach the callback to.
 * @param callback The callback to attach.
 * @param userData User data that gets passed to the callback. Can be NULL.
 */        
void kwlEventSetCallback(kwlEventHandle event, kwlEventStoppedCallack callback, void* userData);

/**
 * <p>Does the same as \c kwlEventStartOneShot but allows a callback to be specified that gets
 * invoked when the event stops.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * <li>\c KWL_INVALID_PARAMETER_VALUE if \c callback is NULL.</li>
 * <li>\c KWL_INVALID_EVENT_DEFINITION_HANDLE if the provided handle does not correspond to an event definition.</li>
 * <li>\c KWL_NO_FREE_EVENT_INSTANCES if all instances of the given event definition are associated with handles.</li>
 * <li>\c KWL_EVENT_IS_NOT_NONPOSITIONAL if the event definition is positional.</li> 
 * </ul>
 * </p> 
 * @param handle An event definition handle corresponding to the event to start.
 * @param callback A pointer to the function to call when playback stops.
 * @param userData User data that gets passed to the callback. Can be \c NULL.
 * @see kwlEventStartOneShot
 * @see kwlEventStartOneShotWithCallbackAt
 * @see kwlEventSetCallback
 */        
void kwlEventStartOneShotWithCallback(kwlEventDefinitionHandle handle, kwlEventStoppedCallack callback, void* userData);
    
/**
 * <p>Does the same as \c kwlEventStartOneShotAt but allows a callback to be specified that gets
 * invoked when the event stops.</p>
 * <p>
 * <strong>Error codes:</strong>
 * <ul>
 * <li>\c KWL_ENGINE_IS_NOT_INITIALIZED if the Kowalski engine has not been initialized.</li>
 * <li>\c KWL_INVALID_PARAMETER_VALUE if \c callback is \c NULL.</li>
 * <li>\c KWL_INVALID_EVENT_DEFINITION_HANDLE if the provided handle does not correspond to an event definition.</li>
 * <li>\c KWL_NO_FREE_EVENT_INSTANCES if all instances of the given event definition are associated with handles.</li>
 * <li>\c KWL_EVENT_IS_NOT_POSITIONAL if the event definition is non-positional.</li>
 * </ul>
 * </p> 
 * @param handle An event definition handle corresponding to the event to start.
 * @param x The x component of the playback position.
 * @param y The y component of the playback position.
 * @param z The z component of the playback position
 * @param callback A pointer to the function to call when playback stops.
 * @param userData User data that gets passed to the callback. Can be \c NULL. 
 * @see kwlEventStartOneShotAt
 * @see kwlEventStartOneShotWithCallback
 * @see kwlEventSetCallback
 */        
void kwlEventStartOneShotWithCallbackAt(kwlEventDefinitionHandle eventDefinition, float x, float y, float z, kwlEventStoppedCallack callback, void* userData);
 
/** @} */ /*End of event interface extensions group*/
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__KOWALSKI_EXT_H*/
