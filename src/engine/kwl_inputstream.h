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

#ifndef KWL__INPUT_STREAM_H
#define KWL__INPUT_STREAM_H

/*! \file */ 

#include "kowalski.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * A struct representing an input stream, getting its data from 
 * either a file or a buffer.
 */
typedef struct kwlInputStream
{
    /** A pointer to a file providing the stream with data (NULL if the stream gets its data from a buffer). */
    FILE* file;
    /** A pointer to a buffer providing the stream with data (NULL if the stream gets its data from a file). */
    void* buffer;
    /** 
     * The size in bytes of the stream data source. A negative size indicates that the size is unknown,
     * which is the case when reading files.
     */
    int size;
    /** The offset in bytes into the underlying data.*/
    int offset;
    /** The current byte position, relative to the start of the underlying data. */
    int readPos;
} kwlInputStream;

void kwlInputStream_init(kwlInputStream* stream);
    
void kwlInputStream_free(kwlInputStream* stream);
    
/** 
 * Opens a file and returns a pointer to it.
 * @param stream A pointer to the kwlInputStream struct.
 * @param path The path of the file to open. 
 * @return A pointer to the newly opened file or NULL if the file could not be opened.
 */
FILE* kwlInputStream_openFile(kwlInputStream* stream, const char* const path);

/** 
 * Initializes an input stream getting its data from a memory buffer region.
 * @param stream The input stream to initialize.
 * @param buffer The buffer to associate \c stream with. Must have a size of at least \c offset \c + \c size.
 * @param offset The byte offset into the buffer.
 * @param size The size of the region.
 */
void kwlInputStream_initWithBuffer(kwlInputStream* const stream, void* buffer, int offset, int size);

/** 
 * Initializes an input stream getting its data from a region within a file.
 * @param stream The input stream to initialize.
 * @param path The path to the file to associate \c stream with.
 * @param offset The byte offset into the file.
 * @param size The size of the region.
 * @return Returns \c KWL_FILE_NOT_FOUND if the specified file does not exist and \c KWL_NO_ERROR otherwise.
 */
kwlError kwlInputStream_initWithFileRegion(kwlInputStream* const stream, const char* const path, int offset, int size);

/** 
 * Initializes the input stream, getting its data from a given file.
 * @param stream The input stream to initialize.
 * @param path The path to the file to associate \c stream with.
 * @return Returns \c KWL_FILE_NOT_FOUND if the specified file does not exist and \c KWL_NO_ERROR otherwise.
 */
kwlError kwlInputStream_initWithFile(kwlInputStream* const stream, const char* const path);
    
/** 
 * Closes a stream and disposes of the underlying file, if any. Any memory buffer associated with the
 * stream is NOT freed.
 * @param stream The input stream to close.
 */
void kwlInputStream_close(kwlInputStream* const stream);

/** 
 * Moves the read position of a file stream a given number of bytes. 
 * @param stream The input stream to advance the read position of.
 * @param size The number of bytes by which to advance the read position.
 */
void kwlInputStream_skip(kwlInputStream* const stream, size_t size);

/** 
 * Moves the read position of a given input stream to the beginning of the stream. 
 * @param stream The input stream to reset the read position of.
 */
void kwlInputStream_reset(kwlInputStream* const stream);
    
/** 
 * Returns a non-zero integer if the stream read position is at the end of the stream and
 * zero otherwise. 
 * @param stream The input stream to check.
 * @return A non-zero integer if the read position of \c stream is at the end of the stream 
 * and zero otherwise.
 */
int kwlInputStream_isAtEndOfStream(kwlInputStream* const stream);

/**
 *
 */
int kwlInputStream_tell(kwlInputStream* const stream);

/**
 *
 */
int kwlInputStream_seek(kwlInputStream* const stream, long pos, int p);
    
/** 
 * Tries to reads a specified number of bytes from a given stream and 
 * advances the read position by the number of bytes actually read. 
 * @param stream The input stream to read from.
 * @param data The buffer to put read data in. Must have a size of at least \c length bytes.
 * @param length The number of bytes to read from the stream.
 * @return The number of bytes actually read.
 */
int kwlInputStream_read(kwlInputStream* const stream, signed char* data, int length);

/** 
 * Reads a \c char from a given stream and advances the read position by one byte. 
 * @param stream The input stream to read from.
 * @return The read \c char. 
 */
char kwlInputStream_readChar(kwlInputStream* const stream);    

/** 
 * Reads an ASCII string from a stream and advances the read position accordingly. 
 * The string is assumed to be encoded as an integer (4 bytes, big endian byte order) defining the length of the string,
 * followed by the chars of the string. Note that this method allocates a new char array that must
 * be freed by the caller.
 * @param stream The input stream to read from.
 * @return The null terminated ASCII string. 
 */
char* kwlInputStream_readASCIIString(kwlInputStream* const stream);

/** 
 * Reads an \c int (big endian byte order) from a given stream and advances the read position by four bytes. 
 * @param stream The input stream to read from.
 * @return The read \c int.
 */
int kwlInputStream_readIntBE(kwlInputStream* const stream);

/** 
 * Reads an \c int (litte endian byte order) from a given stream and advances the read position by four bytes. 
 * @param stream The input stream to read from.
 * @return The read \c int.
 */
int kwlInputStream_readIntLE(kwlInputStream* const stream);
    
/** 
 * Reads a \c short (big endian byte order) from a given stream and advances the read position by two bytes. 
 * @param stream The input stream to read from.
 * @return The read \c short.
 */
short kwlInputStream_readShortBE(kwlInputStream* const stream);

/** 
 * Reads a \c short (little endian byte order) from a given stream and advances the read position by two bytes. 
 * @param stream The input stream to read from.
 * @return The read \c short.
 */
short kwlInputStream_readShortLE(kwlInputStream* const stream);
        
/** 
 * Reads a \c float (big endian byte order) from a given stream and advances the read position by four bytes. 
 * @param stream The input stream to read from.
 * @return The read \c float.
 */
float kwlInputStream_readFloatBE(kwlInputStream* const stream);

/** 
 * Reads a \c float (little endian byte order) from a given stream and advances the read position by four bytes. 
 * @param stream The input stream to read from.
 * @return The read \c float.
 */
float kwlInputStream_readFloatLE(kwlInputStream* const stream);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__INPUT_STREAM_H*/
