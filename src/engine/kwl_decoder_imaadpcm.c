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
#include "kwl_audiofileutil.h"
#include "kwl_decoder_imaadpcm.h"
#include "kwl_memory.h"

kwlError kwlInitDecoderIMAADPCM(kwlDecoder* decoder)
{
    /*Allocate decoder data.*/
    kwlIMAADPCMCodecData* data = 
        (kwlIMAADPCMCodecData*)KWL_MALLOC(sizeof(kwlIMAADPCMCodecData), "IMAADPCM decoder data");
    kwlMemset(data, 0, sizeof(kwlIMAADPCMCodecData));
    
    /* Hook up data and callbacks to the decoder.*/
    decoder->codecData = data;
    decoder->decodeBuffer = kwlDecodeBufferIMAADPCM;
    decoder->deinit = kwlDeinitDecoderIMAADPCM;
    decoder->rewind = kwlRewindDecoderIMAADPCM;
    
    kwlLoadIMAADPCMWAVMetadataFromStream(&decoder->audioDataStream, 
                                         &data->adpcmDataDescription,
                                         &data->firstDataBlockByte,
                                         &data->dataSize,
                                         &data->nBlockAlign);
    
    /*nBlockAlign is the number of bytes per data block. an encoded sample is 4 bits and
      a decoded one is 16, so we need nBlockAlign * 4 bytes to hold a decoded datablock.*/
    decoder->maxDecodedBufferSize = data->nBlockAlign * 4;
    decoder->numChannels = data->adpcmDataDescription.numChannels;
    KWL_ASSERT((data->dataSize % data->nBlockAlign) == 0);
    
    KWL_ASSERT(decoder->numChannels > 0);
    
    /*allocate a buffer for the current data block */
    data->currentDatablock = (unsigned char*)KWL_MALLOC(data->nBlockAlign, "IMAADPCM data block buffer");
    
    /*go to the start of the first data block*/
    kwlInputStream_seek(&decoder->audioDataStream, data->firstDataBlockByte, SEEK_SET);
    
	return KWL_NO_ERROR;
}

void kwlDeinitDecoderIMAADPCM(kwlDecoder* decoder)
{
    kwlIMAADPCMCodecData* data = (kwlIMAADPCMCodecData*)decoder->codecData;
    
    KWL_FREE(data->currentDatablock);
    KWL_FREE(data);
}

int kwlDecodeBufferIMAADPCM(kwlDecoder* decoder)
{
    kwlIMAADPCMCodecData* codecData = (kwlIMAADPCMCodecData*)decoder->codecData;
    
    /*
     each data block is nBlockAlign bytes and starts with numChannels header words followed by
     nBlockAlign/(4 * numChannels) - 1 data words per channel. 
     Each compressed sample is 4 bits in length.
     
     The header words have the following structure
     
     Byte0      |   Byte1       |   Byte2       |   Byte3
     -----------------------------------------------------------
     Absolute   |   Absolute    |   Step table  |   Reserved
     Samp0      |   Samp0       |   index       |
     LoByte     |   HiByte      |               |
     
     
     The data word N has the following structure
     
     Byte0          |Byte1          |Byte2          |Byte3          
     ----------------------------------------------------------------
     SampP+0|SampP+1|SampP+2|SampP+3|SampP+4|SampP+5|SampP+6|SampP+7|
     lsb msb|lsb msb|lsb msb|lsb msb|lsb msb|lsb msb|lsb msb|lsb msb|
     bit0       bit7|bit0       bit7|bit0       bit7|bit0       bit7|
     
     where 
     P = (N * 8) + 1.
     Data word N is a word for a given channel in the range
     0 to nBlockAlign / (4 * numChannels) - numChannels - 1.
     */
    
    /* read the next encoded data block */
    int bytesRead = kwlInputStream_read(&decoder->audioDataStream,
                                        codecData->currentDatablock,
                                        codecData->nBlockAlign);
    if (bytesRead == 0)
    {
        /* No more data. */
        return 1;
    }
    
    KWL_ASSERT(bytesRead == codecData->nBlockAlign);
    codecData->currentByte += bytesRead;
    
    /* decode the data block */
    const int numChannels = decoder->numChannels;
    const int nDataWordsPerChannel = codecData->nBlockAlign/ (4 * numChannels) - 1;
    unsigned char* datablock = codecData->currentDatablock;
    short* outBuffer = decoder->currentDecodedBuffer;
    
    kwlMemset(outBuffer, 0, 4 * codecData->nBlockAlign);
    
    /*Loop over the interleaved data words*/
    for (int ch = 0; ch < numChannels; ch++)
    {
        const int byteOffset = ch * 4; /*the byte offset for the current channel (word offset * 4)*/
        
        /*get step table index and predictor value from the header word for the current channel*/
        int predictor = (short)(datablock[byteOffset] | ((datablock[byteOffset + 1] << 8) & 0xff00));
        int stepIndex = datablock[byteOffset + 2];
        
        /*sanity check. reserved last byte of the header word should be 0.*/
        char reserved = datablock[byteOffset + 3];
        KWL_ASSERT(reserved == 0 && "non-zero reserved ADPCM header block byte");
        
        int byteIdx = numChannels * 4 + byteOffset; //the byte index of the first data word for this channel
        int idx = ch;
        
        while (byteIdx < codecData->nBlockAlign)
        {
            /*
             decode each nibble of the current data word, 
             containing 8 encoded samples, for the current channel 
             */
            outBuffer[idx] = decodeNibble(datablock[byteIdx] & 0x0f, &predictor, &stepIndex);
            idx += numChannels;
            outBuffer[idx] = decodeNibble((datablock[byteIdx] & 0xf0) >> 4, &predictor, &stepIndex);
            idx += numChannels;
            byteIdx++;
            
            outBuffer[idx] = decodeNibble(datablock[byteIdx] & 0x0f, &predictor, &stepIndex);
            idx += numChannels;
            outBuffer[idx] = decodeNibble((datablock[byteIdx] & 0xf0) >> 4, &predictor, &stepIndex);
            idx += numChannels;
            byteIdx++;
            
            outBuffer[idx] = decodeNibble(datablock[byteIdx] & 0x0f, &predictor, &stepIndex);
            idx += numChannels;
            outBuffer[idx] = decodeNibble((datablock[byteIdx] & 0xf0) >> 4, &predictor, &stepIndex);
            idx += numChannels;
            byteIdx++;
            
            outBuffer[idx] = decodeNibble(datablock[byteIdx] & 0x0f, &predictor, &stepIndex);
            idx += numChannels;
            outBuffer[idx] = decodeNibble((datablock[byteIdx] & 0xf0) >> 4, &predictor, &stepIndex);
            idx += numChannels;
            byteIdx++;
            
            byteIdx += (numChannels - 1) << 2; /*jump to the next data word for the current channel*/
        }
    }
    
    /* Loop if requested. */
    int isLastBlock = codecData->currentByte >= codecData->dataSize ? 1 : 0;
    if (isLastBlock != 0 &&
        decoder->loop)
    {
        int minZeroFrameIdx = nDataWordsPerChannel;
        for (int ch = 0; ch < numChannels; ch++)
        {
            int idx = ch;
            for (int i = 0; i < nDataWordsPerChannel; i++)
            {
                if(outBuffer[idx] == 0)
                {
                    if (i < minZeroFrameIdx)
                    {
                        minZeroFrameIdx = i;
                    }
                }
                
                idx += numChannels;
            }
        }
        //printf("minZeroFrameIdx %d\n", minZeroFrameIdx);
    }
    
    /*2 for 2 bytes per 16 bit output sample, 8 for 8 samples per data word.*/
    decoder->currentDecodedBufferSizeInBytes = 2 * 8 * nDataWordsPerChannel * numChannels;
    return isLastBlock;
}

int kwlRewindDecoderIMAADPCM(kwlDecoder* decoder)
{
    kwlIMAADPCMCodecData* codecData = (kwlIMAADPCMCodecData*)decoder->codecData;
    codecData->currentByte = 0;
    kwlInputStream_seek(&decoder->audioDataStream, codecData->firstDataBlockByte, SEEK_SET);
    return 1;
}
