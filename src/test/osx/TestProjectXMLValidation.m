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

-(void)testDuplicateEventGroupIDs
{
    [self ensureXMLValidationResult:@"duplicate_event_group_ids_1.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self ensureXMLValidationResult:@"duplicate_event_group_ids_2.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateEventIDs
{
    [self ensureXMLValidationResult:@"duplicate_event_ids_1.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateMixBusIDs
{
    [self ensureXMLValidationResult:@"duplicate_mix_bus_ids.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateSoundGroupIDs
{
    [self ensureXMLValidationResult:@"duplicate_sound_group_ids_1.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self ensureXMLValidationResult:@"duplicate_sound_group_ids_2.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateSoundIDs
{
    [self ensureXMLValidationResult:@"duplicate_sound_ids_1.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
    [self ensureXMLValidationResult:@"duplicate_sound_ids_2.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testDuplicateWaveBankIDs
{
    [self ensureXMLValidationResult:@"duplicate_wave_bank_ids.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidAudioDataReference
{
    [self ensureXMLValidationResult:@"invalid_audio_data_reference.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidMixBusReference
{
    [self ensureXMLValidationResult:@"invalid_mix_bus_reference.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testInvalidSoundReference
{
    [self ensureXMLValidationResult:@"invalid_sound_reference.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testValidProjectXML
{
    [self ensureXMLValidationResult:@"minimal_valid_project.xml"
                                   :KWL_DATA_IS_VALID];
}

-(void)testMissingDefaultPreset
{
    [self ensureXMLValidationResult:@"missing_default_mix_preset.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testMultipleDefaultPresets
{
    [self ensureXMLValidationResult:@"multiple_default_mix_presets.xml"
                                   :KWL_PROJECT_XML_STRUCTURE_ERROR];
}

-(void)testXMLSyntaxError
{
    [self ensureXMLValidationResult:@"xml_syntax_error.xml"
                                   :KWL_FAILED_TO_PARSE_PROJECT_XML];
}

-(void)ensureXMLValidationResult:(NSString*) xmlPath
                                :(kwlDataValidationResult)expectedResult
{
    kwlDataValidationResult result = kwlValidateProjectData([self getResourcePath:xmlPath],
                                                            [self getResourcePath:@"kowalski.xsd"],
                                                            kwlDefaultLogCallback);
    STAssertEquals(result,
                   expectedResult,
                   [NSString stringWithFormat:@"%@ should %@ generate a validation error", xmlPath, expectedResult == KWL_DATA_IS_VALID ? @"not" : @""]);
}

-(const char*)getResourcePath:(NSString*)fileName
{
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *path = [bundle pathForResource:fileName
                                      ofType:nil];
    
    return [path UTF8String];
}

@end
