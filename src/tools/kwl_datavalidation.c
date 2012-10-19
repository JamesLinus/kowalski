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
#include "kwl_enginedatabinary.h"

static kwlDataValidationResult validateEngineDataBinaryStructure(kwlEngineDataBinary* bin, kwlLogCallback errorCallback)
{
    return 0;
}

kwlDataValidationResult kwlValidateProjectData(const char* xmlPath, const char* xsdPath, kwlLogCallback errorCallback)
{
    

    kwlEngineDataBinary bin;
    kwlMemset(&bin, 0, sizeof(kwlEngineDataBinary));
    
    /*xml syntax and schema validation*/
    {
        kwlDataValidationResult result = kwlEngineDataBinary_loadFromXML(&bin,
                                                                         xmlPath,
                                                                         xsdPath,
                                                                         errorCallback);
        if (result != KWL_DATA_IS_VALID)
        {
            return result;
        }
    }
    
    /*check structure*/
    {
        kwlDataValidationResult result = validateEngineDataBinaryStructure(&bin, errorCallback);
        
        if (result != KWL_DATA_IS_VALID)
        {
            kwlEngineDataBinary_free(&bin);
            return result;
            
        }
    }
    
    /*clean up*/
    kwlEngineDataBinary_free(&bin);
    
    return KWL_DATA_IS_VALID;
    
}




