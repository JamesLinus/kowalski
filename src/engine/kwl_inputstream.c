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
#include "kwl_inputstream.h"
#include "kwl_memory.h"


#ifdef _WIN32
#include <stdlib.h>
#include <fcntl.h>
#endif

void kwlInputStream_free(kwlInputStream* stream)
{
    kwlInputStream_close(stream);
    KWL_FREE(stream);
}

void kwlInputStream_init(kwlInputStream* stream)
{
    stream->file = NULL;
    stream->size = 0;
    stream->offset = 0;
    stream->readPos = 0;
    stream->buffer = NULL;
}

FILE* kwlInputStream_openFile(kwlInputStream* stream, const char* const path)
{
#ifdef _WIN32
    /*make sure we're in binary mode to avoid nasty line ending surprises*/
    _set_fmode(_O_BINARY);
	return fopen(path, "r");
#else
	return fopen(path, "r");
#endif
}

/** */
kwlError kwlInputStream_initWithFile(kwlInputStream* const stream, const char* const path)
{
    kwlInputStream_init(stream);
    
    stream->file = kwlInputStream_openFile(stream, path);
    if (stream->file == NULL)
    {
        return KWL_FILE_NOT_FOUND;
    }
    
    stream->size = -1; /*negative size signals that the size is unknown*/
    stream->offset = 0;
    stream->readPos = 0;
    stream->buffer = NULL;
    fseek(stream->file, stream->offset, SEEK_SET);
     
    return KWL_NO_ERROR;
}

kwlError kwlInputStream_initWithFileRegion(kwlInputStream* const stream, const char* const path, int offset, int size)
{
    kwlInputStream_init(stream);
    KWL_ASSERT(size > 0);
    KWL_ASSERT(offset >= 0);
    
    stream->file = kwlInputStream_openFile(stream, path);
    if (stream->file == NULL)
    {
        return KWL_FILE_NOT_FOUND;
    }
    
    stream->size = size;
    stream->offset = offset;
    stream->readPos = offset;
    stream->buffer = NULL;
    fseek(stream->file, stream->offset, SEEK_SET);

    return KWL_NO_ERROR;
}

void kwlInputStream_initWithBuffer(kwlInputStream* const stream, void* buffer, int offset, int size)
{
    kwlInputStream_init(stream);
    KWL_ASSERT(size > 0);
    KWL_ASSERT(offset >= 0);
    
    stream->size = size;
    stream->offset = offset;
    stream->readPos = offset;
    stream->buffer = buffer;
    stream->file = NULL;
}

void kwlInputStream_skip(kwlInputStream* const stream, size_t size)
{
    if (stream->file != NULL && stream->buffer == NULL)
    {
        KWL_ASSERT(stream->file != NULL);
        fseek(stream->file, size, SEEK_CUR);
        stream->readPos = ftell(stream->file);
    }
    else if (stream->file == NULL && stream->buffer != NULL)
    {
        stream->readPos += size;
        if (stream->readPos > stream->offset + stream->size)
        {
            stream->readPos = stream->offset + stream->size;    
        }
    }
    else 
    {
        KWL_ASSERT(0);
    }

}

/** */
void kwlInputStream_reset(kwlInputStream* const stream)
{
    if (stream->file != NULL && stream->buffer == NULL)
    {
        fseek(stream->file, stream->offset, SEEK_SET);
    }
    else if (stream->file == NULL && stream->buffer != NULL)
    {
        stream->readPos = stream->offset;
    }
    else 
    {
        KWL_ASSERT(0);
    }
    
}

int kwlInputStream_isAtEndOfStream(kwlInputStream* const stream)
{
    if (stream->file != NULL && stream->buffer == NULL)
    {
        if (stream->size < 0)
        {
            return feof(stream->file);
        }
        else 
        {
            return stream->readPos >= stream->offset + stream->size;
        }
    }
    else if (stream->file == NULL && stream->buffer != NULL)
    {
        return stream->readPos >= stream->offset + stream->size;
    }
    else 
    {
		return 0;
        KWL_ASSERT(0);
    }
}

int kwlInputStream_tell(kwlInputStream* const stream)
{
    if (stream->file != NULL && stream->buffer == NULL)
    {
        return ftell(stream->file) - stream->offset;
    }
    else if (stream->file == NULL && stream->buffer != NULL)
    {
        return stream->readPos;
    }
    else 
    {
        KWL_ASSERT(0);
		return -1;
    }
}


int kwlInputStream_seek(kwlInputStream* const stream, long pos, int p)
{
    if (stream->file != NULL && stream->buffer == NULL)
    {
        if (p == SEEK_SET)
        {
            stream->readPos = stream->offset + pos;
            return fseek(stream->file, stream->offset + pos, p);
        }
        else if (p == SEEK_CUR)
        {
            return fseek(stream->file, pos, p);
        }
        else if (p == SEEK_END)
        {
            if (stream->size < 0)
            {
                /*if the size is unknown, assume we're getting data from a file*/
                fseek(stream->file, pos, p);
            }
            else
            {
                /*if we're getting data from file region, first go to the end of the region...*/
                fseek(stream->file, stream->offset + stream->size, SEEK_SET);
                /*then seek relative to the current position (ie the end).*/
                fseek(stream->file, pos, SEEK_CUR);
            }
        }        
        else
        {
            KWL_ASSERT(0);
        }
    }
    else if (stream->file == NULL && stream->buffer != NULL)
    {
        if (p == SEEK_SET)
        {
            stream->readPos = stream->offset + pos;
        }
        else if (p == SEEK_CUR)
        {
            stream->readPos += pos;
        }
        else if (p == SEEK_END)
        {
            stream->readPos = stream->offset + stream->size + pos;
        }
        else
        {
            KWL_ASSERT(0);
        }
        if (stream->readPos < stream->offset || stream->readPos > stream->offset + stream->size)
        {
            stream->readPos = stream->offset; /*TODO: handle properly.*/
            return -1;
        }
        return 0;
    }
    else 
    {
        KWL_ASSERT(0 && "both the file and the buffer of the input stream are NULL");
    }
    
    return 0;
}

int kwlInputStream_read(kwlInputStream* const stream, signed char* data, int length)
{
    if (stream->file != NULL && stream->buffer == NULL)
    {
        int bytesToRead = length;
        
        /*Don't read past the end of streams with a known size*/
        if (stream->readPos + length > stream->offset + stream->size && 
            stream->size > 0)
        {
            bytesToRead = stream->offset + stream->size - stream->readPos;
            KWL_ASSERT(bytesToRead >= 0);
        }
        
        /* File based stream.*/
        int bytesRead = fread(data, 1, bytesToRead, stream->file);
        stream->readPos += bytesRead;
        return bytesRead;
    }
    else if (stream->file == NULL && stream->buffer != NULL)
    {
        /* Buffer based stream.*/
        int bytesToRead = length;
        if (stream->readPos + length > stream->offset + stream->size)
        {
            bytesToRead = stream->offset + stream->size - stream->readPos;
            KWL_ASSERT(bytesToRead >= 0);
        }
        
        kwlMemcpy(data, &((char*)stream->buffer)[stream->readPos], bytesToRead);
        stream->readPos += bytesToRead;
        return bytesToRead;
    }
    else 
    {
        KWL_ASSERT(0 && "stream must have either a file or a buffer");
		return 0;
    }
}

int kwlInputStream_readIntBE(kwlInputStream* const stream)
{
    signed char c[4];
    int r = kwlInputStream_read(stream, c, 4);
    KWL_ASSERT(r == 4);
    const int ret = ((c[0] & 0xff) << 24) |
                    ((c[1] & 0xff) << 16) |
                    ((c[2] & 0xff) << 8) |
                    ((c[3] & 0xff) << 0);
    
    return ret;
}

int kwlInputStream_readIntLE(kwlInputStream* const stream)
{
    signed char c[4];
    int r = kwlInputStream_read(stream, c, 4);
    KWL_ASSERT(r == 4);
    const int ret = ((c[3] & 0xff) << 24) |
    ((c[2] & 0xff) << 16) |
    ((c[1] & 0xff) << 8) |
    ((c[0] & 0xff) << 0);
    
    return ret;
}


float kwlInputStream_readFloatBE(kwlInputStream* const stream)
{
    signed char c[4];
    int r = kwlInputStream_read(stream, c, 4);
    KWL_ASSERT(r == 4);
    int bits = ((c[0] & 0xff) << 24) |
                ((c[1] & 0xff) << 16) |
                ((c[2] & 0xff) << 8) |
                ((c[3] & 0xff) << 0);
    
    return *((float*)(&bits));
}

float kwlInputStream_readFloatLE(kwlInputStream* const stream)
{
    signed char c[4];
    int r = kwlInputStream_read(stream, c, 4);
    KWL_ASSERT(r == 4);
    int bits = ((c[3] & 0xff) << 24) |
    ((c[2] & 0xff) << 16) |
    ((c[1] & 0xff) << 8) |
    ((c[0] & 0xff) << 0);
    
    return *((float*)(&bits));
}

short kwlInputStream_readShortBE(kwlInputStream* const stream)
{    
    signed char c[2];
    int r = kwlInputStream_read(stream, c, 2);
    KWL_ASSERT(r == 2);

    short ret = (c[0] << 8) |
                (c[1] << 0);
    return ret;
}

short kwlInputStream_readShortLE(kwlInputStream* const stream)
{    
    signed char c[2];
    int r = kwlInputStream_read(stream, c, 2);
    KWL_ASSERT(r == 2);
    
    short ret = (c[0] << 0) |
                (c[1] << 8);
    return ret;
}

char kwlInputStream_readChar(kwlInputStream* const stream)
{    
    signed char c;
    int r = kwlInputStream_read(stream, &c, 1);
    KWL_ASSERT(r == 1);
    return c;
}

/** */
char* kwlInputStream_readASCIIString(kwlInputStream* const stream)
{
    const int stringLength = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(stringLength > 0);
    KWL_ASSERT(stringLength < 10000 && "sanity check");
    
    char* returnString = (char*)KWL_MALLOC((stringLength + 1) * sizeof(char), "kwlInputStream_readASCIIString");
    kwlMemset(returnString, 0, (stringLength + 1) * sizeof(char));
    int i;
    for (i = 0; i < stringLength; i++)
    {
        returnString[i] = kwlInputStream_readChar(stream);
        /*printf("char %d: %c", i, returnString[i]);*/
    }
    returnString[stringLength] = '\0';
    
    return returnString;
}

/** */
void kwlInputStream_close(kwlInputStream* const stream)
{
    if (stream->file != NULL)
    {
        fclose(stream->file);
    }
    stream->size = 0;
    stream->offset = 0;
    stream->readPos = 0;
    stream->buffer = NULL;
    stream->file = NULL;
}
