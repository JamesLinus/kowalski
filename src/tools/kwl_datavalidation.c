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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#include "kwl_datavalidation.h"
#include "kwl_inputstream.h"
#include "kwl_enginedatabinary.h"
#include "kwl_wavebankbinary.h"

kwlResultCode kwlValidate(const char* filePath, const char* schemaPath, kwlLogCallback logCallback)
{
    if (kwlFileIsWaveBankBinary(filePath))
    {
        logCallback("Validating wave bank binary file %s:\n", filePath);
        kwlWaveBankBinary wbb;
        kwlResultCode result = kwlWaveBankBinary_loadFromBinaryFile(&wbb, filePath, logCallback);
        kwlWaveBankBinary_free(&wbb);
        if (result == KWL_SUCCESS)
        {
            logCallback("No errors.\n");
        }
        return result;
    }
    else if (kwlFileIsEngineDataBinary(filePath))
    {
        logCallback("Validating engine data binary file %s:\n", filePath);
        kwlEngineDataBinary edb;
        kwlResultCode result = kwlEngineDataBinary_loadFromBinaryFile(&edb, filePath, logCallback);
        kwlEngineDataBinary_free(&edb);
        if (result == KWL_SUCCESS)
        {
            logCallback("No errors.\n");
        }
        return result;
    }
    
    /*The file is not a valid binary. See if it's a project data XML file*/
    logCallback("Validating project data XML file %s:\n", filePath);
    kwlEngineDataBinary edb;
    kwlResultCode result = kwlEngineDataBinary_loadFromXMLFile(&edb,
                                                           filePath,
                                                           schemaPath,
                                                           0,
                                                           logCallback);
    kwlEngineDataBinary_free(&edb);
    if (result == KWL_SUCCESS)
    {
        logCallback("No errors.\n");
    }
    return result;
}

kwlResultCode kwlValidateProjectData(const char* xmlPath,
                                     const char* xsdPath,
                                     int validateAudioFileReferences,
                                     kwlLogCallback errorCallback)
{
    
    
    kwlEngineDataBinary bin;
    kwlMemset(&bin, 0, sizeof(kwlEngineDataBinary));
    
    kwlResultCode result = kwlEngineDataBinary_loadFromXMLFile(&bin,
                                                           xmlPath,
                                                           xsdPath,
                                                           validateAudioFileReferences,
                                                           errorCallback);
    
    /*clean up*/
    kwlEngineDataBinary_free(&bin);
    
    return result;
    
}




