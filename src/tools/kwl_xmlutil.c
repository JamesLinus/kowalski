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

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "kwl_memory.h"
#include "kwl_xmlutil.h"

void kwlTraverseNodeTree(xmlNode* root,
                         const char* branchNodeName,
                         const char* leafNodeName,
                         kwlNodeTraversalCallback callback,
                         void* userData)
{
    for (xmlNode* curr = root->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)branchNodeName))
        {
            kwlTraverseNodeTree(curr, branchNodeName, leafNodeName, callback, userData);
        }
        if (xmlStrEqual(curr->name, (xmlChar*)leafNodeName))
        {
            callback(curr, userData);
        }
    }
}


xmlChar* kwlGetAttributeValue(xmlNode* node, const char* name)
{
    xmlAttr* attr = node->properties;
    while (attr)
    {
        if (xmlStrEqual(attr->name, (xmlChar*)name))
        {
            return attr->children->content;
        }
        
        attr = attr->next;
    }
    
    return NULL;
}

char* kwlGetAttributeValueCopy(xmlNode* node, const char* name)
{
    xmlChar* val = NULL;
    
    xmlAttr* attr = node->properties;
    while (attr)
    {
        if (xmlStrEqual(attr->name, (xmlChar*)name))
        {
            val = attr->children->content;
            break;
        }
        
        attr = attr->next;
    }
    
    if (val == NULL)
    {
        return NULL;
    }
    
    int len = xmlStrlen(val) + 1;
    char* ret = KWL_MALLOCANDZERO(len * sizeof(char), "attribute value copy");
    kwlMemcpy(ret, val, len - 1);
    ret[len - 1] = '\0';
    return ret;
}

float kwlGetFloatAttributeValue(xmlNode* node, const char* attribute)
{
    return strtof(kwlGetAttributeValue(node, attribute), NULL);
}

int kwlGetIntAttributeValue(xmlNode* node, const char* attribute)
{
    return strtof(kwlGetAttributeValue(node, attribute), NULL);
}

int kwlGetBoolAttributeValue(xmlNode* node, const char* attribute)
{
    //TODO
    return strtof(kwlGetAttributeValue(node, attribute), NULL);
}


int kwlGetChildCount(xmlNode* node, const char* childName)
{
    int count = 0;
    
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)childName))
        {
            count++;
        }
    }
    
    return count;
}


xmlDocPtr kwlLoadAndValidateProjectDataDoc(const char* xmlPath, const char* schemaPath)
{
    xmlDocPtr doc = xmlReadFile(xmlPath, NULL, 0);
    if (doc == NULL)
    {
        
        fprintf(stderr, "Failed to parse %s\n", xmlPath);
        return NULL;
    }
    
    
    xmlDocPtr schema_doc = xmlReadFile(schemaPath, NULL, XML_PARSE_NONET);
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
