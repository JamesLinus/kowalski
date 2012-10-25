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

#ifndef KWL_FILE_UTIL_H
#define KWL_FILE_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     * Checks if a path corresponds to an existing file.
     * @param path The path to check.
     * @return Non-zero if the file exists, zero otherwise.
     */
    int kwlDoesFileExist(const char* path);
    
    /**
     * Checks if a given file is a directory.
     * @param path The file to check.
     * @return Non-zero if the file is a directory, zero if it isn't
     * (or if it doesn't exist).
     */
    int kwlIsFileDirectory(const char* path);
    
    /**
     * Gets the time stamp of a file.
     * @param path The file.
     * @return The time stamp.
     */
    long kwlGetFileTimeStamp(const char* path);
    
    /**
     * Appends a path element to a base path.
     * The returned string must be freed by the caller.
     * @param path The base path.
     * @param toAppend The path element to append.
     * @return The combined path.
     */
    char* kwlAppendPathElement(const char* path, const char* toAppend);
    
    /**
     * Gets the path to the directory containing a given file.
     * @param path The file to get the containing directory of.
     * @return The containing directory of \c path (or NULL if \c path
     * does not exist).
     */
    char* kwlGetFileDirectory(const char* path);
    
    /**
     * Returns the absolute path to an audio file referenced
     * from a project data XML file.
     * @param xmlPath The absolute path to the XML file.
     * @param rootDir The audio file root directory specified in the XML.
     * @param rootIsRelative Non-zero if \rootDir is relative to the directory
     * containing the XML file, zero if \rootDir is an absolute path.
     * @param relPath The path of the audio file, relative to \c rootDir.
     * @return The absolute path to the audio file (no check is made to see if the file actually exists)
     */
    char* kwlGetAudioFilePath(const char* xmlPath,
                              const char* rootDir,
                              int rootIsRelative,
                              const char* relPath);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*KWL_FILE_UTIL_H*/
