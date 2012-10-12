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
#include "kwl_decoder_oggvorbis.h"
#include "kwl_inputstream.h"
#include "kwl_memory.h"

#include "kwl_assert.h"

kwlError kwlInitDecoderOggVorbis(kwlDecoder* decoder)
{
    /*Allocate decoder data.*/
    kwlOggVorbisDecoderData* data = 
        (kwlOggVorbisDecoderData*)KWL_MALLOC(sizeof(kwlOggVorbisDecoderData), "ogg vorbis decoder data");
    kwlMemset(data, 0, sizeof(kwlOggVorbisDecoderData));

    /* Hook up data and callbacks to the decoder.*/
    decoder->codecData = data;
    decoder->decodeBuffer = &kwlDecodeBufferOggVorbis;
    decoder->deinit = &kwlDeinitDecoderOggVorbis;
    decoder->rewind = &kwlRewindDecoderOggVorbis;
    
    /*Open the file for reading, providing data from the decoder input stream.*/
    ov_callbacks callbacks;
    callbacks.read_func = &ovReadCallback;
    callbacks.seek_func = &ovSeekCallback;
    callbacks.tell_func = &ovTellCallback;
    callbacks.close_func = &ovCloseCallback;

    int result = ov_open_callbacks(&decoder->audioDataStream, 
                                   &data->oggVorbisFile, 
                                   NULL, 
                                   0, 
                                   callbacks);
    
    if(result < 0)
    {
        return KWL_UNKNOWN_FILE_FORMAT;
    }
     
    vorbis_info* info = ov_info(&data->oggVorbisFile, -1);
     
    decoder->numChannels = info->channels;
    KWL_ASSERT(decoder->numChannels == 1 || decoder->numChannels == 2);
    /*printf("opened ov stream, %d channels\n", decoder->numChannels);*/
    
    decoder->maxDecodedBufferSize = KWL_OGG_NUM_BUFFERED_FRAMES; //TODO: really frames?
    
    
    
    return KWL_NO_ERROR;
}

void kwlDeinitDecoderOggVorbis(kwlDecoder* decoder)
{
    kwlOggVorbisDecoderData* data = (kwlOggVorbisDecoderData*)decoder->codecData;
    OggVorbis_File* file = &data->oggVorbisFile;
    ov_clear(file);
    KWL_FREE(decoder->codecData);
}

int kwlDecodeBufferOggVorbis(kwlDecoder* decoder)
{
    kwlOggVorbisDecoderData* data = (kwlOggVorbisDecoderData*)decoder->codecData;
    
    /*clear the buffer of decoded samples...*/
    decoder->currentDecodedBufferSizeInBytes = 0;
    kwlMemset(decoder->currentDecodedBuffer, 0, decoder->maxDecodedBufferSize);
    /*TODO: sort out bytes vs frames!
     ...and fill it with new samples*/
    //printf("kwlDecodeBufferOggVorbis\n");
    while (decoder->currentDecodedBufferSizeInBytes < decoder->maxDecodedBufferSize)
    {
        int currentSection;
        /*dont request more bytes than we need to fill the current output buffer.*/
        int bytesToRead = decoder->maxDecodedBufferSize - decoder->currentDecodedBufferSizeInBytes;
        int numReadBytes = ov_read(&data->oggVorbisFile, 
                                   (char*)(&decoder->currentDecodedBuffer[decoder->currentDecodedBufferSizeInBytes >> 1]), 
                                   bytesToRead, 
                                   &currentSection);
        decoder->currentDecodedBufferSizeInBytes += numReadBytes;
        /*printf("decoder->currentDecodedBufferSizeInBytes %d\n", decoder->currentDecodedBufferSizeInBytes);*/
        if (numReadBytes == 0)
        {
            /*end of file reached, signal that the stream has been fully decoded.*/
            return 1;
        }
        else if (numReadBytes < 1)
        {
            KWL_ASSERT(0 && "error reading ogg vorbis stream ");
            return 1; /*TODO: handle diffrently?*/
        }
    }
    
    /*if we made it here, a new buffer was decoded without problems and without reaching the
     end of the ogg vorbis stream.*/
    return 0;
}

size_t ovReadCallback(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    kwlInputStream* audioDataStream = (kwlInputStream*)datasource;
    int bytesRead = kwlInputStream_read(audioDataStream, (signed char*)ptr, size * nmemb);
    return bytesRead / size;
}

int ovSeekCallback(void *datasource, ogg_int64_t offset, int whence)
{
    kwlInputStream* stream = (kwlInputStream*)datasource;
    return kwlInputStream_seek(stream, offset, whence);
}

int ovCloseCallback(void *datasource)
{
    kwlInputStream* stream = (kwlInputStream*)datasource;
    kwlInputStream_close(stream);
    /*TODO: return something here*/
	return 0; /*TODO: check this*/
}

long ovTellCallback(void *datasource)
{
    kwlInputStream* stream = (kwlInputStream*)datasource;
    return kwlInputStream_tell(stream);
}

int kwlRewindDecoderOggVorbis(kwlDecoder* decoder)
{
    kwlOggVorbisDecoderData* data = (kwlOggVorbisDecoderData*)decoder->codecData;
    int result = ov_pcm_seek(&data->oggVorbisFile, 0);
    if (result == 0)
    {
        return 1;
    }
    else
    {
        switch (result) {
            case OV_ENOSEEK:
                KWL_ASSERT(0 && "Bitstream is not seekable");
                break;
            case OV_EINVAL:
                KWL_ASSERT(0 && "Invalid argument value; possibly called with an OggVorbis_File structure that isn't open.");
                break;
            case OV_EREAD:
                KWL_ASSERT(0 && "A read from media returned an error.");
                break;
            case OV_EFAULT:
                KWL_ASSERT(0 && "Internal logic fault; indicates a bug or heap/stack corruption.");
                break;
            case OV_EBADLINK:
                KWL_ASSERT(0 && "Invalid stream section supplied to libvorbisfile, or the requested link is corrupt.");
                break;
            default:
                break;
        }
    }
    
    return 0;
}
