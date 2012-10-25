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

#ifndef KWL_BINARY_BUILDING_H
#define KWL_BINARY_BUILDING_H

#include "kwl_datavalidation.h"
#include "kwl_logging.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     * Builds an engine data binary file from a given project data XML file.
     * Unless \c forceRebuild is set, this function performs dependency
     * checking and will only rebuild an existing engine data binary
     * file if the XML file has more recent changes.
     * @param xmlPath The path to the project data XML file.
     * @param xsdPath The path to the project data XSD schema.
     * @param targetFile The path of the resulting engine data binary file.
     * @param forceRebuild If non-zero, dependecy checking is bypassed.
     * @param errorLogCallback Any errors are printed using this callback.
     * @return A result code.
     */
    kwlResultCode kwlBuildEngineData(const char* xmlPath,
                                     const char* xsdPath,
                                     const char* targetFile,
                                     int forceRebuild,
                                     kwlLogCallback errorLogCallback);
    
    /**
     * Creates one wave bank binary file for all wave banks in a given project
     * XML data file. Unless \c forceRebuild is set, this function performs
     * dependency checking and will only rebuild an existing wave bank binary
     * file if the XML file or any audio file referenced by the wave bank
     * has more recent changes.
     * @param xmlPath The path to the project data XML file.
     * @param xsdPath The path to the project data XSD schema.
     * @param targetDir The target directory. A wave bank with node path \'group/bank' will
     * be written to the file \bank.kwb in the subdirectory \group of \targetDir.
     * @param forceRebuild If non-zero, dependecy checking is bypassed.
     * @param errorLogCallback Any errors are printed using this callback.
     * @return A result code.
     */
    kwlResultCode kwlBuildWaveBanks(const char* xmlPath,
                                    const char* xsdPath,
                                    const char* targetDir,
                                    int forceRebuild,
                                    kwlLogCallback errorLogCallback);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_BINARY_BUILDING_H*/
