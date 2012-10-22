
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "kwl_assert.h"
#include "kwl_fileoutputstream.h"

int kwlFileOutputStream_initWithPath(kwlFileOutputStream* const stream, const char* const path)
{
    memset(stream, 0, sizeof(kwlFileOutputStream));
    
    stream->file = fopen(path, "wb");
    if (stream->file == NULL)
    {
        //perror("ERROR:");
        //fprintf(stderr, "%s\n", );
        
        return 0;
    }
    
    return 1;
}

void kwlFileOutputStream_close(kwlFileOutputStream* stream)
{
    if (stream->file)
    {
        fclose(stream->file);
    }
}

void kwlFileOutputStream_writeASCIIString(kwlFileOutputStream* stream, const char* str)
{
    int l = 0;
    while (1)
    {
        if (str[l] == '\0')
        {
            break;
        }
        l++;
    }
    
    l++;
    KWL_ASSERT(l >= 2);
    
    kwlFileOutputStream_writeInt32BE(stream, l - 1);
    kwlFileOutputStream_write(stream, str, l - 1);
    
}

void kwlFileOutputStream_write(kwlFileOutputStream* stream, const void* data, int numBytes)
{
    fwrite(data, 1, numBytes, stream->file);
}

void kwlFileOutputStream_writeInt32BE(kwlFileOutputStream* stream, int val)
{
    char c[4] =
    {
        0xff & ((val & 0xff000000) >> 24),
        0xff & ((val & 0x00ff0000) >> 16),
        0xff & ((val & 0x0000ff00) >> 8),
        0xff & ((val & 0x000000ff) << 0),
    };
    
    kwlFileOutputStream_write(stream, c, 4);
}


void kwlFileOutputStream_writeFloat32BE(kwlFileOutputStream* stream, float val)
{
    kwlFileOutputStream_writeInt32BE(stream, *((int*)(&val)));
}
