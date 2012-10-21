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

#ifndef KWL_XML_UTIL_H
#define KWL_XML_UTIL_H

#include <libxml/tree.h>

#include "kwl_datavalidation.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
#define KWL_XML_MIX_BUS_NAME "MixBus"
#define KWL_XML_ATTR_MIX_BUS_ID "id"
    
#define KWL_XML_WAVE_BANK_GROUP_NAME "WaveBankGroup"
#define KWL_XML_AUDIO_DATA_ITEM_NAME "AudioData"
    
#define KWL_XML_AUDIO_DATA_REFERENCE_NAME "AudioDataReference"
#define KWL_XML_ATTR_AUDIO_DATA_REFERENCE_PATH "relativePath"
#define KWL_XML_ATTR_AUDIO_DATA_REFERENCE_WAVEBANK "waveBank"
    
#define KWL_XML_SOUND_REFERENCE_NAME "SoundReference"
#define KWL_XML_ATTR_SOUND_REFERENCE_SOUND "sound"
    
#define KWL_XML_WAVE_BANK_NAME "WaveBank"
#define KWL_XML_MIX_PRESET_GROUP_NAME "MixPresetGroup"
    
#define KWL_XML_MIX_PRESET_NAME "MixPreset"
#define KWL_XML_ATTR_MIX_PRESET_IS_DEFAULT "default"
    
#define KWL_XML_SOUND_GROUP_NAME "SoundGroup"
    
#define KWL_XML_SOUND_NAME "Sound"
    
#define KWL_XML_ATTR_SOUND_GAIN "gain"
#define KWL_XML_ATTR_SOUND_GAIN_VAR "gainVariationPercent"
#define KWL_XML_ATTR_PITCH "pitch"
#define KWL_XML_ATTR_PITCH_VAR "pitchVariationPercent"
#define KWL_XML_ATTR_DEFER_STOP "deferStop"
#define KWL_XML_ATTR_PLAYBACK_COUNT "playbackCount"
#define KWL_XML_ATTR_PLAYBACK_MODE "playbackMode"
    
#define KWL_XML_EVENT_GROUP_NAME "EventGroup"
    
#define KWL_XML_EVENT_NAME "Event"
#define KWL_XML_ATTR_EVENT_OUTER_ANGLE "outerConeAngle"
#define KWL_XML_ATTR_EVENT_INNER_ANGLE "innerConeAngle"
#define KWL_XML_ATTR_EVENT_GAIN "gain"
#define KWL_XML_ATTR_EVENT_PITCH "pitch"
#define KWL_XML_ATTR_EVENT_INSTANCE_COUNT "instanceCount"
#define KWL_XML_ATTR_EVENT_IS_POSITIONAL "positional"
#define KWL_XML_ATTR_EVENT_BUS "bus"
    
#define KWL_XML_PROJECT_NODE_NAME "KowalskiProject"
#define KWL_XML_ATTR_PROJECT_AUDIO_FILE_ROOT "audioFileRootPath"
#define KWL_XML_ATTR_PROJECT_AUDIO_FILE_ROOT_IS_RELATIVE "audioFileRootPathIsRelative"
    
#define KWL_XML_PARAM_SET_NAME "MixBusParameters"
#define KWL_XML_ATTR_PARAM_SET_GAIN_L "leftGain"
#define KWL_XML_ATTR_PARAM_SET_GAIN_R "rightGain"
#define KWL_XML_ATTR_PARAM_SET_PITCH "pitch"
#define KWL_XML_ATTR_PARAM_SET_BUS "mixBus"
    
#define KWL_XML_ATTR_REL_PATH "relativePath"
#define KWL_XML_ATTR_STREAM "streamFromDisk"
    
    /**
     * A callback used when traversing XML node trees.
     * @param currentNode
     * @param userData
     */
    typedef void (*kwlNodeTraversalCallback)(xmlNode* currentNode, void* userData, int* errorOccurred, kwlLogCallback errorLogCallback);
    
    /**
     * Traverses an xml tree starting at a given root, decending at nodes with the name \c branchNodeName
     * and invoking \c callback at nodes with the name \c leafNodeName.
     * @param root
     * @param branchNodeName
     * @param leafNodeName
     * @param callback
     * @param userData
     */
    void kwlTraverseNodeTree(xmlNode* root,
                             const char* branchNodeName,
                             const char* leafNodeName,
                             kwlNodeTraversalCallback callback,
                             void* userData,
                             int* errorOccurred,
                             kwlLogCallback errorLogCallback);
    
    /**
     * Returns the value of an attribute with a given name for a given node. The
     * return value gets released along with the document containing \c node.
     * @param node The node to get the attribute from.
     * @param name The name of the attribute.
     * @return The attribute value, or NULL if the attribute name could not be found.
     */
    xmlChar* kwlGetAttributeValue(xmlNode* node, const char* name);
    
    /**
     * Does the same as kwlGetAttributeValue but returns a copy of the attribute value.
     * @param node The node to get the attribute from.
     * @param name The name of the attribute.
     * @return The attribute value, or NULL if the attribute name could not be found.
     * @see kwlGetAttributeValue
     */
    char* kwlGetAttributeValueCopy(xmlNode* node, const char* name);
    
    /**
     * Returns the float value of a given attribute for a given node.
     * @param node The node to get the attribute from.
     * @param name The name of the attribute.
     * @return The float value of the attribute.
     */
    float kwlGetFloatAttributeValue(xmlNode* node, const char* name);
    
    /**
     * Returns the integer value of a given attribute for a given node.
     * @param node The node to get the attribute from.
     * @param name The name of the attribute.
     * @return The integer value of the attribute.
     */
    int kwlGetIntAttributeValue(xmlNode* node, const char* name);
    
    /**
     * Returns the boolean value of a given attribute for a given node.
     * @param node The node to get the attribute from.
     * @param name The name of the attribute.
     * @return The boolean value of the attribute.
     */
    int kwlGetBoolAttributeValue(xmlNode* node, const char* name);
    
    /**
     * Returns the number of children with the specified name of a given node.
     * @param node The node to count the children of.
     * @param childName Only count children with this name.
     * @return The number of children of \c node.
     */
    int kwlGetChildCount(xmlNode* node, const char* childName);
    
    /**
     * Loads an XML document at a given path and validates it with a given schema.
     * Any omitted optional attributes will be added and assigned their default values.
     * @param xmlPath The path of the XML document.
     * @param schemaPath The path of the XML schema.
     * @param docPtr The loaded document on success, NULL on failure.
     * @param errorLogCallback A callback used to print error messages
     * @return An error code.
     */
    kwlResultCode kwlLoadAndValidateProjectDataDoc(const char* xmlPath,
                                                   const char* schemaPath,
                                                   xmlDocPtr* docPtr,
                                                   kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    xmlNode* kwlGetChild(xmlNode* root, const char* id);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_XML_UTIL_H*/
