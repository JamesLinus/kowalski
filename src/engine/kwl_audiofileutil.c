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
#include "kwl_audiofileutil.h"
#include "kwl_asm.h"
#include "kwl_memory.h"

#include "assert.h"

static void* kwlAllocateBufferWithEntireStream(kwlInputStream* stream, int* fileSize)
{
    kwlInputStream_seek(stream, 0, SEEK_END);
    *fileSize = kwlInputStream_tell(stream);
    KWL_ASSERT(*fileSize > 0);
    signed char* buffer = (signed char*)KWL_MALLOC(*fileSize ,"entire file buffer");
    kwlInputStream_seek(stream, 0, SEEK_SET);
    int readBytes = kwlInputStream_read(stream, buffer, *fileSize);
    KWL_ASSERT(readBytes == *fileSize);
    
    return buffer;
}

kwlError kwlLoadAudioFile(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    audioData->encoding = KWL_ENCODING_UNKNOWN;
    
    kwlError error = kwlLoadAIFF(path, audioData, mode);
    if (error == KWL_NO_ERROR)
    {
        return error;
    }
    
    error = kwlLoadAU(path, audioData, mode);
    if (error == KWL_NO_ERROR)
    {
        return error;
    }
    
    error = kwlLoadWAV(path, audioData, mode);
    if (error == KWL_NO_ERROR)
    {
        return error;
    }
    
    error = kwlLoadOggVorbis(path, audioData, mode);
    return error;
}

kwlError kwlLoadAIFF(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    /* see http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/AIFF/Docs/AIFF-1.3.pdf */
    kwlInputStream stream;
    kwlInputStream_initWithFile(&stream, path);
    if (stream.file == NULL)
    {
        return KWL_FILE_NOT_FOUND;
    }
    
    kwlError result = kwlLoadAIFFFromStream(&stream, audioData, mode);
    if (result == KWL_NO_ERROR)
    {
        audioData->filePath = path;
    }
    
    kwlInputStream_close(&stream);
    
    return result;
}
    
kwlError kwlLoadAIFFFromStream(kwlInputStream* stream, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    kwlMemset(audioData, 0, sizeof(kwlAudioData));
    
    /*read FORM container chunk*/
    {
        /*Container chunk ID */
        const unsigned char c1 = kwlInputStream_readChar(stream);
        const unsigned char c2 = kwlInputStream_readChar(stream);
        const unsigned char c3 = kwlInputStream_readChar(stream);
        const unsigned char c4 = kwlInputStream_readChar(stream);
        if (c1 != 'F' || c2 != 'O' || c3 != 'R' || c4 != 'M')
        {
            //kwlInputStream_close(stream);
            return KWL_UNKNOWN_FILE_FORMAT;
        }
        
        /*Form type*/
        /*const int chunkSize = */kwlInputStream_readIntBE(stream);
        const unsigned char f1 = kwlInputStream_readChar(stream);
        const unsigned char f2 = kwlInputStream_readChar(stream);
        const unsigned char f3 = kwlInputStream_readChar(stream);
        const unsigned char f4 = kwlInputStream_readChar(stream);
        if (f1 != 'A' || f2 != 'I' || f3 != 'F' || f4 != 'F')
        {
            //kwlInputStream_close(stream);
            return KWL_UNKNOWN_FILE_FORMAT;
        }
    }
    
    int commonChunkFound = 0;
    int dataChunkFound = 0;
    int sampleSize = 0;
    int dataSizeInBytes = 0;
    unsigned int numFrames = 0;
    short numChannels = 0;
    void* readSamples = NULL;
    
    while (!commonChunkFound || !dataChunkFound)
    {
        unsigned char c1 = kwlInputStream_readChar(stream);
        unsigned char c2 = kwlInputStream_readChar(stream);
        unsigned char c3 = kwlInputStream_readChar(stream);
        unsigned char c4 = kwlInputStream_readChar(stream);
        
        if (c1 == 'C' && c2 == 'O' && c3 == 'M' && c4 =='M')
        {
            int chunkSize = kwlInputStream_readIntBE(stream);
            numChannels = kwlInputStream_readShortBE(stream);
            numFrames = kwlInputStream_readIntBE(stream);
            sampleSize = kwlInputStream_readShortBE(stream);
            /*Ignore the the sample rate for now, so skip those 10 bits.*/
            kwlInputStream_skip(stream, chunkSize - (2 + 4 + 2));
            commonChunkFound = 1;
            
            int unsupportedSampleSize = sampleSize != 8 &&
                                        sampleSize != 16 && 
                                        sampleSize != 24 && 
                                        sampleSize != 32;
            
            if (unsupportedSampleSize != 0 || numChannels > 2)
            {
                //kwlInputStream_close(stream);
                if (readSamples != NULL)
                {
                    KWL_FREE(readSamples);
                }
                return KWL_UNSUPPORTED_ENCODING;
            }
            
            audioData->numChannels = numChannels;
            audioData->numFrames = numFrames;
            
            
        }
        else if (c1 == 'S' && c2 == 'S' && c3 == 'N' && c4 =='D')
        {
            dataSizeInBytes = kwlInputStream_readIntBE(stream);
            unsigned int offset = kwlInputStream_readIntBE(stream);
            /*unsigned int blockSize = */kwlInputStream_readIntBE(stream);
            KWL_ASSERT(offset >= 0);
            
            if (mode == KWL_CONVERT_TO_INT16_OR_FAIL)
            {
                kwlInputStream_skip(stream, offset);
                readSamples = KWL_MALLOC(dataSizeInBytes, "aiff audio data");
                kwlInputStream_read(stream, (signed char*)readSamples, dataSizeInBytes);
            }
            
            dataChunkFound = 1;
        }
        else if (c1 == 'C' && c2 == 'O' && c3 == 'M' && c4 =='M')
        {
            int chunkSize = kwlInputStream_readIntBE(stream);
            kwlInputStream_skip(stream, chunkSize);
        }
        else 
        {
            int chunkSize = kwlInputStream_readIntBE(stream);
            kwlInputStream_skip(stream, chunkSize);
            //printf("skipping %c%c%c%c chunk\n", c1, c2, c3, c4);
        }
    }
    
    kwlAudioEncoding encoding = KWL_ENCODING_UNKNOWN;    
        
    switch (sampleSize)
    {
        case 8:
            encoding = KWL_ENCODING_SIGNED_8BIT_PCM;
            break;
        case 16:
            encoding = KWL_ENCODING_SIGNED_16BIT_PCM;
            break;
        case 24:
            encoding = KWL_ENCODING_SIGNED_24BIT_PCM;
            break;
        case 32:
            encoding = KWL_ENCODING_SIGNED_32BIT_PCM;
            break;
    }
    
    if (encoding == KWL_ENCODING_UNKNOWN &&
        mode == KWL_CONVERT_TO_INT16_OR_FAIL) 
    {
        return KWL_UNSUPPORTED_ENCODING;
    }
    
    int numBytes = 0;
    short* finalSamples = NULL;
    
    if (mode == KWL_CONVERT_TO_INT16_OR_FAIL)
    {
        finalSamples = kwlConvertBufferTo16BitSigned((char*)readSamples, 
                                                     dataSizeInBytes, 
                                                     &numBytes, 
                                                     encoding, 
                                                     1);
        audioData->encoding = KWL_ENCODING_SIGNED_16BIT_PCM;
        audioData->isBigEndian = 0;
        sampleSize = 16;
    }
    else if (mode == KWL_LOAD_ENTIRE_FILE)
    {
        int fileSize = -1;
        finalSamples = kwlAllocateBufferWithEntireStream(stream, &fileSize);
        numBytes = fileSize;
        audioData->encoding = encoding;
        audioData->isBigEndian = 1;
        audioData->isEntireFile = 1;
    }
    
    int numSamples = numBytes / (sampleSize >> 3);
    audioData->numFrames = numSamples / numChannels;
    audioData->numChannels = numChannels;
    audioData->numBytes = numBytes;
    audioData->bytes = finalSamples;
    
    audioData->isLoaded = mode != KWL_SKIP_AUDIO_DATA ? 1 : 0;
    
    return KWL_NO_ERROR;
}

kwlError kwlLoadWAV(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    kwlInputStream stream;
    kwlInputStream_initWithFile(&stream, path);
    if (stream.file == NULL)
    {
        return KWL_FILE_NOT_FOUND;
    }
    
    kwlError result = kwlLoadWAVFromStream(&stream, audioData, mode);
    if (result == KWL_NO_ERROR)
    {
        audioData->filePath = path;
    }
    
    kwlInputStream_close(&stream);
    
    return result;
}

kwlError kwlLoadWAVFromStreamWithOptionalIMA4Params(kwlInputStream* stream, 
                                                    kwlAudioData* audioData, 
                                                    kwlAudioDataLoadingMode mode, 
                                                    int* firstDataBlockByteOut,
                                                    int* dataBlockSizeOut,
                                                    int* nBlockAlignOut)
{
    kwlMemset(audioData, 0, sizeof(kwlAudioData));
    
    /*read RIFF chunk*/
    {
        const unsigned char c1 = kwlInputStream_readChar(stream);
        const unsigned char c2 = kwlInputStream_readChar(stream);
        const unsigned char c3 = kwlInputStream_readChar(stream);
        const unsigned char c4 = kwlInputStream_readChar(stream);
        if (c1 != 'R' || c2 != 'I' || c3 != 'F' || c4 != 'F')
        {
            //kwlInputStream_close(stream);
            return KWL_UNKNOWN_FILE_FORMAT;
        }
        
        /*const int chunkSize = */kwlInputStream_readIntBE(stream);
        const unsigned char f1 = kwlInputStream_readChar(stream);
        const unsigned char f2 = kwlInputStream_readChar(stream);
        const unsigned char f3 = kwlInputStream_readChar(stream);
        const unsigned char f4 = kwlInputStream_readChar(stream);
        if (f1 != 'W' || f2 != 'A' || f3 != 'V' || f4 != 'E')
        {
            //kwlInputStream_close(stream);
            return KWL_UNKNOWN_FILE_FORMAT;
        }
    }
    
    int fmtChunkFound = 0;
    int dataChunkFound = 0;
    short numChannels = 0;
    short nBlockAlign = 0;
    short audioFormat = 0;
    short bitsPerSample = 0;
    kwlAudioEncoding sourceEncoding = KWL_ENCODING_UNKNOWN;
    
    while (!fmtChunkFound || !dataChunkFound)
    {
        unsigned char c1 = kwlInputStream_readChar(stream);
        unsigned char c2 = kwlInputStream_readChar(stream);
        unsigned char c3 = kwlInputStream_readChar(stream);
        unsigned char c4 = kwlInputStream_readChar(stream);
        
        if (c1 == 'f' && c2 == 'm' && c3 == 't' && c4 ==' ')
        {
            /*printf("reading %c%c%c%c (fmt ) chunk", c1, c2, c3, c4);*/
            const int chunkSize = kwlInputStream_readIntLE(stream);
            audioFormat = kwlInputStream_readShortLE(stream);
            numChannels = kwlInputStream_readShortLE(stream);
            const int sampleRate = kwlInputStream_readIntLE(stream);
            const int  byteRate = kwlInputStream_readIntLE(stream);
            nBlockAlign = kwlInputStream_readShortLE(stream);
            if (nBlockAlignOut != NULL)    
            {
                *nBlockAlignOut = nBlockAlign;
            }
            bitsPerSample = kwlInputStream_readShortLE(stream);
            const short cbSize = audioFormat == 1 ? 0 : kwlInputStream_readIntLE(stream);
            /*
            printf("    chunk size      = %d\n", chunkSize);
            printf("    audio format    = %d\n", audioFormat);
            printf("    num channels    = %d\n", numChannels);
            printf("    sample rate     = %d\n", sampleRate);
            printf("    byte rate = %d\n", byteRate);
            printf("    block align     = %d\n", nBlockAlign);
            printf("    bits per sample = %d\n", bitsPerSample);
            printf("    cbSize = %d\n", cbSize);
            */
            if (audioFormat == 0x1) /*PCM*/
            {
                switch (bitsPerSample)
                {
                    case 8:
                        sourceEncoding = KWL_ENCODING_UNSIGNED_8BIT_PCM;
                        break;
                    case 16:
                        sourceEncoding = KWL_ENCODING_SIGNED_16BIT_PCM;
                        break;
                    case 24:
                        sourceEncoding = KWL_ENCODING_SIGNED_24BIT_PCM;
                        break;
                    case 32:
                        sourceEncoding = KWL_ENCODING_SIGNED_32BIT_PCM;
                        break;
                }
                
                if (sourceEncoding == KWL_ENCODING_UNKNOWN &&
                    mode == KWL_CONVERT_TO_INT16_OR_FAIL)
                {
                    //kwlInputStream_close(stream);
                    return KWL_UNSUPPORTED_ENCODING;
                }
            }
            else if (audioFormat == 0x11 && bitsPerSample == 4)/*4 bit ADPCM*/
            {
                sourceEncoding = KWL_ENCODING_IMA_ADPCM;
                if (mode == KWL_CONVERT_TO_INT16_OR_FAIL)
                {
                    return KWL_UNSUPPORTED_ENCODING;
                }
            }
            else
            {
                //kwlInputStream_close(stream);
                return KWL_UNSUPPORTED_ENCODING;
            }
            
            fmtChunkFound = 1;            
        }
        else if (c1 == 'd' && c2 == 'a' && c3 == 't' && c4 =='a')
        {
            //printf("reading %c%c%c%c (data) chunk\n", c1, c2, c3, c4);
            const int chunkSize = kwlInputStream_readIntLE(stream);
            void* readSamples = NULL;
            if (mode == KWL_CONVERT_TO_INT16_OR_FAIL)
            {
                readSamples = KWL_MALLOC(chunkSize, "read wav audio data");
                kwlInputStream_read(stream, (signed char*)readSamples, chunkSize);
            }
            else
            {
                audioData->fileOffset = kwlInputStream_tell(stream);
                kwlInputStream_skip(stream, chunkSize);
                
                if (firstDataBlockByteOut != NULL)
                {
                    *firstDataBlockByteOut = audioData->fileOffset;
                }
                if (dataBlockSizeOut != NULL)
                {
                    *dataBlockSizeOut = chunkSize;
                }
            }
            
            /*
            if (sourceEncoding == KWL_ENCODING_IMA_ADPCM)
            {
                audioData->bytes = readSamples;
                audioData->encoding = KWL_ENCODING_IMA_ADPCM;
                audioData->numBytes = chunkSize;
                audioData->numFrames = chunkSize * 2 / numChannels;
            }
            else*/
            {
                int convertedSizeInBytes = -1;
                audioData->bytes = NULL;
                
                if (mode == KWL_CONVERT_TO_INT16_OR_FAIL)
                {
                    audioData->bytes = (short*)kwlConvertBufferTo16BitSigned((char*)readSamples, 
                                                                             chunkSize, 
                                                                             &convertedSizeInBytes, 
                                                                             sourceEncoding, 
                                                                             0);
                    audioData->encoding = KWL_ENCODING_SIGNED_16BIT_PCM;
                    audioData->numBytes = convertedSizeInBytes;
                    audioData->numFrames = convertedSizeInBytes / (2 * numChannels);
                }
                else
                {
                    audioData->encoding = sourceEncoding;
                    audioData->numBytes = chunkSize;
                    audioData->numFrames = 8 * chunkSize / (bitsPerSample * numChannels);
                }
                
            }
            
            audioData->isLoaded = mode != KWL_SKIP_AUDIO_DATA ? 1 : 0;
            audioData->streamFromDisk = 0;
            audioData->numChannels = numChannels;
            audioData->waveBank = NULL;
            
            kwlInputStream_skip(stream, chunkSize); //TODO: remove?
            dataChunkFound = 1;
        }
        else
        {
            //printf("skipping %c%c%c%c chunk\n", c1, c2, c3, c4);
            const int chunkSize = kwlInputStream_readIntLE(stream);
            kwlInputStream_skip(stream, chunkSize);
        }
    }
    
    if (mode == KWL_LOAD_ENTIRE_FILE)
    {
        int fileSize = -1;
        audioData->bytes = kwlAllocateBufferWithEntireStream(stream, &fileSize);
        audioData->numBytes = fileSize;
        audioData->isEntireFile = 1;
    }
    
    //kwlInputStream_close(stream);
        
    return KWL_NO_ERROR;
}

kwlError kwlLoadWAVFromStream(kwlInputStream* stream, 
                              kwlAudioData* audioData, 
                              kwlAudioDataLoadingMode mode)
{
    return kwlLoadWAVFromStreamWithOptionalIMA4Params(stream, audioData, mode, NULL, NULL, NULL);
}

kwlError kwlLoadIMAADPCMWAVMetadataFromStream(kwlInputStream* stream, 
                                              kwlAudioData* audioData, 
                                              int* firstDataBlockByte,
                                              int* dataBlockSize,
                                              int* nBlockAlign)
{
    return kwlLoadWAVFromStreamWithOptionalIMA4Params(stream, 
                                                      audioData, 
                                                      KWL_SKIP_AUDIO_DATA, 
                                                      firstDataBlockByte,
                                                      dataBlockSize,
                                                      nBlockAlign);
}

kwlError kwlLoadAU(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    kwlInputStream stream;
    kwlInputStream_initWithFile(&stream, path);
    if (stream.file == NULL)
    {
        kwlInputStream_close(&stream);
        return KWL_FILE_NOT_FOUND;
    }
    
    kwlError result = kwlLoadAUFromStream(&stream, audioData, mode);
    if (result == KWL_NO_ERROR)
    {
        audioData->filePath = path;
    }
    
    kwlInputStream_close(&stream);
    
    return result;
}

kwlError kwlLoadAUFromStream(kwlInputStream* stream, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    kwlMemset(audioData, 0, sizeof(kwlAudioData));
    
    int magicNumber = kwlInputStream_readIntBE(stream);
    if (magicNumber != 0x2e736e64)
    {
        //kwlInputStream_close(stream);
        return KWL_UNKNOWN_FILE_FORMAT;
    }
    
    int dataOffset = kwlInputStream_readIntBE(stream);
    int dataSizeInBytes = kwlInputStream_readIntBE(stream);
    if (dataSizeInBytes < 0)
    {
        //kwlInputStream_close(stream);
        return KWL_UNSUPPORTED_ENCODING;
    }
    int encodingInt = kwlInputStream_readIntBE(stream);
    int sampleRate = kwlInputStream_readIntBE(stream);
    int numChannels = kwlInputStream_readIntBE(stream);
    if (numChannels != 1 && numChannels != 2)
    {
        //kwlInputStream_close(stream);
        return KWL_UNSUPPORTED_NUM_OUTPUT_CHANNELS;
    }
    
    if (encodingInt < 2 && encodingInt > 7)
    {
        /* Not a PCM encoding. */
        //kwlInputStream_close(stream);
        return KWL_UNSUPPORTED_ENCODING;
    }
    
    /*jump to the beginning of the sample data*/
    kwlInputStream_reset(stream);
    kwlInputStream_skip(stream, dataOffset);
    
    kwlAudioEncoding encoding = KWL_ENCODING_UNKNOWN;    
    switch (encodingInt)
    {
        case 2:
            encoding = KWL_ENCODING_SIGNED_8BIT_PCM;
            break;
        case 3:
            encoding = KWL_ENCODING_SIGNED_16BIT_PCM;
            break;
        case 4:
            encoding = KWL_ENCODING_SIGNED_24BIT_PCM;
            break;
        case 5:
            encoding = KWL_ENCODING_SIGNED_32BIT_PCM;
            break;
    }
    
    if (encoding == KWL_ENCODING_UNKNOWN &&
        mode == KWL_CONVERT_TO_INT16_OR_FAIL) 
    {
        kwlInputStream_close(stream);
        return KWL_UNSUPPORTED_ENCODING;
    }
    
    int numBytes = 0;
    short* finalSamples = NULL;
    
    if (mode == KWL_CONVERT_TO_INT16_OR_FAIL)
    {
        void* readSamples = KWL_MALLOC(dataSizeInBytes, "read au audio data");
        kwlInputStream_read(stream, (signed char*)readSamples, dataSizeInBytes);
        finalSamples = kwlConvertBufferTo16BitSigned((char*)readSamples, 
                                                     dataSizeInBytes, 
                                                     &numBytes, 
                                                     encoding, 
                                                     1);
        
        audioData->encoding = KWL_ENCODING_SIGNED_16BIT_PCM;
    }
    else if (mode == KWL_LOAD_ENTIRE_FILE)
    {
        int fileSize = -1;
        finalSamples = kwlAllocateBufferWithEntireStream(stream, &fileSize);
        numBytes = fileSize;
        audioData->encoding = encoding;
        audioData->isBigEndian = 1;
        audioData->isEntireFile = 1;
    }
    
    int numSamples = numBytes / 2;
    
    audioData->numFrames = numSamples / numChannels;
    audioData->numChannels = numChannels;
    audioData->numBytes = numSamples * 2;
    audioData->bytes = finalSamples;
    audioData->isLoaded = mode != KWL_SKIP_AUDIO_DATA ? 1 : 0;
    
    return KWL_NO_ERROR;
}

kwlError kwlLoadOggVorbis(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode)
{
    kwlMemset(audioData, 0, sizeof(kwlAudioData));
    
    kwlInputStream stream;
    kwlInputStream_initWithFile(&stream, path);
    if (stream.file == NULL)
    {
        return KWL_FILE_NOT_FOUND;
    }
    
    int numChannels = 0;
    float sampleRate = 0;
    int bitsPerSample = 0;
    
    //read ogg header
    {
        //capture pattern (4 bytes)
        const char cp1 = kwlInputStream_readChar(&stream);
        const char cp2 = kwlInputStream_readChar(&stream);
        const char cp3 = kwlInputStream_readChar(&stream);
        const char cp4 = kwlInputStream_readChar(&stream);
        
        int isOgg = cp1 == 'O' && cp2 == 'g' && cp3 == 'g' && cp4 == 'S';
        
        if (!isOgg)
        {
            kwlInputStream_close(&stream);
            return KWL_UNKNOWN_FILE_FORMAT;
        }
        
        char version = kwlInputStream_readChar(&stream);
        char headerTyper = kwlInputStream_readChar(&stream);
        /*long granulePosition*/ kwlInputStream_readIntLE(&stream); kwlInputStream_readIntLE(&stream);
        int serialNumber = kwlInputStream_readIntLE(&stream);
        int pageSequenceNumber = kwlInputStream_readIntLE(&stream);
        int checkSum = kwlInputStream_readIntLE(&stream);
        char numPageSegments = kwlInputStream_readChar(&stream);
        for (int i = 0; i < numPageSegments; i++)
        {
            kwlInputStream_readChar(&stream);
        }
        //byte[] segmentTable = new byte[numPageSegments];
        //dis.read(segmentTable);
    }
    
    //read vorbis header
    {
        //packet type should be 1 for identification header
        const char packetType = kwlInputStream_readChar(&stream);
        
        int isVorbis =
        kwlInputStream_readChar(&stream) == 'v' &&
        kwlInputStream_readChar(&stream) == 'o' &&
        kwlInputStream_readChar(&stream) == 'r' &&
        kwlInputStream_readChar(&stream) == 'b' &&
        kwlInputStream_readChar(&stream) == 'i' &&
        kwlInputStream_readChar(&stream) == 's';
        
        if (!isVorbis)
        {
            return KWL_UNKNOWN_FILE_FORMAT;
        }
        
        int vorbisVersion = kwlInputStream_readIntLE(&stream);
        numChannels = kwlInputStream_readChar(&stream);
        sampleRate = kwlInputStream_readIntLE(&stream);
        int maxBitRate = kwlInputStream_readIntLE(&stream);
        int minBitRate = kwlInputStream_readIntLE(&stream);
        int nominalBitRate = kwlInputStream_readIntLE(&stream);
        //etc
    }
    
    audioData->encoding = KWL_ENCODING_VORBIS;
    audioData->numChannels = numChannels;
    audioData->numFrames = 0;
    
    if (mode == KWL_SKIP_AUDIO_DATA)
    {
        /*nothing further*/
    }
    else
    {
        KWL_ASSERT(0 && "TODO");
    }
    
    kwlInputStream_close(&stream);
    
    
    return KWL_NO_ERROR;
}

short* kwlConvertBufferTo16BitSigned(char* inBuffer,
                                     int inBufferSizeInBytes,
                                     int* outBufferSizeInBytes,
                                     kwlAudioEncoding inBufferEncoding,
                                     int isBigEndian)
{
    int bytesPerSample = 0;
    switch (inBufferEncoding)
    {
        case KWL_ENCODING_SIGNED_8BIT_PCM:
        case KWL_ENCODING_UNSIGNED_8BIT_PCM:
            bytesPerSample = 1;
            break;
        case KWL_ENCODING_SIGNED_16BIT_PCM:
            bytesPerSample = 2;
            break;
        case KWL_ENCODING_SIGNED_24BIT_PCM:
            bytesPerSample = 3;
            break;
        case KWL_ENCODING_SIGNED_32BIT_PCM:
            bytesPerSample = 4;
            break;
        default:
            KWL_ASSERT(0 && "unknown encoding");
    }
    
    const int numSamples = inBufferSizeInBytes / bytesPerSample;
    *outBufferSizeInBytes = numSamples * 2;
    short* outBuffer = inBufferEncoding == KWL_ENCODING_SIGNED_16BIT_PCM ? (short*)inBuffer :
        (short*)KWL_MALLOC(*outBufferSizeInBytes, "converted audio buffer");
    
    switch (inBufferEncoding)
    {
        case KWL_ENCODING_UNSIGNED_8BIT_PCM:
        {
            kwlUInt8ToInt16(inBuffer, inBufferSizeInBytes, outBuffer);
            break;
        }
        case KWL_ENCODING_SIGNED_8BIT_PCM:
        {
            kwlInt8ToInt16(inBuffer, inBufferSizeInBytes, outBuffer);
            break;
        }
        case KWL_ENCODING_SIGNED_16BIT_PCM:
        {
            kwlInt16ToInt16(inBuffer, inBufferSizeInBytes, outBuffer, isBigEndian);
            break;
        }
        case KWL_ENCODING_SIGNED_24BIT_PCM:
        {
            kwlInt24ToInt16(inBuffer, inBufferSizeInBytes, outBuffer, isBigEndian);
            break;
        }
        case KWL_ENCODING_SIGNED_32BIT_PCM:
        {
            kwlInt32ToInt16(inBuffer, inBufferSizeInBytes, outBuffer, isBigEndian);
            break;
        }
        default:
            KWL_ASSERT(0 && "unknown encoding");
    }
    
    if ((short*)inBuffer != (short*)outBuffer)
    {
        KWL_FREE(inBuffer);
    }
    
    return outBuffer;
}

