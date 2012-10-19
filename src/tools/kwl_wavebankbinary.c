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

#include "kwl_fileoutputstream.h"
#include "kwl_enginedatabinary.h"
#include "kwl_wavebankbinary.h"
#include "kwl_xmlutil.h"

void kwlWaveBankBinary_writeToFile(kwlWaveBankBinary* bin,
                                   const char* path)
{
    kwlFileOutputStream fos;
    int success = kwlFileOutputStream_initWithPath(&fos, path);
    if (!success)
    {
        KWL_ASSERT(0 && "could not open file for writing. TODO: proper error handling here");
        return;
    }
    
    /*write file identifier*/
    kwlFileOutputStream_write(&fos, bin->fileIdentifier, KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH);
    
    /*write id and entry count*/
    kwlFileOutputStream_writeASCIIString(&fos, bin->id);
    kwlFileOutputStream_writeInt32BE(&fos, bin->numEntries);
    
    /*write entries*/
    for (int i = 0; i < bin->numEntries; i++)
    {
        kwlWaveBankEntryChunk* ei = &bin->entries[i];
        kwlFileOutputStream_writeASCIIString(&fos, ei->fileName);
        kwlFileOutputStream_writeInt32BE(&fos, ei->encoding);
        kwlFileOutputStream_writeInt32BE(&fos, ei->isStreaming);
        kwlFileOutputStream_writeInt32BE(&fos, ei->numChannels);
        kwlFileOutputStream_writeInt32BE(&fos, ei->numBytes);
        kwlFileOutputStream_write(&fos, ei->data, ei->numBytes);
    }
    
    /*done*/
    kwlFileOutputStream_close(&fos);
}

kwlDataValidationResult kwlWaveBankBinary_loadFromBinaryFile(kwlWaveBankBinary* binaryRep,
                                                         const char* path,
                                                         kwlLogCallback errorLogCallback)
{
    kwlLogCallback errorCallback = errorLogCallback == NULL ? kwlSilentLogCallback : errorLogCallback;
    
    kwlMemset(binaryRep, 0, sizeof(kwlWaveBankBinary));
    
    kwlInputStream stream;
    kwlError result = kwlInputStream_initWithFile(&stream, path);
    if (result != KWL_NO_ERROR)
    {
        //error loading file
        errorCallback("Could not open wave bank binary %s\n", path);
        return KWL_COULD_NOT_OPEN_WAVE_BANK_BINARY_FILE;
    }
    
    kwlDataValidationResult error = KWL_DATA_IS_VALID;
    
    /*... and check the wave bank file identifier.*/
    for (int i = 0; i < KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        const char identifierChari = kwlInputStream_readChar(&stream);
        binaryRep->fileIdentifier[i] = identifierChari;
        if (identifierChari != KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER[i])
        {
            /* Not the file identifier we expected. */
            errorLogCallback("Invalid wave bank binary file header\n");
            error = KWL_INVALID_FILE_IDENTIFIER;
            goto onDataError;
        }
    }
    
    binaryRep->id = kwlInputStream_readASCIIString(&stream);
    binaryRep->numEntries = kwlInputStream_readIntBE(&stream);
    binaryRep->entries = KWL_MALLOCANDZERO(binaryRep->numEntries * sizeof(kwlWaveBankEntryChunk),
                                           "bin wb entries");
    
    for (int i = 0; i < binaryRep->numEntries; i++)
    {
        kwlWaveBankEntryChunk* ei = &binaryRep->entries[i];
        ei->fileName = kwlInputStream_readASCIIString(&stream);
        ei->encoding = kwlInputStream_readIntBE(&stream);
        ei->isStreaming = kwlInputStream_readIntBE(&stream);
        ei->numChannels = kwlInputStream_readIntBE(&stream);
        ei->numBytes = kwlInputStream_readIntBE(&stream);
        KWL_ASSERT(ei->numBytes >= 0);
        ei->data = KWL_MALLOC(ei->numBytes, "bin wb audio data entry");
        kwlInputStream_read(&stream, ei->data, ei->numBytes);
    }
    
    kwlInputStream_close(&stream);
    return KWL_DATA_IS_VALID;
    
onDataError:
    kwlInputStream_close(&stream);
    kwlWaveBankBinary_free(binaryRep);
    return error;
}

static const char* kwlGetAudioFilePath(const char* xmlPath,
                                       const char* rootDir,
                                       int rootIsRelative,
                                       const char* relPath)
{
    //TODO:
    return relPath;
}

static char* kwlDuplicateString(const char* str)
{
    size_t len = strlen(str);
    char* copy = KWL_MALLOC(len + 1, "string copy");
    kwlMemcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

kwlDataValidationResult kwlWaveBankBinary_loadFromXML(kwlWaveBankBinary* bin,
                                                      const char* xmlPath,
                                                      const char* xsdPath,
                                                      const char* waveBankId,
                                                      kwlLogCallback errorLogCallback)
{
    /*grab the audio file root path*/
    xmlDocPtr doc = NULL;
    kwlDataValidationResult result = kwlLoadAndValidateProjectDataDoc(xmlPath, xsdPath, &doc, errorLogCallback);
    if (result != KWL_DATA_IS_VALID)
    {
        return result;
    }
    
    xmlNode* projNode = xmlDocGetRootElement(doc);
    char* audioFileRoot = kwlGetAttributeValueCopy(projNode, KWL_XML_ATTR_PROJECT_AUDIO_FILE_ROOT);
    int rootIsRelative = kwlGetBoolAttributeValue(projNode, KWL_XML_ATTR_PROJECT_AUDIO_FILE_ROOT_IS_RELATIVE);
    xmlFreeDoc(doc);
    
    /*load project data*/
    kwlEngineDataBinary projBin;
    kwlEngineDataBinary_loadFromXML(&projBin,
                                    xmlPath,
                                    xsdPath,
                                    errorLogCallback);
    
    /*find the wavebank*/
    kwlWaveBankChunk* waveBank = NULL;
    for (int i = 0; i < projBin.waveBankChunk.numWaveBanks; i++)
    {
        kwlWaveBankChunk* wbi = &projBin.waveBankChunk.waveBanks[i];
        if (strcmp(wbi->id, waveBankId) == 0)
        {
            waveBank = wbi;
            break;
        }
    }
    
    if (waveBank == NULL)
    {
        errorLogCallback("The wave bank '%s' could not be found in project data file %s\n",
                         waveBankId, xmlPath);
        return KWL_WAVE_BANK_ID_NOT_FOUND;
    }
    
    /*create wave bank binary struct and populate it using the corresponding
     wave bank from project data and the contents of the referenecd audio files*/
    kwlMemset(bin, 0, sizeof(kwlWaveBankBinary));
    bin->numEntries = waveBank->numAudioDataEntries;
    for (int i = 0; i < KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        bin->fileIdentifier[i] = KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER[i];
    }
    
    bin->id = kwlDuplicateString(waveBank->id);
    bin->entries = KWL_MALLOC(waveBank->numAudioDataEntries * sizeof(kwlWaveBankEntryChunk), "bin wb entries");
    for (int i = 0; i < waveBank->numAudioDataEntries; i++)
    {
        kwlWaveBankEntryChunk* ei = &bin->entries[i];
        ei->fileName = kwlDuplicateString(waveBank->audioDataEntries[i]);
        const char* audioFilePath = kwlGetAudioFilePath(xmlPath, audioFileRoot, rootIsRelative, ei->fileName);
        //TODO: load audio data from file
    }
    
    /*clean up*/
    kwlEngineDataBinary_free(&projBin);
    
    return KWL_DATA_IS_VALID;
}

void kwlWaveBankBinary_dump(kwlWaveBankBinary* bin,
                            kwlLogCallback lcb)
{
    kwlLogCallback logCallback = lcb == NULL ? kwlSilentLogCallback : lcb;
    
    logCallback("Kowalski wave bank binary (");
    logCallback("file ID: ");
    for (int i = 0; i < KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        logCallback("%d ", (int)bin->fileIdentifier[i]);
    }
    logCallback(")\n");
    
    logCallback("    %s (%d entries):\n", bin->id, bin->numEntries);
    
    for (int i = 0; i < bin->numEntries; i++)
    {
        kwlWaveBankEntryChunk* ei = &bin->entries[i];
        logCallback("        %s (encoding %d, streaming %d, %d channel(s), %d bytes)\n",
                    ei->fileName, ei->encoding, ei->isStreaming, ei->numChannels, ei->numBytes);
    }
}

void kwlWaveBankBinary_free(kwlWaveBankBinary* bin)
{
    KWL_FREE(bin->id);
    
    for (int i = 0; i < bin->numEntries; i++)
    {
        kwlWaveBankEntryChunk* ei = &bin->entries[i];
        KWL_FREE(ei->fileName);
        KWL_FREE(ei->data);
    }
    
    KWL_FREE(bin->entries);
    
    kwlMemset(bin, 0, sizeof(kwlWaveBankBinary));
}
