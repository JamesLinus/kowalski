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

#include <string.h>

#include "kwl_binarybuilding.h"
#include "kwl_enginedatabinary.h"
#include "kwl_wavebankbinary.h"
#include "kwl_xmlutil.h"
#include "kwl_fileutil.h"

kwlResultCode kwlBuildEngineData(const char* xmlPath,
                                 const char* xsdPath,
                                 const char* targetFile,
                                 kwlLogCallback errorLogCallback)
{
    kwlEngineDataBinary edb;
    kwlResultCode result = kwlEngineDataBinary_loadFromXMLFile(&edb,
                                                               xmlPath,
                                                               xsdPath,
                                                               0,
                                                               errorLogCallback);
    
    if (result != KWL_SUCCESS)
    {
        return result;
    }
    
    result = kwlEngineDataBinary_writeToFile(&edb, targetFile);
    
    if (result != KWL_SUCCESS)
    {
        return result;
    }
    
    kwlEngineDataBinary_free(&edb);
    
    return KWL_SUCCESS;
}

kwlResultCode kwlBuildWaveBanks(const char* xmlPath,
                                const char* xsdPath,
                                const char* targetDir,
                                kwlLogCallback errorLogCallback)
{
    if (!kwlIsFileDirectory(targetDir))
    {
        /**/
        errorLogCallback("Wave bank target location '%s' is not a directory.\n", targetDir);
        return KWL_INVALID_PATH;
    }
    
    /*grab the audio file root path*/
    xmlDocPtr doc = NULL;
    kwlResultCode result = kwlLoadAndValidateProjectDataDoc(xmlPath, xsdPath, &doc, errorLogCallback);
    if (result != KWL_SUCCESS)
    {
        return result;
    }
    
    xmlNode* projNode = xmlDocGetRootElement(doc);
    char* audioFileRoot = kwlGetAttributeValueCopy(projNode, KWL_XML_ATTR_PROJECT_AUDIO_FILE_ROOT);
    int rootIsRelative = kwlGetBoolAttributeValue(projNode, KWL_XML_ATTR_PROJECT_AUDIO_FILE_ROOT_IS_RELATIVE);
    
    kwlEngineDataBinary edb;
    kwlResultCode r = kwlEngineDataBinary_loadFromXMLDocument(&edb,
                                                              xmlPath,
                                                              doc,
                                                              errorLogCallback);
    
    xmlFreeDoc(doc);
    
    if (r != KWL_SUCCESS)
    {
        return r;
    }
    
    kwlResultCode finalResult = KWL_SUCCESS;
    
    for (int i = 0; i < edb.waveBankChunk.numWaveBanks; i++)
    {
        kwlWaveBankChunk* edwb = &edb.waveBankChunk.waveBanks[i];
        
        /*create a wave bank structure...*/
        const char* wbId = edb.waveBankChunk.waveBanks[i].id;
           
        kwlWaveBankBinary wbBin;
        kwlResultCode wbResult = kwlWaveBankBinary_create(&wbBin,
                                                          &edb,
                                                          xmlPath,
                                                          audioFileRoot,
                                                          rootIsRelative,
                                                          wbId,
                                                          errorLogCallback);
        
        if (wbResult != KWL_SUCCESS)
        {
            finalResult = wbResult;
        }
        else
        {
            /*create the path of the output file*/
            char* wbFilePathNoExt = kwlAppendPathElement(targetDir, wbId);
            size_t fullPathLen = strlen(wbFilePathNoExt) + 5;
            char* wbFilePath = KWL_MALLOCANDZERO(fullPathLen * sizeof(char), "wb path w ext");
            strcpy(wbFilePath, wbFilePathNoExt);
            
            wbFilePath[fullPathLen - 5] = '.';
            wbFilePath[fullPathLen - 4] = 'k';
            wbFilePath[fullPathLen - 3] = 'w';
            wbFilePath[fullPathLen - 2] = 'b';
            wbFilePath[fullPathLen - 1] = '\0';
            
            /*do dependency checking.*/
            int shouldWrite = 1;
            if (kwlDoesFileExist(wbFilePath))
            {
                /*only write the binary
                 if the project data or any containing audio file
                 has more recent changes than the wave bank file that
                 already exists*/
                long wbTimestamp = kwlGetFileTimeStamp(wbFilePath);
                long projectTimestamp = kwlGetFileTimeStamp(xmlPath);
                long latestAudioFileTimeStamp = 0;
                for (int j = 0; j < edwb->numAudioDataEntries; j++)
                {
                    char* audioFilePath = kwlGetAudioFilePath(xmlPath,
                                                              audioFileRoot,
                                                              rootIsRelative,
                                                              edwb->audioDataEntries[j]);
                    long timeStamp = kwlGetFileTimeStamp(audioFilePath);
                    if (timeStamp > latestAudioFileTimeStamp)
                    {
                        latestAudioFileTimeStamp = timeStamp;
                    }
                    KWL_FREE(audioFilePath);
                }
                
                if (wbTimestamp > projectTimestamp &&
                    wbTimestamp > latestAudioFileTimeStamp)
                {
                    errorLogCallback("Wave bank file '%s' is up to date.\n", wbFilePath);
                    shouldWrite = 0;
                }
            }
            
            if (shouldWrite)
            {
                errorLogCallback("Building wave bank file '%s'.\n", wbFilePath);
                kwlWaveBankBinary_writeToFile(&wbBin, wbFilePath);
            }
            
            KWL_FREE(wbFilePath);
            KWL_FREE(wbFilePathNoExt);
        }
    }
    
    return finalResult;
}
