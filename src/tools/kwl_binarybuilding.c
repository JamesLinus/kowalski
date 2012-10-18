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

#define KWL_TEMP_STRING_LENGTH 1024

typedef void (*kwlNodeTraversalCallback)(xmlNode* currentNode, void* userData);

static char* kwlGetAttributeValueCopy(xmlNode* node, const char* name)
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


static xmlChar* kwlGetAttributeValue(xmlNode* node, const char* name)
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

static float kwlGetFloatAttributeValue(xmlNode* node, const char* attribute)
{
    return strtof(kwlGetAttributeValue(node, attribute), NULL);
}

static int kwlGetIntAttributeValue(xmlNode* node, const char* attribute)
{
    return strtof(kwlGetAttributeValue(node, attribute), NULL);
}

static int kwlGetBoolAttributeValue(xmlNode* node, const char* attribute)
{
    //TODO
    return strtof(kwlGetAttributeValue(node, attribute), NULL);
}


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





/**
 * returns the path up to the top group
 */
static const xmlChar* kwlGetNodePath(xmlNode* currentNode)
{
    xmlNode* path[KWL_TEMP_STRING_LENGTH];
    memset(path, 0, KWL_TEMP_STRING_LENGTH * sizeof(xmlNode*));

    xmlNode* n = currentNode;
    int idx = 0;
    while (!xmlStrEqual(n->parent->name, (xmlChar*)KWL_XML_PROJECT_NODE_NAME))
    {
        path[idx] = n;
        idx++;
        KWL_ASSERT(idx < KWL_TEMP_STRING_LENGTH);
        n = n->parent;
    }
    
    int numChars = 0;
    xmlChar tempStr[KWL_TEMP_STRING_LENGTH];
    kwlMemset(tempStr, 0, sizeof(xmlChar) * KWL_TEMP_STRING_LENGTH);
    for (int i = idx - 1; i >= 0; i--)
    {
        //id gets freed along with the xml structure
        xmlChar* id = kwlGetAttributeValue(path[i], "id");
        tempStr[numChars] = '/';
        memcpy(&tempStr[numChars + 1], id, xmlStrlen(id));
        numChars += xmlStrlen(id) + 1;
        KWL_ASSERT(numChars < KWL_TEMP_STRING_LENGTH);
    }
    tempStr[numChars] = '\0';
    
    xmlChar* pathStr = KWL_MALLOCANDZERO(numChars, "path string");
    memcpy(pathStr, &tempStr[1], numChars); //remove leading slash
    pathStr[numChars - 1] = '\0';
    return pathStr;
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
    c->id = kwlGetAttributeValueCopy(currentNode, "id");
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
        const xmlChar* id = kwlGetAttributeValue(curr, "id");
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
    bin->mixBusChunk.chunkId = KWL_MIX_BUSES_CHUNK_ID;
    
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
}

static int kwlGetMixBusIndex(kwlProjectDataBinaryRepresentation* bin, const char* id)
{
    KWL_ASSERT(bin->mixBusChunk.numMixBuses > 0);
    
    for (int i = 0; i < bin->mixBusChunk.numMixBuses; i++)
    {
        kwlMixBusChunk* mbi = &bin->mixBusChunk.mixBuses[i];
        if (strcmp(id, mbi->id) == 0)
        {
            return i;
        }
    }
    
    KWL_ASSERT(0);
    return -1;
}

/**
 * Gather mix presets
 */
static void kwlGatherMixPresetsCallback(xmlNode* node, void* b)
{
    kwlProjectDataBinaryRepresentation* bin = (kwlProjectDataBinaryRepresentation*)b;
    const int numMixBuses = bin->mixBusChunk.numMixBuses;
    KWL_ASSERT(numMixBuses > 0);
    
    bin->mixPresetChunk.numMixPresets += 1;
    bin->mixPresetChunk.mixPresets = realloc(bin->mixPresetChunk.mixPresets,
                                        sizeof(kwlMixPresetChunk) * bin->mixPresetChunk.numMixPresets);
    kwlMixPresetChunk* c = &bin->mixPresetChunk.mixPresets[bin->mixPresetChunk.numMixPresets - 1];
    c->id = kwlGetNodePath(node);
    c->isDefault = kwlGetIntAttributeValue(node, KWL_XML_ATTR_MIX_PRESET_IS_DEFAULT);
    c->mixBusIndices = KWL_MALLOCANDZERO(numMixBuses * sizeof(int), "xml 2 bin mix preset bus indices");
    c->gainLeft = KWL_MALLOCANDZERO(numMixBuses * sizeof(int), "xml 2 bin mix preset bus gain l");
    c->gainRight = KWL_MALLOCANDZERO(numMixBuses * sizeof(int), "xml 2 bin mix preset bus gain r");
    c->pitch = KWL_MALLOCANDZERO(numMixBuses * sizeof(int), "xml 2 bin mix preset bus pitch");
    
    int paramSetIdx = 0;
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)KWL_XML_PARAM_SET_NAME))
        {
            char* busId = kwlGetAttributeValue(curr, KWL_XML_ATTR_PARAM_SET_BUS);
            const int busIdx = kwlGetMixBusIndex(bin, busId);
            float gainLeft = kwlGetFloatAttributeValue(curr, KWL_XML_ATTR_PARAM_SET_GAIN_L);
            float gainRight = kwlGetFloatAttributeValue(curr, KWL_XML_ATTR_PARAM_SET_GAIN_R);
            float pitch = kwlGetFloatAttributeValue(curr, KWL_XML_ATTR_PARAM_SET_PITCH);
            
            KWL_ASSERT(paramSetIdx < numMixBuses);
            c->gainLeft[paramSetIdx] = gainLeft;
            c->gainLeft[paramSetIdx] = gainRight;
            c->pitch[paramSetIdx] = pitch;
            c->mixBusIndices[paramSetIdx] = busIdx;
            paramSetIdx++;
        }
    }
    
    KWL_ASSERT(paramSetIdx == numMixBuses);
}

static void kwlMixPresetRootXMLToBin(xmlNode* projectRoot, kwlProjectDataBinaryRepresentation* bin)
{
    bin->mixPresetChunk.chunkId = KWL_MIX_PRESETS_CHUNK_ID;
    
    kwlTraverseNodeTree(projectRoot,
                        KWL_XML_MIX_PRESET_GROUP_NAME,
                        KWL_XML_MIX_PRESET_NAME,
                        kwlGatherMixPresetsCallback,
                        bin);
}

static void kwlGatherWaveBanksCallback(xmlNode* node, void* b)
{
    kwlProjectDataBinaryRepresentation* bin = (kwlProjectDataBinaryRepresentation*)b;
    bin->waveBankChunk.numWaveBanks += 1;
    bin->waveBankChunk.waveBanks = realloc(bin->waveBankChunk.waveBanks,
                                             sizeof(kwlWaveBankChunk) * bin->waveBankChunk.numWaveBanks);
    kwlWaveBankChunk* c = &bin->waveBankChunk.waveBanks[bin->waveBankChunk.numWaveBanks - 1];
    const xmlChar* path = kwlGetNodePath(node);
    c->id = path;
    c->numAudioDataEntries = kwlGetChildCount(node, KWL_XML_AUDIO_DATA_ITEM_NAME);
    KWL_ASSERT(c->numAudioDataEntries > 0);
    c->audioDataEntries = KWL_MALLOCANDZERO(c->numAudioDataEntries * sizeof(char*), "xml 2 bin wb entries");
    
    int idx = 0;
    //printf("reading wavebank %s\n", c->id);
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)KWL_XML_AUDIO_DATA_ITEM_NAME))
        {
            
            char* itemPath = kwlGetAttributeValueCopy(curr, KWL_XML_ATTR_REL_PATH);
            c->audioDataEntries[idx] = itemPath;
            //printf("    %s\n", itemPath);
            idx++;
        }
    }
    
    KWL_ASSERT(path != NULL);
}

static void kwlWaveBankRootXMLToBin(xmlNode* projectRoot, kwlProjectDataBinaryRepresentation* bin)
{
    bin->waveBankChunk.chunkId = KWL_WAVE_BANKS_CHUNK_ID;
    
    /*collect wave banks and their ids and audio data entry counts*/
    kwlTraverseNodeTree(projectRoot,
                        KWL_XML_WAVE_BANK_GROUP_NAME,
                        KWL_XML_WAVE_BANK_NAME,
                        kwlGatherWaveBanksCallback,
                        bin);
    /*store the total number of audio data items*/
    int totalNumItems = 0;
    for (int i = 0; i < bin->waveBankChunk.numWaveBanks; i++)
    {
        kwlWaveBankChunk* wbi = &bin->waveBankChunk.waveBanks[i];
        totalNumItems += wbi->numAudioDataEntries;
    }
    bin->waveBankChunk.numAudioDataItemsTotal = totalNumItems;
}

static int kwlGetWaveBankIndex(kwlProjectDataBinaryRepresentation* bin, const char* id)
{
    KWL_ASSERT(bin->waveBankChunk.numWaveBanks > 0);
    
    for (int i = 0; i < bin->waveBankChunk.numWaveBanks; i++)
    {
        kwlWaveBankChunk* wbi = &bin->waveBankChunk.waveBanks[i];
        if (strcmp(id, wbi->id) == 0)
        {
            return i;
        }
    }
    
    KWL_ASSERT(0);
    return -1;
}


static int kwlGetAudioDataIndex(kwlWaveBankChunk* wb, const char* id)
{
    KWL_ASSERT(wb->numAudioDataEntries > 0);
    
    for (int i = 0; i < wb->numAudioDataEntries; i++)
    {
        
        if (strcmp(id, wb->audioDataEntries[i]) == 0)
        {
            return i;
        }
    }
    
    KWL_ASSERT(0);
    return -1;
}




static void kwlGatherSoundsCallback(xmlNode* node, void* b)
{
    kwlProjectDataBinaryRepresentation* bin = (kwlProjectDataBinaryRepresentation*)b;
    bin->soundChunk.numSoundDefinitions += 1;
    bin->soundChunk.soundDefinitions = realloc(bin->soundChunk.soundDefinitions,
                                               sizeof(kwlSoundChunk) * bin->soundChunk.numSoundDefinitions);
    kwlSoundChunk* c = &bin->soundChunk.soundDefinitions[bin->soundChunk.numSoundDefinitions - 1];
    /*
    c->gain = kwlGetFloatAttributeValue(node, KWL_XML_ATTR_SOUND_GAIN);
    c->gainVariation = kwlGetFloatAttributeValue(node, KWL_XML_ATTR_SOUND_GAIN_VAR);
    c->pitch = kwlGetFloatAttributeValue(node, KWL_XML_ATTR_PITCH);
    c->pitchVariation = kwlGetFloatAttributeValue(node, KWL_XML_ATTR_PITCH_VAR);
    c->deferStop = kwlGetIntAttributeValue(node, KWL_XML_ATTR_DEFER_STOP);
    c->playbackCount = kwlGetIntAttributeValue(node, KWL_XML_ATTR_PLAYBACK_COUNT);
    c->playbackMode = kwlGetIntAttributeValue(node, KWL_XML_ATTR_PLAYBACK_MODE);*/
    c->numWaveReferences = kwlGetChildCount(node, KWL_XML_AUDIO_DATA_REFERENCE_NAME);
    KWL_ASSERT(c->numWaveReferences > 0);
    
    if (c->numWaveReferences > 0)
    {
        c->waveBankIndices = KWL_MALLOCANDZERO(c->numWaveReferences * sizeof(int), "xml 2 bin sound wb indices");
        c->audioDataIndices = KWL_MALLOCANDZERO(c->numWaveReferences * sizeof(int), "xml 2 bin sound ad indices");
    }
    
    int refIdx = 0;
    for (xmlNode* curr = node->children; curr != NULL; curr = curr->next)
    {
        if (xmlStrEqual(curr->name, (xmlChar*)KWL_XML_AUDIO_DATA_REFERENCE_NAME))
        {
            xmlChar* filePath = kwlGetAttributeValue(curr, KWL_XML_ATTR_AUDIO_DATA_REFERENCE_PATH);
            xmlChar* waveBankId = kwlGetAttributeValue(curr, KWL_XML_ATTR_AUDIO_DATA_REFERENCE_WAVEBANK);
            
            const int wbIdx = kwlGetWaveBankIndex(bin, waveBankId);
            kwlWaveBankChunk* wb = &bin->waveBankChunk.waveBanks[wbIdx];
            const int itemIdx = kwlGetAudioDataIndex(wb, filePath);
            
            c->waveBankIndices[refIdx] = wbIdx;
            c->audioDataIndices[refIdx] = itemIdx;
            
            KWL_ASSERT(refIdx < bin->waveBankChunk.numAudioDataItemsTotal);
            refIdx++;
        }
    }
    
    KWL_ASSERT(refIdx == c->numWaveReferences);
    
    //const xmlChar* path = kwlGetNodePath(currentNode);
    //printf("sound path %s\n", path);
    //KWL_ASSERT(path != NULL);
}

static void kwlSoundRootXMLToBin(xmlNode* projectRoot, kwlProjectDataBinaryRepresentation* bin)
{
    bin->soundChunk.chunkId = KWL_SOUNDS_CHUNK_ID;
    
    kwlTraverseNodeTree(projectRoot,
                        KWL_XML_SOUND_GROUP_NAME,
                        KWL_XML_SOUND_NAME,
                        kwlGatherSoundsCallback,
                        bin);
}


static void kwlGatherEventsCallback(xmlNode* currentNode, void* b)
{
    /*
     kwlProjectDataBinaryRepresentation* bin = (kwlProjectDataBinaryRepresentation*)b;
     bin->waveBankChunk.numWaveBanks += 1;
     bin->mixPresetChunk.mixPresets = realloc(bin->mixPresetChunk.mixPresets,
     sizeof(kwlMixPresetChunk) * bin->mixPresetChunk.numMixPresets);
     kwlMixPresetChunk* c = &bin->mixPresetChunk.mixPresets[bin->mixPresetChunk.numMixPresets - 1];*/
    const xmlChar* path = kwlGetNodePath(currentNode);
    //printf("event path %s\n", path);
    KWL_ASSERT(path != NULL);
}

static void kwlEventRootXMLToBin(xmlNode* projectRoot, kwlProjectDataBinaryRepresentation* bin)
{
    bin->eventChunk.chunkId = KWL_EVENTS_CHUNK_ID;
    
    kwlTraverseNodeTree(projectRoot,
                        KWL_XML_EVENT_GROUP_NAME,
                        KWL_XML_EVENT_NAME,
                        kwlGatherEventsCallback,
                        bin);
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
    
    kwlProjectDataBinaryRepresentation_dump(&bin, kwlDefaultLogCallback);
    
    /*free the document */
    xmlFreeDoc(doc);
    
    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}


