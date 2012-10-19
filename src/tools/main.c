

#include <stdio.h>

#include "kwl_projectdatabinaryrepresentation.h"
#include "kwl_wavebankbinaryrepresentation.h"
#include "kwl_binarybuilding.h"
#include "kwl_datavalidation.h"
#include "kwl_logging.h"

int main(int argc, const char * argv[])
{
    const char* xmlPath = "/Users/perarne/code/kowalski/res/demodata/master/demoproject.xml";
    const char* xsdPath = "/Users/perarne/code/kowalski/src/tools/kowalski1.0.xsd";
    const char* kwlPath = "/Users/perarne/code/kowalski/res/demodata/final/demoproject.kwl";
    const char* kwbPath = "/Users/perarne/code/kowalski/res/demodata/final/numbers.kwb";
    
    const char* wbId = "";
    const char* wbTargetDir = "";
    const char* wbTargetFile = "";
    const char* binTargetFile = "";
    
    kwlProjectDataBinaryRepresentation pdb;
    kwlMemset(&pdb, 0, sizeof(kwlProjectDataBinaryRepresentation));
    
    //load kwl from xml
    if (1)
    {
        kwlProjectDataBinaryRepresentation_loadFromXML(&pdb,
                                                       xmlPath,
                                                       xsdPath,
                                                       kwlDefaultLogCallback);
        kwlProjectDataBinaryRepresentation_dump(&pdb, kwlDefaultLogCallback);
        kwlProjectDataBinaryRepresentation_free(&pdb);
    }
    
    //load kwl from bin
    if (0)
    {
        kwlProjectDataBinaryRepresentation_loadFromBinary(&pdb,
                                                          kwlPath,
                                                          kwlDefaultLogCallback);
        kwlProjectDataBinaryRepresentation_dump(&pdb, kwlDefaultLogCallback);
        kwlProjectDataBinaryRepresentation_free(&pdb);
    }

    //load kwb from bin
    if (0)
    {
        kwlWaveBankBinaryRepresentation wbb;
        kwlWaveBankBinaryRepresentation_loadFromBinary(&wbb,
                                                       kwbPath,
                                                       kwlDefaultLogCallback);
        kwlWaveBankBinaryRepresentation_dump(&wbb, kwlDefaultLogCallback);
        kwlWaveBankBinaryRepresentation_free(&wbb);
    }
    
    //validate xml
    if (0)
    {
        kwlValidateProjectData(xmlPath, xsdPath, kwlDefaultLogCallback);
    }
    
    //validate kwl
    if (0)
    {
        kwlValidateEngineData(kwlPath, kwlDefaultLogCallback);
    }
    
    //validate kwb
    if (0)
    {
        kwlValidateWaveBank(kwbPath, kwlDefaultLogCallback);
    }
    
    //build engine data
    if (0)
    {
        kwlBuildEngineData(xmlPath,
                           xsdPath,
                           binTargetFile);
    }
    
    //build wave bank
    if (0)
    {
        kwlBuildWaveBank(xmlPath, xsdPath, wbId, wbTargetDir);
        kwlBuildWaveBanks(xmlPath, xsdPath, wbTargetFile);
    }
    
    return 0;
}

