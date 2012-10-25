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

#include <string.h>

#include "kwl_assert.h"
#include "kwl_memory.h"
#include "kwl_xmlutil.h"

void kwlTraverseNodeTree(xmlNode* root,
                         const char* branchNodeName,
                         const char* leafNodeName,
                         kwlNodeTraversalCallback callback,
                         void* userData,
                         int* errorOccurred,
                         kwlLogCallback errorLogCallback)
{
    for (xmlNode* curr = root->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)branchNodeName))
        {
            kwlTraverseNodeTree(curr, branchNodeName, leafNodeName, callback, userData, errorOccurred, errorLogCallback);
        }
        if (xmlStrEqual(curr->name, (xmlChar*)leafNodeName))
        {
            callback(curr, userData, errorOccurred, errorLogCallback);
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
    char* trueVals[3] = {"true", "True", "TRUE"};
    
    const xmlChar* val = kwlGetAttributeValue(node, attribute);
    
    for (int i = 0; i < 3; i++)
    {
        if (xmlStrEqual(val, (xmlChar*)trueVals[i]))
        {
            return 1;
        }
    }
    
    return 0;
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


kwlResultCode kwlLoadAndValidateProjectDataDoc(const char* xmlPath,
                                               const char* schemaPath,
                                               xmlDocPtr* docPtr,
                                               kwlLogCallback lc)
{
    kwlLogCallback logCallback = lc == NULL ? kwlSilentLogCallback : lc;
    
    *docPtr = NULL;
    xmlDocPtr doc = xmlReadFile(xmlPath, NULL, 0);
    if (doc == NULL)
    {
        logCallback("Failed to parse %s\n", xmlPath);
        return KWL_FAILED_TO_PARSE_PROJECT_XML;
    }
    
    xmlDocPtr schema_doc = xmlReadFile(schemaPath, NULL, XML_PARSE_NONET);
    if (schema_doc == NULL)
    {
        logCallback("the schema %s cannot be loaded or is not well-formed\n", schemaPath);
        return KWL_FAILED_TO_PARSE_SCHEMA;
    }
    
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL)
    {
        /* unable to create a parser context for the schema */
        logCallback("unable to create a parser context for the schema %s\n", schemaPath);
        xmlFreeDoc(schema_doc);
        return KWL_FAILED_TO_CREATE_SCHEMA_PARSER_CONTEXT;
    }
    
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL)
    {
        logCallback("the schema %s is not valid", schemaPath);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return KWL_INVALID_SCHEMA;
    }
    
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL)
    {
        /* unable to create a validation context for the schema */
        logCallback("unable to create a validation context for the schema %s\n", schemaPath);
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return KWL_FAILED_TO_CREATE_SCHEMA_VALIDATION_CONTEXT;
    }
    
    /*tell libxml2 to add omitted optional attributes with default values*/
    xmlSchemaSetValidOptions(valid_ctxt, XML_SCHEMA_VAL_VC_I_CREATE);
    
    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    
    if (is_valid)
    {
        *docPtr = doc;
        return KWL_SUCCESS;
    }
    else
    {
        xmlFreeDoc(doc);
        return KWL_XML_VALIDATION_FAILED;
    }
}

xmlNode* kwlGetChild(xmlNode* node, const char* id)
{
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)id))
        {
            return curr;
        }
    }
    
    return NULL;
}

xmlNode* kwlGetChildWithIdAttributeValue(xmlNode* node, const char* id)
{
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (curr->type != XML_ELEMENT_NODE)
        {
            continue;
        }
        
        xmlChar* currId = kwlGetAttributeValue(curr, KWL_XML_ATTR_ID);
        if (currId == NULL)
        {
            continue;
        }
        //printf("  looking for child with id %s, testing %s\n", id, currId);
        if (xmlStrEqual(currId, (xmlChar*)id))
        {
            return curr;
        }
    }
    
    return NULL;
}

xmlNode* kwlResolveNodePath(xmlNode* root, const char* path)
{
    //printf("resolving node path '%s'\n", path);
    
    const size_t pathLen = strlen(path);
    if (pathLen == 0)
    {
        return NULL;
    }
    
    if (path[0] == '/')
    {
        return NULL;
    }
    
    size_t currStart = 0;
    size_t currEnd = 0;
    
    xmlNode* currNode = root;
    
    while (currEnd < pathLen)
    {
        /*get current path element */
        for (currEnd = currStart; currEnd < pathLen; currEnd++)
        {
            if (path[currEnd] == '/')
            {
                break;
            }
        }
        
        char* currPathElement = KWL_MALLOC(currEnd - currStart + 1, "path element");
        kwlMemcpy(currPathElement, &path[currStart], currEnd - currStart);
        currPathElement[currEnd - currStart] = '\0';
        
        //printf("    current path element '%s'\n", currPathElement);
        
        currNode = kwlGetChildWithIdAttributeValue(currNode, currPathElement);
        KWL_FREE(currPathElement);
        
        if (currNode == NULL)
        {
            return NULL;
        }
        
        currStart = currEnd + 1; //+1 to skip '/'
    }
    
    return currNode;
}

xmlNode* kwlResolveAudioDataReference(xmlNode* someNode, const char* wbPath, const char* audioDataPath)
{
    /*get the project node (a bit hacky)*/
    xmlNode* rootNode = someNode;
    while (xmlStrcmp(rootNode->name, (xmlChar*)KWL_XML_KOWALSKI_PROJECT_NODE) != 0)
    {
        rootNode = rootNode->parent;
    }
    
    xmlNode* waveBankRootNode = kwlGetChild(rootNode, KWL_XML_WAVE_BANK_GROUP_NODE);
    xmlNode* waveBankNode = kwlResolveNodePath(waveBankRootNode, wbPath);
    
    KWL_ASSERT(xmlStrEqual(waveBankNode->name, (xmlChar*)KWL_XML_WAVE_BANK_NODE));
    
    if (waveBankNode == NULL)
    {
        return NULL;
    }
    
    for (xmlNode* curr = waveBankNode->children; curr != NULL; curr = curr->next)
    {
        if (curr->type != XML_ELEMENT_NODE)
        {
            continue;
        }
        
        xmlChar* currPath = kwlGetAttributeValue(curr, KWL_XML_ATTR_REL_PATH);
        KWL_ASSERT(currPath != NULL);
        //printf("currPath %s, audioDataPath %s\n", currPath, audioDataPath);
        if (xmlStrcmp(currPath, (xmlChar*)audioDataPath) == 0)
        {
            return curr;
        }
    }
    
    return NULL;
}
