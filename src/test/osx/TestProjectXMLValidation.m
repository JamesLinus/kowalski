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

/***************************************************************************
 * EVENT STRUCTURE TESTS
 ***************************************************************************/


-(void)testDuplicateEventGroupIDs
{
    [self requireXMLValidationResult:@"event_group_duplicate_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"event_group_duplicate_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateEventIDs
{
    [self requireXMLValidationResult:@"event_duplicate_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"event_duplicate_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidEventMixBusReference
{
    [self requireXMLValidationResult:@"event_invalid_mix_bus_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidSoundReference
{
    [self requireXMLValidationResult:@"event_invalid_sound_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

/***************************************************************************
 * MIX BUS STRUCTURE TESTS
 ***************************************************************************/

-(void)testDuplicateMixBusIDs
{
    [self requireXMLValidationResult:@"mix_bus_duplicate_ids.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

/***************************************************************************
 * SOUND STRUCTURE TESTS
 ***************************************************************************/

-(void)testDuplicateSoundGroupIDs
{
    [self requireXMLValidationResult:@"sound_group_duplicate_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"sound_group_duplicate_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateSoundIDs
{
    [self requireXMLValidationResult:@"sound_duplicate_ids_1.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self requireXMLValidationResult:@"sound_duplicate_ids_2.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testSoundInvalidAudioDataReference
{
    [self requireXMLValidationResult:@"sound_invalid_audio_data_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

/***************************************************************************
 * WAVE BANK STRUCTURE TESTS
 ***************************************************************************/

-(void)testDuplicateWaveBankIDs
{
    [self requireXMLValidationResult:@"wave_bank_duplicate_ids.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

/***************************************************************************
 * MIX PRESET STRUCTURE TESTS
 ***************************************************************************/


-(void)testInvalidMixPresetMixBusReference
{
    [self requireXMLValidationResult:@"mix_preset_invalid_mix_bus_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testExtraMixBusParameterSet
{
    [self requireXMLValidationResult:@"mix_preset_extra_parameter_set.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}


-(void)testMissingMixBusParameterSet
{
    [self requireXMLValidationResult:@"mix_preset_missing_parameter_set.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateMixPresetMixBusReference
{
    [self requireXMLValidationResult:@"mix_preset_duplicate_bus_reference.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testMissingDefaultPreset
{
    [self requireXMLValidationResult:@"mix_preset_missing_default.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testMultipleDefaultPresets
{
    [self requireXMLValidationResult:@"mix_preset_multiple_defaults.xml"
                                    :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

/***************************************************************************
 * SCHEMA VALIDATION TESTS
 ***************************************************************************/

-(void)testBrokenSchemaRules
{
    [self requireXMLValidationResult:@"schema_error_duplicate_event_root_group.xml"
                                    :KWL_XML_VALIDATION_FAILED];
    [self requireXMLValidationResult:@"schema_error_forbidden_element_under_root.xml"
                                    :KWL_XML_VALIDATION_FAILED];
    [self requireXMLValidationResult:@"schema_error_wrong_root_name.xml"
                                    :KWL_XML_VALIDATION_FAILED];
}

/***************************************************************************
 * VALID XML DATA TESTS
 ***************************************************************************/

-(void)testValidProjectXML
{
    [self requireXMLValidationResult:@"valid_project_minimal.xml"
                                    :KWL_SUCCESS];
    [self requireXMLValidationResult:@"valid_project_minimal_reordered_root_groups.xml"
                                    :KWL_SUCCESS];
}

/***************************************************************************
 * XML SYNTAX ERROR TESTS
 ***************************************************************************/

-(void)testXMLSyntaxError
{
    [self requireXMLValidationResult:@"xml_syntax_error.xml"
                                    :KWL_FAILED_TO_PARSE_PROJECT_XML];
}

/***************************************************************************
 * HELPER METHODS
 ***************************************************************************/

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
