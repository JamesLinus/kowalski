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

#ifndef KWL_ASM_H
#define KWL_ASM_H

/*! \file
 This file contains methods called in the tight loops
 of the engine that are all candidates to be rewritten in assembler.
 */

#ifdef KWL_IPHONE
#include <TargetConditionals.h>
#endif /*KWL_IPHONE*/

#include "kwl_assert.h"
#include "kwl_memory.h"
#include "string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
#include "assert.h"
    
    union FloatAsBits
    {
        float value;
        int bits;
    };
    
    static inline float logGainToLinGain(float input)
    {
        return input * input * input * input;
    }
    
    static inline int kwlIsGreaterThanOne(float value)
    {
#if KWL_IPHONE
        
        /*TODO*/
        union FloatAsBits fab;
        fab.value = value;
        /*check that the sign bit is not set and the exponent is less than 127*/
        return ((fab.bits & 0x80000000) == 0) && ((fab.bits & 0x7f800000) > 0x3f800000);
#else
        return value > 1.0f;
#endif
    }
    
    static inline int kwlIsAbsGreaterThanOne(float value)
    {
#if KWL_IPHONE
        /*TODO*/
        union FloatAsBits fab;
        fab.value = value;
        /*check that the sign bit is not set and the exponent is less than 127*/
        
        return (fab.bits & 0x7f800000) > 0x3f800000;
#else
        return value > 1.0f || value < -1.0f;
#endif
    }
    
    /**
     * Reverses the byte order for all elements in a given array of shorts.
     * @param buffer The buffer to process.
     * @param size The number of elements in the buffer to process.
     */
    static inline void kwlSwapEndian16(short* buffer, int size)
    {
        int i;
        for (i = 0; i < size; i++)
        {
            int temp = buffer[i];
            int high = ((temp & 0x00ff) << 8) & 0xff00;
            int low = ((temp & 0xff00) >> 8) & 0x00ff;
            buffer[i] = high | low;
        }
    }
    
    /**
     * Finds and returns the maximum absolute value in a given buffer.
     * @param buffer The buffer containing the values to check
     * @param size The total size of the buffer
     * @param offset The index of the first element to check.
     * @param stride The distance between elements to check, e.g if stride is 2, every other
     *               element is checked.
     * @return The maximum absolute value.
     */
    static inline float kwlGetBufferAbsMax(float* buffer, int size, int offset, int stride)
    {
        float absMax = 0.0f;
        int i = offset;
        
        while (i < size)
        {
            const float val = buffer[i];
            if (val < -absMax)
            {
                absMax = -val;
            }
            else if (val > absMax)
            {
                absMax = val;
            }
            i += stride;
        }
        
        return absMax;
    }
    
    /**
     * Sets all elements of a given float buffer to zero
     * @param buffer The buffer to clear.
     * @param size The size of the buffer to clear.
     */
    static inline void kwlClearFloatBuffer(float* buffer, int size)
    {
        /*TODO*/
        memset(buffer, 0, size * sizeof(float));
    }
    
    
    
    static inline void kwlMixFloatBuffer(float* sourceBuffer, float* targetBuffer, int numSamples)
    {
#if TARGET_CPU_ARM
        int i = 0;
        while (i < numSamples)
        {
            targetBuffer[i] += sourceBuffer[i];
            i++;
        }
#else
        
        int i = 0;
        while (i < numSamples)
        {
            targetBuffer[i] += sourceBuffer[i];
            i++;
        }
#endif
    }
    
    /**
     * Mixes a given source buffer into a given target buffer.
     * @param sourceBuffer The buffer of source samples.
     * @param targetBuffer The buffer to mix into.
     * @param size The size of \c sourceBuffer and \c targetBuffer.
     * @param offset The index of the first sample to mix.
     * @param stride The distance between samples to mix.
     * @param gain The gain to apply to the mixed source buffer.
     */
    static inline void kwlMixFloatBufferWithGain(float* sourceBuffer, float* targetBuffer,
                                                 int size, int offset, int stride, float gain)
    {
        /*TODO*/
        int i = offset;
        while (i < size)
        {
            targetBuffer[i] += gain * sourceBuffer[i];
            i += stride;
        }
    }
    
    static inline void kwlApplyGainRamp(float* outBuffer,
                                        int numOutChannels,
                                        int numFrames,
                                        float startGain[2],
                                        float endGain[2])
    {
        float deltaGain[2] =
        {
            endGain[0] - startGain[0],
            endGain[1] - startGain[1]
        };
        
        const int numSamples = numOutChannels * numFrames;
        
        /*
         If the gain difference between consecutive frames is less than this,
         a gain ramp will not be applied.
         */
        const float eps = 1e-7f;
        
        for (int ch = 0; ch < numOutChannels; ch++)
        {
            float gain = startGain[ch];
            float deltaGainPerFrame = deltaGain[ch] / numFrames;
            
            if (deltaGainPerFrame < eps && deltaGainPerFrame > -eps)
            {
                /* Gain difference too small, don't apply ramp */
                for (int i = ch; i < numSamples; i += numOutChannels)
                {
                    outBuffer[i] *= gain;
                }
            }
            else
            {
                /* Apply gain ramp */
                for (int i = ch; i < numSamples; i += numOutChannels)
                {
                    outBuffer[i] *= gain;
                    gain += deltaGainPerFrame;
                }
            }
        }
    }
    
    static inline void kwlInt16ToFloatWithGain(short* sourceBuffer,
                                               float* targetBuffer,
                                               int maxTargetPosPlusOne,
                                               int* sourceReadPos,
                                               int sourceStride,
                                               int* targetReadPos,
                                               int targetStride,
                                               float gain)
    {
        KWL_ASSERT(sourceBuffer != NULL);
        KWL_ASSERT(targetBuffer != NULL);
        KWL_ASSERT(*sourceReadPos >= 0);
        KWL_ASSERT(*targetReadPos >= 0);
        KWL_ASSERT(sourceStride >= 0);
        KWL_ASSERT(targetStride >= 0);
        KWL_ASSERT(gain >= 0);
        
        int srcPos = *sourceReadPos;
        int targetPos = *targetReadPos;
        float gainTot = gain / 32767.0f;
        while (targetPos < maxTargetPosPlusOne)
        {
            targetBuffer[targetPos] = gainTot * sourceBuffer[srcPos];
            targetPos += targetStride;
            srcPos += sourceStride;
        }
        
        *sourceReadPos = srcPos;
        *targetReadPos = targetPos;
    }
    
    
    static inline void kwlInt16ToFloatWithGainAndPitch(short* sourceBuffer,
                                                       float* targetBuffer,
                                                       int maxTargetPosPlusOne,
                                                       int* sourceReadPos,
                                                       int sourceStride,
                                                       int* targetReadPos,
                                                       int targetStride,
                                                       float gain,
                                                       float pitch,
                                                       float* pitchAccumulator)
    {
        KWL_ASSERT(sourceBuffer != NULL);
        KWL_ASSERT(targetBuffer != NULL);
        KWL_ASSERT(*sourceReadPos >= 0);
        KWL_ASSERT(*targetReadPos >= 0);
        KWL_ASSERT(sourceStride >= 0);
        KWL_ASSERT(targetStride >= 0);
        KWL_ASSERT(gain >= 0);
        KWL_ASSERT(pitch > 0);
        
        int srcPos = *sourceReadPos;
        int targetPos = *targetReadPos;
        float pitchAccum = *pitchAccumulator;
        
        /*a mix loop with linear interpolation pitch shifting*/
        while (targetPos < maxTargetPosPlusOne)
        {
            /** Compute the current output sample by linear interpolation between the current and next
             source samples.*/
            /*TODO: optimize this.*/
            targetBuffer[targetPos] = (1 - pitchAccum) * sourceBuffer[srcPos] +
            pitchAccum * sourceBuffer[srcPos + sourceStride];
            targetBuffer[targetPos] *= gain / 32767.0f;
            /** Increment the pitch accumulator...*/
            pitchAccum += pitch;
            /** ... and advance the source read position by the integer part of the accumulator.*/
            const int accumulatorIntegerPart = (int)(pitchAccum);
            srcPos += accumulatorIntegerPart * sourceStride;
            /** Finally, update the pitch accumulator and advance the out buffer position*/
            pitchAccum -= accumulatorIntegerPart;
            targetPos += targetStride;
        }
        
        *sourceReadPos = srcPos;
        *targetReadPos = targetPos;
        *pitchAccumulator = pitchAccum;
    }
    
    /**
     * Converts a buffer of signed short values to a buffer of floats
     * in the range [-1, 1].
     * @param sourceBuffer The buffer containing the values to convert.
     * @param targetBuffer The buffer to write converted samples to.
     * @param size The size of the source and target buffers.
     */
    static inline void kwlInt16ToFloat(short* sourceBuffer, float* targetBuffer, int size)
    {
        KWL_ASSERT(sourceBuffer != NULL);
        KWL_ASSERT(targetBuffer != NULL);
        
        int i = 0;
        while (i < size)
        {
            targetBuffer[i] = (float)(sourceBuffer[i] / 32768.0);
            i++;
        }
    }
    
    /**
     * Converts a buffer of floats to a buffer of signed shorts. The floats are assumed
     * to be in the range [-1, 1].
     * @param sourceBuffer The buffer containing the values to convert.
     * @param targetBuffer The buffer to write converted samples to.
     * @param size The size of the source and target buffers.
     */
    static inline void kwlFloatToInt16(float* sourceBuffer, short* targetBuffer, int size)
    {
        KWL_ASSERT(sourceBuffer != NULL);
        KWL_ASSERT(targetBuffer != NULL);
        
        int i = 0;
        while (i < size)
        {
            targetBuffer[i] = (short)(32767 * sourceBuffer[i]);
            i++;
        }
    }
    
    static inline void kwlUInt8ToInt16(char* sourceBuffer,
                                       int sourceBufferSizeInBytes,
                                       short* targetBuffer)
    {
        const int bytesPerSample = 1;
        const int numSamples = sourceBufferSizeInBytes / bytesPerSample;
        
        int i;
        for (i = 0; i < numSamples; i++)
        {
            targetBuffer[i] = (((signed char*)sourceBuffer)[i] - 128) << 8;
        }
    }
    
    static inline void kwlInt8ToInt16(char* sourceBuffer,
                                      int sourceBufferSizeInBytes,
                                      short* targetBuffer)
    {
        const int bytesPerSample = 1;
        const int numSamples = sourceBufferSizeInBytes / bytesPerSample;
        
        int i;
        for (i = 0; i < numSamples; i++)
        {
            targetBuffer[i] = ((signed char*)sourceBuffer)[i] << 8;
        }
    }
    
    static inline void kwlInt16ToInt16(char* sourceBuffer,
                                       int sourceBufferSizeInBytes,
                                       short* targetBuffer,
                                       int bigEndian)
    {
        const int bytesPerSample = 2;
        const int numSamples = sourceBufferSizeInBytes / bytesPerSample;
        
        kwlMemcpy(targetBuffer, sourceBuffer, sourceBufferSizeInBytes);
        
        if (bigEndian != 0)
        {
            kwlSwapEndian16(targetBuffer, numSamples);
        }
    }
    
    
    static inline void kwlInt24ToInt16(char* sourceBuffer,
                                       int sourceBufferSizeInBytes,
                                       short* targetBuffer,
                                       int bigEndian)
    {
        const int bytesPerSample = 3;
        const int numSamples = sourceBufferSizeInBytes / bytesPerSample;
        
        if (bigEndian != 0)
        {
            int i;
            for (i = 0; i < numSamples; i++)
            {
                targetBuffer[i] = ((sourceBuffer[3 * i] << 8) & 0xff00) |
                (sourceBuffer[3 * i + 1] & 0x00ff);
            }
        }
        else
        {
            int i;
            for (i = 0; i < numSamples; i++)
            {
                targetBuffer[i] = ((sourceBuffer[3 * i + 2] << 8) & 0xff00) |
                (sourceBuffer[3 * i + 1] & 0x00ff);
            }
        }
    }
    
    static inline void kwlInt32ToInt16(char* sourceBuffer,
                                       int sourceBufferSizeInBytes,
                                       short* targetBuffer,
                                       int bigEndian)
    {
        const int bytesPerSample = 4;
        const int numSamples = sourceBufferSizeInBytes / bytesPerSample;
        
        if (bigEndian != 0)
        {
            int i;
            for (i = 0; i < numSamples; i++)
            {
                const int be = ((int*)(sourceBuffer))[i];
                targetBuffer[i] = (((be << 24) & 0xff000000) |
                                   ((be << 8) & 0x00ff0000) |
                                   ((be >> 8) & 0x0000ff00) |
                                   ((be >> 24) & 0x000000ff)) >> 16;
            }
        }
        else
        {
            int i;
            for (i = 0; i < numSamples; i++)
            {
                const int be = ((int*)(sourceBuffer))[i];
                targetBuffer[i] = be >> 16;
            }
        }
    }
    
    
    /**
     * Clamps all the values in a given float buffer to be in the range [-1, 1].
     * @param buffer The buffer containing the values to clamp.
     * @param size The number of values in the buffer.
     */
    static inline void kwlClampBuffer(float* buffer, int size)
    {
        int i = 0;
        while (i < size)
        {
            float val = buffer[i];
            if (val < -1.0f)
            {
                val = -1.0f;
            }
            else if (val > 1.0f)
            {
                val = 1.0f;
            }
            buffer[i] = val;
            i++;
        }
    }
    
    /**
     * Computes an approximation of the inverse square root.
     * @param x The argument.
     * @return An approximation of 1 / sqrt(x).
     */
    static inline float kwlFastInverseSqrt(float x)
    {
        union {
            float f;
            int i;
        } tmp;
        tmp.f = x;
        tmp.i = 0x5f3759df - (tmp.i >> 1);
        float y = tmp.f;
        return y * (1.5f - 0.5f * x * y * y);
    }
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_ASM_H*/
