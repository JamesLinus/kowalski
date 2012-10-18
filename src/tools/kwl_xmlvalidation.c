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

#include "kwl_xmlvalidation.h"

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

void kwlValidateProjectData(const char* xmlPath, kwlLogCallback errorCallback)
{
    
    example1Func(xmlPath);
    
    //check valid xml structure
    
    //validate against project data schema
    
    //create binary representation and check references etc
    
}

xmlDocPtr kwlLoadAndValidateProjectData(const char* xmlPath)
{
    xmlDocPtr doc = xmlReadFile(xmlPath, NULL, 0);
    if (doc == NULL)
    {
        
        fprintf(stderr, "Failed to parse %s\n", xmlPath);
        return NULL;
    }
    
    
    xmlDocPtr schema_doc = xmlReadFile("/Users/perarne/code/kowalski/src/tools/kowalski1.0.xsd", NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
        /* the schema cannot be loaded or is not well-formed */
        return NULL;
    }
    
    
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
        /* unable to create a parser context for the schema */
        xmlFreeDoc(schema_doc);
        return NULL;
    }
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        /* the schema itself is not valid */
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return NULL;
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        /* unable to create a validation context for the schema */
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return NULL;
    }
    xmlSchemaSetValidOptions(valid_ctxt, XML_SCHEMA_VAL_VC_I_CREATE);
    
    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    /* force the return value to be non-negative on success */
    return is_valid ? doc : NULL;
}


