

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
    
    const char* waveBankId = "music";
    const char* wbId = "";
    const char* wbTargetDir = "";
    const char* wbTargetFile = "";
    const char* binTargetFile = "/Users/perarne/code/kowalski/res/demodata/final/demoproject_testwrite.kwl";
    
    kwlProjectDataBinary pdb;
    kwlMemset(&pdb, 0, sizeof(kwlProjectDataBinary));
    
    //load kwl from xml
    if (0)
    {
        kwlProjectDataBinary_loadFromXML(&pdb,
                                         xmlPath,
                                         xsdPath,
                                         kwlDefaultLogCallback);
        //kwlProjectDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlProjectDataBinary_writeToBinary(&pdb, binTargetFile);
        kwlProjectDataBinary_free(&pdb);
        
        kwlProjectDataBinary_loadFromBinary(&pdb, binTargetFile, kwlDefaultLogCallback);
        //kwlProjectDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlProjectDataBinary_free(&pdb);
        //kwlDebugPrintAllocationReport();
    }
    
    //load kwl from bin
    if (0)
    {
        kwlProjectDataBinary_loadFromBinary(&pdb,
                                            kwlPath,
                                            kwlDefaultLogCallback);
        //kwlProjectDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlProjectDataBinary_free(&pdb);
    }

    //load kwb from xml
    if (0)
    {
        kwlWaveBankBinary wbb;
        kwlWaveBankBinary_loadFromXML(&wbb,
                                                    xmlPath,
                                                    xsdPath,
                                                    waveBankId,
                                                    kwlDefaultLogCallback);
        kwlWaveBankBinary_dump(&wbb, kwlDefaultLogCallback);
        kwlWaveBankBinary_free(&wbb);
    }
    
    //load kwb from bin
    if (0)
    {
        kwlWaveBankBinary wbb;
        kwlWaveBankBinary_loadFromBinary(&wbb,
                                                       kwbPath,
                                                       kwlDefaultLogCallback);
        kwlWaveBankBinary_dump(&wbb, kwlDefaultLogCallback);
        kwlWaveBankBinary_free(&wbb);
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

