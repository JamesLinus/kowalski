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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "kwl_assert.h"
#include "kwl_fileutil.h"
#include "kwl_memory.h"

int kwlDoesFileExist(const char* path)
{
    FILE * file = fopen(path, "r");
    
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    
    return 0;
}

int kwlIsFileDirectory(const char* path)
{
    struct stat st;
    
    if (stat(path, &st) < 0)
    {
        return 0;
    }
    
    return S_ISDIR(st.st_mode);
}

char* kwlAppendPathElement(const char* path, const char* toAppend)
{
    size_t plen = strlen(path);
    size_t appLen = strlen(toAppend);
    
    int pEndsWithSep = 0;
    int appStartsWithSep = 0;
    
    if (plen > 0)
    {
        if (path[plen - 1] == '/')
        {
            pEndsWithSep = 1;
        }
    }

    if (appLen > 0)
    {
        if (toAppend[0] == '/')
        {
            appStartsWithSep = 1;
        }
    }
    
    const size_t finalLen = plen - (pEndsWithSep ? 1 : 0) + appLen - (appStartsWithSep ? 1 : 0) + 1;
    char* final = KWL_MALLOC(finalLen + 1, "path combine str");
    
    const size_t sepIdx = plen - (pEndsWithSep ? 1 : 0);
    kwlMemcpy(final, path, sepIdx + 1);
    final[sepIdx] = '/';
    kwlMemcpy(&final[sepIdx + 1], toAppend, appLen - (appStartsWithSep ? 1 : 0) + 1);
    
    return final;
}

char* kwlGetFileDirectory(const char* path)
{
    KWL_ASSERT(!kwlIsFileDirectory(path));
    
    const size_t len = strlen(path);
    
    int lastSepIdx;
    for (int i = len - 1; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            lastSepIdx = i;
            break;
        }
    }
    
    char* str = KWL_MALLOC(lastSepIdx + 1, "file path str");
    kwlMemcpy(str, path, lastSepIdx);
    str[lastSepIdx] = '\0';
    
    
    return str;
}

char* kwlGetAudioFilePath(const char* xmlPath,
                          const char* rootDir,
                          int rootIsRelative,
                          const char* relPath)
{
    /*
    printf("kwlGetAudioFilePath:\n");
    printf("    xmlPath  : %s\n", xmlPath);
    printf("    root     : %s\n", rootDir);
    printf("    rel root : %d\n", rootIsRelative);
    printf("    rel path : %s\n", relPath);*/
    
    char* audioFilePath = NULL;
    if (rootIsRelative)
    {
        char* xmlRootDir = kwlGetFileDirectory(xmlPath);
        //printf("    xml root : %s\n", xmlRootDir);
        KWL_ASSERT(kwlIsFileDirectory(xmlRootDir));
        char* audioRootDir = kwlAppendPathElement(xmlRootDir, rootDir);
        audioFilePath = kwlAppendPathElement(audioRootDir, relPath);
        KWL_FREE(audioRootDir);
        KWL_FREE(xmlRootDir);
        //printf("    full path: %s\n", audioFilePath);
        return audioFilePath;
    }
    
    char* finalPath = kwlAppendPathElement(rootDir, relPath);
    
    //printf("    full path: %s\n", finalPath);
    
    return finalPath;
    
}

