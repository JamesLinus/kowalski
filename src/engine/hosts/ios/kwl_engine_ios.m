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

#ifdef KWL_IPHONE

#include "../../kwl_memory.h"
#include "kwl_asm.h"
#include "kwl_engine.h"
#include "kwl_engine_ios.h"
#include "kwl_mixer.h"

#include <AudioToolbox/AudioToolbox.h>

AudioStreamBasicDescription outputFormat;
AudioStreamBasicDescription inputFormat;
/** The Remote I/O unit */
AudioComponentInstance auComponentInstance;
/** A buffer list for storing input samples. */
AudioBufferList inputBufferList;
/** The size in bytes of the input sample buffer. */
int inputBufferByteSize = -1;

kwlEngine* soundEngine; //TODO: remove this?


kwlError kwlEngine_hostSpecificInitialize(kwlEngine* engine, 
                                               int sampleRate, 
                                               int numOutChannels, 
                                               int numInChannels, 
                                               int bufferSize)
{
    KWL_ASSERT(engine);
    KWL_ASSERT(engine->mixer);
    
    soundEngine = engine;
    
    if (numOutChannels != 2 && numOutChannels != 1)
    {
        return KWL_UNSUPPORTED_NUM_OUTPUT_CHANNELS;
    }
    
    if (numInChannels > 2 || numInChannels < 0)
    {
        return KWL_UNSUPPORTED_NUM_INPUT_CHANNELS;
    }
    
    /** 
     * Create the remote IO instance once.
     */
    createRemoteIOInstance();
    
    /*
     * Initialize the audio session
     */
    initAudioSession();
    
    /*
     * Activates audio session and starts RemoteIO unit if successful.
     */
    kwlResume();
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_hostSpecificDeinitialize(kwlEngine* engine)
{
    stopAndDeinitRemoteIO();
    
    AudioComponentInstanceDispose(auComponentInstance);
    
    return KWL_NO_ERROR;
}

void audioSessionInterruptionCallback(void *inClientData,  UInt32 inInterruptionState)
{
    if (inInterruptionState == kAudioSessionBeginInterruption)
    {
        //printf("* audio session interruption callback: begin interruption\n");
        kwlSuspend();
    }
    else if (inInterruptionState == kAudioSessionEndInterruption)
    {
        //printf("* audio session interruption callback: end interruption\n");
        kwlResume();
    }
    else 
    {
        KWL_ASSERT(0 && "unknown interruption state");
    }
    //debugPrintAudioSessionInfo();
}


void inputAvailableChangeCallback(void *inUserData,
                              AudioSessionPropertyID inPropertyID,
                              UInt32 inPropertyValueSize,
                              const void *inPropertyValue)
{
    //TODO: make sure this is thread safe
    kwlEngine* engine = (kwlEngine*)inUserData;
    engine->isInputEnabled = *((int*)inPropertyValue);

    //printf("* input availability changed. availability=%d\n", (*(int*)inPropertyValue));
    //debugPrintAudioSessionInfo();
}

OSStatus coreAudioInputCallback(void *inRefCon,
                                AudioUnitRenderActionFlags *ioActionFlags,
                                const AudioTimeStamp *inTimeStamp,
                                UInt32 inBusNumber,
                                UInt32 inNumberFrames,
                                AudioBufferList *ioData)
{
    inputBufferList.mBuffers[0].mDataByteSize = inputBufferByteSize;
    //fill the already allocated input buffer list with samples
    OSStatus status;    
    status = AudioUnitRender(auComponentInstance, 
                             ioActionFlags, 
                             inTimeStamp, 
                             inBusNumber, 
                             inNumberFrames, 
                             &inputBufferList);
    KWL_ASSERT(status == 0);
    
    //then pass the samples on to the mixer
    kwlMixer* const mixer = (kwlMixer*)inRefCon;

    const int numChannels = inputBufferList.mBuffers[0].mNumberChannels;
    short* buffer = (short*) inputBufferList.mBuffers[0].mData;
    int currFrame = 0;
    while (currFrame < inNumberFrames)
    {
        int numFramesToMix = inNumberFrames - currFrame;
        if (numFramesToMix > KWL_TEMP_BUFFER_SIZE_IN_FRAMES)
        {
            numFramesToMix = KWL_TEMP_BUFFER_SIZE_IN_FRAMES;
        }
        
        /*Convert input buffer samples to floats*/
        kwlInt16ToFloat(&buffer[currFrame * numChannels], 
                        mixer->inBuffer,
                        numFramesToMix * numChannels);
            
        /*Pass the converted buffer to the mixer*/
        kwlMixer_processInputBuffer(mixer, 
                                            &(mixer->inBuffer)[currFrame * numChannels], 
                                            numFramesToMix);
        
        currFrame += numFramesToMix;
    }
    
    return noErr;
}

OSStatus coreAudioOutputCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData)
{   
//#define KWL_DEBUG_CA_DEADLINE
#ifdef KWL_DEBUG_CA_DEADLINE
    static double prevDelta = 0.0;
    static double ht = 0.0;
    double delta = inTimeStamp->mSampleTime - ht;
    ht = inTimeStamp->mSampleTime;
    if (delta > inNumberFrames && prevDelta > 0.0)
    {
        printf("missed deadline, time since prev callback: %f samples, curr buffer size %d samples\n", delta, inNumberFrames);
        //debugPrintAudioSessionInfo();
        //debugPrintRemoteIOInfo();
    }
    prevDelta = delta;
#endif
    
    kwlMixer* const mixer = (kwlMixer*)inRefCon;
    
    const int numChannels = ioData->mBuffers[0].mNumberChannels;
    short* buffer = (short*) ioData->mBuffers[0].mData;
    int currFrame = 0;
    while (currFrame < inNumberFrames)
    {
        int numFramesToMix = inNumberFrames - currFrame;
        if (numFramesToMix > KWL_TEMP_BUFFER_SIZE_IN_FRAMES)
        {
            numFramesToMix = KWL_TEMP_BUFFER_SIZE_IN_FRAMES;
        }
    
        /*prepare a new buffer*/
        kwlMixer_render(mixer, mixer->outBuffer, numFramesToMix);

        kwlFloatToInt16(mixer->outBuffer, 
                        &buffer[currFrame * numChannels], 
                        numFramesToMix * numChannels);
        currFrame += numFramesToMix;
    }
    
    return noErr;
}

void stopAndDeinitRemoteIO()
{
    OSStatus status = AudioOutputUnitStop(auComponentInstance);
    KWL_ASSERT(status == noErr);
    
    status = AudioUnitUninitialize(auComponentInstance);
    KWL_ASSERT(status == noErr);
    
    free(inputBufferList.mBuffers[0].mData);
    inputBufferList.mBuffers[0].mData = NULL;
}

void createRemoteIOInstance()
{
    /*create audio component description*/
    AudioComponentDescription auDescription;
    
    auDescription.componentType          = kAudioUnitType_Output;
    auDescription.componentSubType       = kAudioUnitSubType_RemoteIO;
    auDescription.componentManufacturer  = kAudioUnitManufacturer_Apple;
    auDescription.componentFlags         = 0;
    auDescription.componentFlagsMask     = 0;
    
    /*get a component reference*/
    AudioComponent auComponent = AudioComponentFindNext(NULL, &auDescription);
    
    /*get the actual instance*/
    OSStatus status = AudioComponentInstanceNew(auComponent, &auComponentInstance);
    KWL_ASSERT(status == noErr);
}



void initAndStartRemoteIO()
{
	//make sure the audio unit is not initialized more than once.
	//some of the operations below depend on the unit not being
	//initialized.
    stopAndDeinitRemoteIO();
    
    const int bitsPerSample = 16; 
    const int numInChannels = soundEngine->mixer->numInChannels;
    const int numOutChannels = soundEngine->mixer->numOutChannels;
    float sampleRate = soundEngine->mixer->sampleRate;
    
    const unsigned int OUTPUT_BUS_ID = 0;
    const unsigned int INPUT_BUS_ID = 1;
    
    OSStatus status = 0;
    
    /*Enable recording if requested*/
    if (numInChannels > 0)
    {
        UInt32 flag = 1;
        status = AudioUnitSetProperty(auComponentInstance, 
                                      kAudioOutputUnitProperty_EnableIO, 
                                      kAudioUnitScope_Input, 
                                      INPUT_BUS_ID,
                                      &flag, 
                                      sizeof(flag));
        ensureNoAudioUnitError(status);
    }
    
    /*set buffer size. TODO: use the passed in value.*/
    Float32 preferredBufferSize = 0.024;
    status = AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, 
                                     sizeof(preferredBufferSize), 
                                     &preferredBufferSize);
    ensureNoAudioSessionError(status);
    
    /*enable playback*/    
    UInt32 flag = 1;
    status = AudioUnitSetProperty(auComponentInstance, 
                                  kAudioOutputUnitProperty_EnableIO, 
                                  kAudioUnitScope_Output, 
                                  OUTPUT_BUS_ID,
                                  &flag, 
                                  sizeof(flag));
    ensureNoAudioUnitError(status);
    
    /*set up output audio format description*/
    setASBD(&outputFormat, numOutChannels, sampleRate);
    
    /*apply format to output*/
    status = AudioUnitSetProperty(auComponentInstance, 
                                  kAudioUnitProperty_StreamFormat, 
                                  kAudioUnitScope_Input, 
                                  OUTPUT_BUS_ID, 
                                  &outputFormat, 
                                  sizeof(outputFormat));
    ensureNoAudioUnitError(status);
    
    /*apply format to input if enabled*/
    if (numInChannels > 0)
    {
        setASBD(&inputFormat, numInChannels, sampleRate);
        
        status = AudioUnitSetProperty(auComponentInstance, 
                                      kAudioUnitProperty_StreamFormat, 
                                      kAudioUnitScope_Output, 
                                      INPUT_BUS_ID, 
                                      &inputFormat, 
                                      sizeof(outputFormat));
        ensureNoAudioUnitError(status);
        
        int maxSliceSize = 0;
        int s = sizeof(maxSliceSize);
		status = AudioUnitGetProperty(auComponentInstance, 
                                      kAudioUnitProperty_MaximumFramesPerSlice, 
                                      kAudioUnitScope_Global, 
                                      0, 
                                      &maxSliceSize, 
                                      &s);
        ensureNoAudioUnitError(status);
        
        inputBufferList.mNumberBuffers = 1;
        inputBufferList.mBuffers[0].mNumberChannels = numInChannels;
        inputBufferByteSize = 2 * numInChannels * maxSliceSize;
        inputBufferList.mBuffers[0].mDataByteSize = inputBufferByteSize;
        inputBufferList.mBuffers[0].mData = malloc(inputBufferList.mBuffers[0].mDataByteSize);
    }
    
    KWL_ASSERT(status == noErr);
    
    AURenderCallbackStruct renderCallbackStruct;
    /*hook up the input callback*/
    if (numInChannels > 0)
    {
        renderCallbackStruct.inputProc = coreAudioInputCallback;
        renderCallbackStruct.inputProcRefCon = soundEngine->mixer;
        
        status = AudioUnitSetProperty(auComponentInstance, 
                                      kAudioOutputUnitProperty_SetInputCallback, 
                                      kAudioUnitScope_Global, 
                                      OUTPUT_BUS_ID, 
                                      &renderCallbackStruct, 
                                      sizeof(renderCallbackStruct));
        ensureNoAudioUnitError(status);
    }
    
    
    /*hook up the output callback*/
    renderCallbackStruct.inputProc = coreAudioOutputCallback;
    renderCallbackStruct.inputProcRefCon = soundEngine->mixer;
    
    status = AudioUnitSetProperty(auComponentInstance, 
                                  kAudioUnitProperty_SetRenderCallback, 
                                  kAudioUnitScope_Global, 
                                  OUTPUT_BUS_ID,
                                  &renderCallbackStruct, 
                                  sizeof(renderCallbackStruct));
    
    ensureNoAudioUnitError(status);
    
    /*init audio unit*/
    status = AudioUnitInitialize(auComponentInstance);
    //printf("status %d\n", status);
    ensureNoAudioUnitError(status);
    
    /*start audio unit*/
    status = AudioOutputUnitStart(auComponentInstance);
    //printf("status %d\n", status);
    ensureNoAudioUnitError(status);
    
    //TODO: use these to see what buffer size and sample rate we actually got.
    Float32 buffDur;
    int ss = sizeof(Float32);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareIOBufferDuration, 
                            &ss, 
                            &buffDur);
    
    Float64 sampRate;
    ss = sizeof(Float64);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, 
                            &ss, 
                            &sampRate);
}

void audioRouteChangeCallback(void *inUserData,
                              AudioSessionPropertyID inPropertyID,
                              UInt32 inPropertyValueSize,
                              const void *inPropertyValue)
{
    //printf("* audio route changed,\n");
    kwlEngine* engine = (kwlEngine*)inUserData;
    
    //get the old audio route name and the reason for the change
    CFDictionaryRef dict = inPropertyValue;
    CFStringRef oldRoute = 
        CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
    CFNumberRef reason = 
    CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
    int reasonNumber = -1;
    CFNumberGetValue(reason, CFNumberGetType(reason), &reasonNumber);
    
    //reason specific code
    switch (reasonNumber)
    {
        case kAudioSessionRouteChangeReason_Unknown: //0
        {
            //printf("kAudioSessionRouteChangeReason_Unknown\n");
            break;
        }   
        case kAudioSessionRouteChangeReason_NewDeviceAvailable: //1
        {
            //printf("kAudioSessionRouteChangeReason_NewDeviceAvailable\n");
            break;
        }
        case kAudioSessionRouteChangeReason_OldDeviceUnavailable: //2
        {
            //printf("kAudioSessionRouteChangeReason_OldDeviceUnavailable\n");
            break;
        }
        case kAudioSessionRouteChangeReason_CategoryChange: //3
        {
            //printf("kAudioSessionRouteChangeReason_CategoryChange\n");
            break;
        }   
        case kAudioSessionRouteChangeReason_Override: //4
        {
            //printf("kAudioSessionRouteChangeReason_Override\n");
            break;
        }
            // this enum has no constant with a value of 5
        case kAudioSessionRouteChangeReason_WakeFromSleep: //6
        {
            //printf("kAudioSessionRouteChangeReason_WakeFromSleep\n");
            break;
        }
        case kAudioSessionRouteChangeReason_NoSuitableRouteForCategory:
        {
            //printf("kAudioSessionRouteChangeReason_NoSuitableRouteForCategory\n");
            break;
        }
    }
    
    /* 
     From the Apple "Handling Audio Hardware Route Changes" docs:
     
     "One of the audio hardware route change reasons in iOS is 
     kAudioSessionRouteChangeReason_CategoryChange. In other words, 
     a change in audio session category is considered by the system—in 
     this context—to be a route change, and will invoke a route change 
     property listener callback. As a consequence, such a callback—if 
     it is intended to respond only to headset plugging and unplugging—should 
     explicitly ignore this type of route change."
     
     If kAudioSessionRouteChangeReason_CategoryChange is not ignored, we could get 
     an infinite loop because the audio session category is set below, which will in
     turn trigger kAudioSessionRouteChangeReason_CategoryChange and so on.
     */
    if (reasonNumber != kAudioSessionRouteChangeReason_CategoryChange)
    {
        /*
         * Deinit the remote io and set it up again depending on if input is available. 
         */
        UInt32 isAudioInputAvailable; 
        UInt32 size = sizeof(isAudioInputAvailable);
        OSStatus result = AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, 
                                                  &size, 
                                                  &isAudioInputAvailable);
        ensureNoAudioSessionError(result);
        
        stopAndDeinitRemoteIO();
        
        int numInChannels = isAudioInputAvailable != 0 ? soundEngine->mixer->numInChannels : 0;
        UInt32 sessionCategory = numInChannels == 0 ? kAudioSessionCategory_MediaPlayback : 
                                                      kAudioSessionCategory_PlayAndRecord;
        result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory), &sessionCategory);
        ensureNoAudioSessionError(result);
        
        if (numInChannels > 0)
        {
            int val = 1;
            result = AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, 
                                             sizeof(val), 
                                             &val);
            ensureNoAudioSessionError(result);
        }

        result = AudioSessionSetActive(true);
        ensureNoAudioSessionError(result); //-12986 seems to mean that kAudioSessionCategory_PlayAndRecord was set and no input is available 
        
        initAndStartRemoteIO();
    }
}

void serverDiedCallback(void *inUserData,
                        AudioSessionPropertyID inPropertyID,
                        UInt32 inPropertyValueSize,
                        const void *inPropertyValue)
{
    //printf("server died\n");
}

void initAudioSession()
{
    OSStatus status = 0;
    
    /*
     * Initialize and activte audio session
     */
    status = AudioSessionInitialize(NULL, NULL, &audioSessionInterruptionCallback, soundEngine);
    KWL_ASSERT(status == noErr);
    
    /*
     UInt32 isOtherAudioPlaying = 0;
     UInt32 propertySize = sizeof(isOtherAudioPlaying);
     status = AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &propertySize, &isOtherAudioPlaying);
     KWL_ASSERT(status == noErr);
     //printf("other audio playing = %d\n", isOtherAudioPlaying);
     */
    
    //check if audio input is available at all
    
    UInt32 inputAvailable; 
    int propertySize = sizeof(inputAvailable);
    status = AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &propertySize, &inputAvailable);
    KWL_ASSERT(status == noErr);
    
    if (inputAvailable == 0)
    {
        //This device does not support audio input at this point 
        //(this may change at any time, for example when connecting
        //a headset to an iPod touch).
        soundEngine->isInputEnabled = 0;
    }
    
    UInt32 sessionCategory = inputAvailable == 0 ? kAudioSessionCategory_MediaPlayback : 
                                                   kAudioSessionCategory_PlayAndRecord;
    status = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory), &sessionCategory);
    ensureNoAudioSessionError(status);
    
    int val = 1;
    status = AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, 
                                     sizeof(val), 
                                     &val);
    
    status = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioInputAvailable, 
                                             &inputAvailableChangeCallback, 
                                             soundEngine);
    ensureNoAudioSessionError(status);
    
    status = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, 
                                             audioRouteChangeCallback, 
                                             soundEngine);
    ensureNoAudioSessionError(status);
    
    status = AudioSessionAddPropertyListener(kAudioSessionProperty_ServerDied, 
                                             serverDiedCallback, 
                                             soundEngine);
    ensureNoAudioSessionError(status);
}

void kwlSuspend(void)
{
    stopAndDeinitRemoteIO();
    OSStatus result = AudioSessionSetActive(0);
    ensureNoAudioSessionError(result);
}

int kwlResume(void)
{
    OSStatus result = AudioSessionSetActive(1);
    if (result == noErr)
    {
        initAndStartRemoteIO();
        return 1;
    }
    
    return 0;
}

void setASBD(AudioStreamBasicDescription* asbd, int numChannels, float sampleRate)
{
    kwlMemset(asbd, 0, sizeof(AudioStreamBasicDescription));
    KWL_ASSERT(numChannels == 1 || numChannels == 2);
    asbd->mBitsPerChannel = 16;
    asbd->mBytesPerFrame = 2 * numChannels;
    asbd->mBytesPerPacket = asbd->mBytesPerFrame;
    asbd->mChannelsPerFrame = numChannels;
    asbd->mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    asbd->mFormatID = kAudioFormatLinearPCM;
    asbd->mFramesPerPacket = 1;
    asbd->mSampleRate = sampleRate;
}

void ensureNoAudioUnitError(OSStatus result)
{
    switch (result)
    {
        case kAudioUnitErr_InvalidProperty:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidProperty");
            break;
        case kAudioUnitErr_InvalidParameter:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidParameter");
            break;
        case kAudioUnitErr_InvalidElement:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidElement");
            break;
        case kAudioUnitErr_NoConnection:
            KWL_ASSERT(0 && "kAudioUnitErr_NoConnection");
            break;
        case kAudioUnitErr_FailedInitialization:
            KWL_ASSERT(0 && "kAudioUnitErr_FailedInitialization");
            break;
        case kAudioUnitErr_TooManyFramesToProcess:
            KWL_ASSERT(0 && "kAudioUnitErr_TooManyFramesToProcess");
            break;
        case kAudioUnitErr_InvalidFile:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidFile");
            break;
        case kAudioUnitErr_FormatNotSupported:
            KWL_ASSERT(0 && "kAudioUnitErr_FormatNotSupported");
            break;
        case kAudioUnitErr_Uninitialized:
            KWL_ASSERT(0 && "kAudioUnitErr_Uninitialized");
            break;
        case kAudioUnitErr_InvalidScope:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidScope");
            break;
        case kAudioUnitErr_PropertyNotWritable:
            KWL_ASSERT(0 && "kAudioUnitErr_PropertyNotWritable");
            break;
        case kAudioUnitErr_CannotDoInCurrentContext:
            KWL_ASSERT(0 && "kAudioUnitErr_CannotDoInCurrentContext");
            break;
        case kAudioUnitErr_InvalidPropertyValue:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidPropertyValue");
            break;
        case kAudioUnitErr_PropertyNotInUse:
            KWL_ASSERT(0 && "kAudioUnitErr_PropertyNotInUse");
            break;
        case kAudioUnitErr_Initialized:
            KWL_ASSERT(0 && "kAudioUnitErr_Initialized");
            break;
        case kAudioUnitErr_InvalidOfflineRender:
            KWL_ASSERT(0 && "kAudioUnitErr_InvalidOfflineRender");
            break;
        case kAudioUnitErr_Unauthorized:
            KWL_ASSERT(0 && "kAudioUnitErr_Unauthorized");
            break;
        default:
            KWL_ASSERT(result == noErr);
            break;
    }
}

void ensureNoAudioSessionError(OSStatus result)
{
    switch (result) 
    {
        case kAudioSessionNotActiveError:
            KWL_ASSERT(0 && "kAudioSessionNotActiveError");
            break;
        case kAudioSessionNotInitialized:
            KWL_ASSERT(0 && "kAudioSessionNotInitialized");
            break;
        case kAudioSessionAlreadyInitialized:
            KWL_ASSERT(0 && "kAudioSessionAlreadyInitialized");
            break;
        case kAudioSessionInitializationError:
            KWL_ASSERT(0 && "kAudioSessionInitializationError");
            break;
        case kAudioSessionUnsupportedPropertyError:
            KWL_ASSERT(0 && "kAudioSessionUnsupportedPropertyError");
            break;
        case kAudioSessionBadPropertySizeError:
            KWL_ASSERT(0 && "kAudioSessionBadPropertySizeError");
            break;
        case kAudioServicesNoHardwareError:
            KWL_ASSERT(0 && "kAudioServicesNoHardwareError");
            break;
        case kAudioSessionNoCategorySet:
            KWL_ASSERT(0 && "kAudioSessionNoCategorySet");
            break;
        case kAudioSessionIncompatibleCategory:
            KWL_ASSERT(0 && "kAudioSessionIncompatibleCategory");
            break;
        case kAudioSessionUnspecifiedError:
            KWL_ASSERT(0 && "kAudioSessionUnspecifiedError");
            break;
        default:
            KWL_ASSERT(result == noErr);
            break;
    }
}
    
void debugPrintAudioSessionInfo()
{
    int category = -1;
    int numOutChannels = -1;
    int numInChannels = -1;
    
    int propertySize = sizeof(category);
    
    OSStatus status = AudioSessionGetProperty(kAudioSessionProperty_AudioCategory, 
                                              &propertySize, 
                                              &category);
    KWL_ASSERT(status == noErr);
    
    status = AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareInputNumberChannels, 
                                     &propertySize, 
                                     &numInChannels);
    KWL_ASSERT(status == noErr);
    
    status = AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareOutputNumberChannels, 
                                     &propertySize, 
                                     &numOutChannels);
    KWL_ASSERT(status == noErr);
    
    printf("    Audio session info:\n");
    printf("        category %d\n", category);
    printf("        n in ch  %d\n", numInChannels);    
    printf("        n out ch %d\n", numOutChannels);
}

void debugPrintRemoteIOInfo()
{
    AudioStreamBasicDescription outFmt;
    int sz = sizeof(AudioStreamBasicDescription);
    AudioUnitGetProperty(auComponentInstance, 
                         kAudioUnitProperty_StreamFormat, 
                         kAudioUnitScope_Input, 
                         0,
                         &outFmt, 
                         &sz);
    
    AudioStreamBasicDescription inFmt;
    sz = sizeof(AudioStreamBasicDescription);
    AudioUnitGetProperty(auComponentInstance, 
                         kAudioUnitProperty_StreamFormat, 
                         kAudioUnitScope_Output, 
                         1,
                         &inFmt, 
                         &sz);
    
    printf("    Remote IO info:\n");
    printf("        Input bits/channel %d\n", inFmt.mBitsPerChannel);
    printf("        Input bytes/frame %d\n", inFmt.mBytesPerFrame);
    printf("        Input bytes/packet %d\n", inFmt.mBytesPerPacket);
    printf("        Input channels/frame %d\n", inFmt.mChannelsPerFrame);
    printf("        Input format flags %d\n", inFmt.mFormatFlags);
    printf("        Input format ID %d\n", inFmt.mFormatID);
    printf("        Input frames per packet %d\n", inFmt.mFramesPerPacket);
    printf("        Input sample rate %f\n", inFmt.mSampleRate);
    printf("\n");
    printf("        Output bits/channel %d\n", outFmt.mBitsPerChannel);
    printf("        Output bytes/frame %d\n", outFmt.mBytesPerFrame);
    printf("        Output bytes/packet %d\n", outFmt.mBytesPerPacket);
    printf("        Output channels/frame %d\n", outFmt.mChannelsPerFrame);
    printf("        Output format flags %d\n", outFmt.mFormatFlags);
    printf("        Output format ID %d\n", outFmt.mFormatID);
    printf("        Output frames per packet %d\n", outFmt.mFramesPerPacket);
    printf("        Output sample rate %f\n", outFmt.mSampleRate);
    
}


#endif /*KWL_IPHONE*/
