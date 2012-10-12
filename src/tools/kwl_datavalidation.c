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

#include "kwl_datavalidation.h"

/**
 * example1Func:
 * @filename: a filename or an URL
 *
 * Parse the resource and free the resulting tree
 */
static void example1Func(const char *filename)
{
    xmlDocPtr doc; /* the resulting document tree */
    
    doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL)
    {
        
        fprintf(stderr, "Failed to parse %s\n", filename);
        return;
    }
    xmlFreeDoc(doc);
}

void kwlValidateProjectData(const char* xmlPath)
{
    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION
    
    example1Func(xmlPath);
    
    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    
}
