#ifndef KWL_FILE_OUTPUTSTREAM_H
#define KWL_FILE_OUTPUTSTREAM_H

/*! \file */

#include <stdio.h>
#include "kwl_inputstream.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     * An output stream for writing data to a file.
     */
    typedef struct kwlFileOutputStream
    {
        FILE* file;
    } kwlFileOutputStream;
    
    /**
     * Initializes a given output stream with a file at a given location.
     * @param stream The output stream to initialize.
     * @param path The path to the file to write to.
     * @return Non-zero on success, zero otherwise.
     */
    int kwlFileOutputStream_initWithPath(kwlFileOutputStream* const stream, const char* const path);
    
    /**
     * Closes the underlying file.
     * @param stream The output stream to close.
     */
    void kwlFileOutputStream_close(kwlFileOutputStream* stream);
    
    /**
     * Writes an ASCII string (an int32 length + that many chars) to a given output stream.
     * @param stream The output stream to write to.
     * @param str The string to write.
     */
    void kwlFileOutputStream_writeASCIIString(kwlFileOutputStream* stream, const char* str);
    
    /**
     * Writes a number of bytes to a given output stream.
     * @param stream The output stream to write to.
     * @param data The data to write.
     * @param numBytes The number of bytes to write.
     */
    void kwlFileOutputStream_write(kwlFileOutputStream* stream, const void* data, int numBytes);
    
    /**
     * Writes an int32 to a given output stream.
     * @param stream The output stream to write to.
     * @param val The value to write.
     */
    void kwlFileOutputStream_writeInt32BE(kwlFileOutputStream* stream, int val);
    
    /**
     * Writes a float32 to a given output stream.
     * @param stream The output stream to write to.
     * @param val The value to write.
     */
    void kwlFileOutputStream_writeFloat32BE(kwlFileOutputStream* stream, float val);
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*KWL_FILE_OUTPUTSTREAM_H*/
