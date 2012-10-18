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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#include "kwl_assert.h"
#include "kwl_binarybuilding.h"
#include "kwl_projectdatabinaryrepresentation.h"
#include "kwl_memory.h"
#include "kwl_toolsutil.h"

typedef void (*kwlNodeTraversalCallback)(xmlNode* currentNode, void* userData);

static int kwlGetChildCount(xmlNode* node, const char* childName)
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

static char* kwlGetAttributeValue(xmlNode* node, const char* name)
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



/**
 * returns the path up to the top group
 */
static const char* kwlGetNodePath(xmlNode* currentNode)
{
    xmlNode* n = currentNode;
    
}

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


static int kwlGetNumChildrenWithName(xmlNode* root, const char* branchNodeName, const char* name)
{
    for (xmlNode* curr = root->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)branchNodeName))
        {
            
        }
        else if (xmlStrEqual(curr->name, (xmlChar*)name))
        {
            
        }
    }
}

/**
 * Gather mix buses excluding sub buses
 */
static void kwlGatherMixBusesCallback(xmlNode* currentNode, void* b)
{
    kwlProjectDataBinaryRepresentation* bin = (kwlProjectDataBinaryRepresentation*)b;
    bin->mixBusChunk.numMixBuses += 1;
    bin->mixBusChunk.mixBuses = realloc(bin->mixBusChunk.mixBuses,
                                        sizeof(kwlMixBusChunk) * bin->mixBusChunk.numMixBuses);
    kwlMixBusChunk* c = &bin->mixBusChunk.mixBuses[bin->mixBusChunk.numMixBuses - 1];
    c->id = kwlGetAttributeValue(currentNode, "id");
    KWL_ASSERT(c->id != NULL);
    c->numSubBuses = kwlGetChildCount(currentNode, KWL_XML_MIX_BUS_NAME);
    if (c->numSubBuses > 0)
    {
        c->subBusIndices = KWL_MALLOCANDZERO(c->numSubBuses * sizeof(int), "xml 2 bin sub buses");
    }
}

/**
 * gather sub buses of already gathered mix buses
 */
static void kwlGatherSubBusesCallback(xmlNode* node, void* b)
{
    kwlProjectDataBinaryRepresentation* bin = (kwlProjectDataBinaryRepresentation*)b;
    
    //find the mix bus to attach the children to
    kwlMixBusChunk* mb = NULL;
    for (int i = 0; i < bin->mixBusChunk.numMixBuses; i++)
    {
        kwlMixBusChunk* mbi = &bin->mixBusChunk.mixBuses[i];
        if (strcmp(mbi->id, kwlGetAttributeValue(node, "id")) == 0)
        {
            mb = mbi;
            break;
        }
    }
    KWL_ASSERT(mb != NULL);
    
    int childIdx = 0;
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (!xmlStrEqual(curr->name, (xmlChar*)KWL_XML_MIX_BUS_NAME))
        {
            continue;
        }
        
        //find the index of this sub bus
        const char* id = kwlGetAttributeValue(curr, "id");
        KWL_ASSERT(id != NULL);
        int idx = -1;
        for (int i = 0; i < bin->mixBusChunk.numMixBuses; i++)
        {
            if (strcmp(bin->mixBusChunk.mixBuses[i].id, id) == 0)
            {
                idx = i;
                break;
            }
        }
        
        mb->subBusIndices[childIdx] = idx;
        
        
        KWL_ASSERT(idx >= 0);
        childIdx++;
    }
    
}

static void kwlMixBusRootXMLToBin(xmlNode* projectRoot, kwlProjectDataBinaryRepresentation* bin)
{
    kwlTraverseNodeTree(projectRoot,
                        KWL_XML_MIX_BUS_NAME,
                        KWL_XML_MIX_BUS_NAME,
                        kwlGatherMixBusesCallback,
                        bin);
    
    kwlTraverseNodeTree(projectRoot,
                        KWL_XML_MIX_BUS_NAME,
                        KWL_XML_MIX_BUS_NAME,
                        kwlGatherSubBusesCallback,
                        bin);
    
    kwlProjectDataBinaryRepresentation_dump(bin, kwlDefaultLogCallback);
}

static void kwlMixPresetRootXMLToBin(xmlNode* root, kwlProjectDataBinaryRepresentation* bin)
{
    
}

static void kwlWaveBankRootXMLToBin(xmlNode* root, kwlProjectDataBinaryRepresentation* bin)
{
    
}

static void kwlSoundRootXMLToBin(xmlNode* root, kwlProjectDataBinaryRepresentation* bin)
{
    
}

static void kwlEventRootXMLToBin(xmlNode* root, kwlProjectDataBinaryRepresentation* bin)
{
    
}

void kwlBuildEngineData(const char* xmlPath, const char* targetFile)
{
    xmlDoc *doc = NULL;
    /*parse the file and get the node tree */
    doc = xmlReadFile(xmlPath, NULL, 0);
    
    if (doc == NULL) {
        printf("error: could not parse file %s\n", xmlPath);
        return;
    }
    
    /*Get the root element node */
    xmlNode* projectRootNode = xmlDocGetRootElement(doc);
    
    /*Process mix bus, mix presets, wave banks, sounds and events*/
    xmlNode* mixBusRootNode = NULL;
    xmlNode* mixPresetRootNode = NULL;
    xmlNode* waveBankRootNode = NULL;
    xmlNode* soundRootNode = NULL;
    xmlNode* eventRootNode = NULL;
    
    for (xmlNode* currNode = projectRootNode->children; currNode; currNode = currNode->next)
    {
        if (xmlStrcmp(currNode->name, (xmlChar*)KWL_XML_MIX_BUS_NAME) == 0)
        {
            mixBusRootNode = currNode;
        }
        else if (xmlStrcmp(currNode->name, (xmlChar*)KWL_XML_MIX_PRESET_GROUP_NAME) == 0)
        {
            mixPresetRootNode = currNode;
        }
        else if (xmlStrcmp(currNode->name, (xmlChar*)KWL_XML_WAVE_BANK_GROUP_NAME) == 0)
        {
            waveBankRootNode = currNode;
        }
        else if (xmlStrcmp(currNode->name, (xmlChar*)KWL_XML_SOUND_GROUP_NAME) == 0)
        {
            soundRootNode = currNode;
        }
        else if (xmlStrcmp(currNode->name, (xmlChar*)KWL_XML_EVENT_GROUP_NAME) == 0)
        {
            eventRootNode = currNode;
        }
    }
    
    KWL_ASSERT(projectRootNode != NULL);
    KWL_ASSERT(mixBusRootNode != NULL);
    KWL_ASSERT(mixPresetRootNode != NULL);
    KWL_ASSERT(waveBankRootNode != NULL);
    KWL_ASSERT(soundRootNode != NULL);
    KWL_ASSERT(eventRootNode != NULL);
    
    kwlProjectDataBinaryRepresentation bin;
    memset(&bin, 0, sizeof(kwlProjectDataBinaryRepresentation));
    
    kwlMixBusRootXMLToBin(projectRootNode, &bin);
    kwlMixPresetRootXMLToBin(mixPresetRootNode, &bin);
    kwlWaveBankRootXMLToBin(waveBankRootNode, &bin);
    kwlSoundRootXMLToBin(soundRootNode, &bin);
    kwlEventRootXMLToBin(eventRootNode, &bin);
    
    /*free the document */
    xmlFreeDoc(doc);
    
    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}


