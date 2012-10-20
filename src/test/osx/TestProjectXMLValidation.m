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

#import "TestProjectXMLValidation.h"

#import "kwl_logging.h"

@implementation TestProjectXMLValidation

- (void)setUp
{
    [super setUp];
    
    // Set-up code here.
}

- (void)tearDown
{
    // Tear-down code here.
    
    [super tearDown];
}

-(void)testBrokenSchemaRules
{
    [self requireXMLValidationResult:@"schema_error_duplicate_event_root_group.xml"
                                    :KWL_XML_VALIDATION_FAILED];
    [self requireXMLValidationResult:@"schema_error_forbidden_element_under_root.xml"
                                    :KWL_XML_VALIDATION_FAILED];
    [self requireXMLValidationResult:@"schema_error_wrong_root_name.xml"
                                    :KWL_XML_VALIDATION_FAILED];
}

-(void)testDuplicateEventGroupIDs
{
    [self requireXMLValidationResult:@"duplicate_event_group_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"duplicate_event_group_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateEventIDs
{
    [self requireXMLValidationResult:@"duplicate_event_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateMixBusIDs
{
    [self requireXMLValidationResult:@"duplicate_mix_bus_ids.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateSoundGroupIDs
{
    [self requireXMLValidationResult:@"duplicate_sound_group_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"duplicate_sound_group_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateSoundIDs
{
    [self requireXMLValidationResult:@"duplicate_sound_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"duplicate_sound_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateWaveBankIDs
{
    [self requireXMLValidationResult:@"duplicate_wave_bank_ids.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidAudioDataReference
{
    [self requireXMLValidationResult:@"invalid_audio_data_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidEventMixBusReference
{
    [self requireXMLValidationResult:@"invalid_event_mix_bus_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidMixPresetMixBusReference
{
    [self requireXMLValidationResult:@"invalid_mix_preset_mix_bus_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testExtraMixBusParameterSet
{
    [self requireXMLValidationResult:@"extra_mix_bus_parameter_set.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}


-(void)testMissingMixBusParameterSet
{
    [self requireXMLValidationResult:@"missing_mix_bus_parameter_set.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateMixPresetMixBusReference
{
    [self requireXMLValidationResult:@"duplicate_mix_preset_bus_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidSoundReference
{
    [self requireXMLValidationResult:@"invalid_sound_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testValidProjectXML
{
    [self requireXMLValidationResult:@"minimal_valid_project.xml"
                                    :KWL_SUCCESS];
    [self requireXMLValidationResult:@"minimal_valid_project_reordered_root_groups.xml"
                                    :KWL_SUCCESS];
}

-(void)testMissingDefaultPreset
{
    [self requireXMLValidationResult:@"missing_default_mix_preset.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testMultipleDefaultPresets
{
    [self requireXMLValidationResult:@"multiple_default_mix_presets.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testXMLSyntaxError
{
    [self requireXMLValidationResult:@"xml_syntax_error.xml"
                                    :KWL_FAILED_TO_PARSE_PROJECT_XML];
}

-(void)requireXMLValidationResult:(NSString*) xmlPath
                                 :(kwlResultCode)expectedResult
{
    kwlResultCode result = kwlValidateProjectData([self getResourcePath:xmlPath],
                                                  [self getResourcePath:@"kowalski.xsd"],
                                                  0,
                                                  kwlDefaultLogCallback);
    STAssertEquals(result,
                   expectedResult,
                   [NSString stringWithFormat:@"%@ should %@ generate a validation error", xmlPath, expectedResult == KWL_SUCCESS ? @"not" : @""]);
}

-(const char*)getResourcePath:(NSString*)fileName
{
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *path = [bundle pathForResource:fileName
                                      ofType:nil];
    
    return [path UTF8String];
}

@end
