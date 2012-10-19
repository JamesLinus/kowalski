#ifndef KWL_FILE_OUTPUTSTREAM_H
#define KWL_FILE_OUTPUTSTREAM_H

/*! \file */ 

#include <stdio.h>
#include "kwl_inputstream.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef struct kwlFileOutputStream
    {
        FILE* file;
    } kwlFileOutputStream;
    
    /**
     * Return 1 on success
     */
    int kwlFileOutputStream_initWithPath(kwlFileOutputStream* const stream, const char* const path);
    
    /**
     *
     */
    void kwlFileOutputStream_close(kwlFileOutputStream* stream);
    
    /**
     *
     */
    void kwlFileOutputStream_writeASCIIString(kwlFileOutputStream* stream, const char* str);
    
    /**
     *
     */
    void kwlFileOutputStream_write(kwlFileOutputStream* stream, const void* data, int numBytes);
    
    /**
     *
     */
    void kwlFileOutputStream_writeInt32BE(kwlFileOutputStream* stream, int val);
    
    /**
     *
     */
    void kwlFileOutputStream_writeFloat32BE(kwlFileOutputStream* stream, float val);
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL_FILE_OUTPUTSTREAM_H*/
