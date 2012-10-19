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

#include "kwl_wavebankbinaryrepresentation.h"

void kwlWaveBankBinaryRepresentation_saveToBinary(kwlWaveBankBinaryRepresentation* bin,
                                               const char* path)
{
    
}

void kwlWaveBankBinaryRepresentation_loadFromBinary(kwlWaveBankBinaryRepresentation* binaryRep,
                                                 const char* path,
                                                 kwlLogCallback errorLogCallback)
{
    kwlLogCallback errorCallback = errorLogCallback == NULL ? kwlSilentLogCallback : errorLogCallback;
    
    kwlMemset(binaryRep, 0, sizeof(kwlWaveBankBinaryRepresentation));
    
    kwlInputStream stream;
    kwlError result = kwlInputStream_initWithFile(&stream, path);
    if (result != KWL_NO_ERROR)
    {
        //error loading file
        errorCallback("Could not open wave bank binary %s\n", path);
        return;
    }
    
    /*... and check the wave bank file identifier.*/
    for (int i = 0; i < KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        const char identifierChari = kwlInputStream_readChar(&stream);
        binaryRep->fileIdentifier[i] = identifierChari;
        if (identifierChari != KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER[i])
        {
            /* Not the file identifier we expected. */
            errorLogCallback("Invalid wave bank binary file header\n");
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
        kwlInputStream_skip(&stream, ei->numBytes);
    }
    
    kwlInputStream_close(&stream);
    return;
    
onDataError:
    kwlInputStream_close(&stream);
    kwlWaveBankBinaryRepresentation_free(binaryRep);
    return;
}

void kwlWaveBankBinaryRepresentation_dump(kwlWaveBankBinaryRepresentation* bin,
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

void kwlWaveBankBinaryRepresentation_free(kwlWaveBankBinaryRepresentation* bin)
{
    
}
