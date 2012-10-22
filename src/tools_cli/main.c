
#include <string.h>
#include <stdio.h>

#include "kwl_enginedatabinary.h"
#include "kwl_wavebankbinary.h"
#include "kwl_binarybuilding.h"
#include "kwl_datavalidation.h"
#include "kwl_logging.h"
#include "kwl_fileutil.h"

static void printUsage()
{
    printf("Build engine data and/or wave banks:\n");
    printf("    kowalski build\n");
    printf("        -xml xmlfile\n");
    printf("            The path to the project data XML file (required).\n");
    printf("        -xsd xsdfile\n");
    printf("            The path to the project data XML schema (required).\n");
    printf("        -dir wavebankdir\n");
    printf("            The wave bank binary output directory (required if -bin is missing).\n");
    printf("        -bin binfile\n");
    printf("            The output engine data binary file (required if -dir is missing).\n");
    printf("\n");
    printf("Validate project a project data XML file:\n");
    printf("    kowalski validate\n");
    printf("        -xml filepath\n");
    printf("            The path to the project XML file (required).\n");
    printf("        -xsd xsdfile\n");
    printf("            The path to the project data XML schema (required if validating XML data).\n");
    printf("\n");
    printf("View contents of engine data or wave bank binary file:\n");
    printf("    kowalski view\n");
    printf("        -bin filepath\n");
    printf("            The path to the file to view (required).\n");

}

enum
{
    XML_ARG = 0,
    XSD_ARG = 1,
    BIN_ARG = 2,
    DIR_ARG = 3,
    NUM_VALID_ARG_NAMES = 4
};

static const char* argNames[NUM_VALID_ARG_NAMES] =
{
    "-xml",
    "-xsd",
    "-bin",
    "-dir",
};

static const char* getArgumentValue(int argc, const char * argv[], const char* name)
{
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], name) == 0)
        {
            return argv[i + 1];
        }
    }
    
    return NULL;
}

int checkPathExists(const char* p)
{
    if (p == NULL)
    {
        //Handle this in a better way
        printf("Path is null\n");
        return 0;
    }
    
    int exists = kwlDoesFileExist(p);
    
    if (!exists)
    {
        printf("'%s' does not exist\n", p);
        return 0;
    }
    
    return 1;
}

/**
 * kowalski build -p xmlpath -s xsdpath -o binpath -w dir
 * kowalski validate -p filepath -s xsdpath
 * kowalski view -f filepath
 */
int main(int argc, const char * argv[])
{
    if (1)
    {
        if (argc <= 2)
        {
            printf("Too few arguments.\n\n");
            printUsage();
            return 0;
        }
        
        const char* operation = argv[1];
        const char* build = "build";
        const char* validate = "validate";
        const char* view = "view";

        
        if (strcmp(operation, build) == 0)
        {
            const char* xmlPath = getArgumentValue(argc - 2, &argv[2], argNames[XML_ARG]);
            const char* xsdPath = getArgumentValue(argc - 2, &argv[2], argNames[XSD_ARG]);
            const char* binPath = getArgumentValue(argc - 2, &argv[2], argNames[BIN_ARG]);
            const char* wbDir = getArgumentValue(argc - 2, &argv[2], argNames[DIR_ARG]);
            
            if (binPath != NULL)
            {
                kwlResultCode result = kwlBuildEngineData(xmlPath, xsdPath, binPath, 0, kwlDefaultLogCallback);
            }
            if (wbDir != NULL)
            {
                kwlResultCode result = kwlBuildWaveBanks(xmlPath,
                                                         xsdPath,
                                                         wbDir,
                                                         0,
                                                         kwlDefaultLogCallback);
            }
            
        }
        else if (strcmp(operation, validate) == 0)
        {
            const char* xmlPath = getArgumentValue(argc - 2, &argv[2], argNames[XML_ARG]);
            const char* xsdPath = getArgumentValue(argc - 2, &argv[2], argNames[XSD_ARG]);
            /*kwlResultCode result = */kwlValidate(xmlPath, xsdPath, kwlDefaultLogCallback);
        }
        else if (strcmp(operation, view) == 0)
        {
            const char* binPath = getArgumentValue(argc - 2, &argv[2], argNames[BIN_ARG]);
            if (kwlFileIsEngineDataBinary(binPath))
            {
                kwlEngineDataBinary edb;
                kwlEngineDataBinary_loadFromBinaryFile(&edb,
                                                       binPath,
                                                       kwlDefaultLogCallback);
                kwlEngineDataBinary_dump(&edb, kwlDefaultLogCallback);
                kwlEngineDataBinary_free(&edb);
            }
            else if (kwlFileIsWaveBankBinary(binPath))
            {
                kwlWaveBankBinary wbb;
                kwlWaveBankBinary_loadFromBinaryFile(&wbb,
                                                     binPath,
                                                     kwlDefaultLogCallback);
                kwlWaveBankBinary_dump(&wbb, kwlDefaultLogCallback);
                kwlWaveBankBinary_free(&wbb);
            }
            else
            {
                printf("'%s' is not an engine data or wave bank binary file.\n", binPath);
            }
            
        }
        else
        {
            printf("Invalid operation '%s'.", operation);
            return 0;
        }
    }
    
    return 0;
    
    const char* xmlPath = "/Users/perarne/code/kowalski/res/demodata/master/demoproject.xml";
    const char* xsdPath = "/Users/perarne/code/kowalski/src/tools/kowalski.xsd";
    const char* kwlPath = "/Users/perarne/code/kowalski/res/demodata/final/demoproject.kwl";
    const char* kwbPath = "/Users/perarne/code/kowalski/res/demodata/final/numbers.kwb";
    
    const char* waveBankId = "music";
    const char* wbId = "";
    const char* wbTargetDir = "/Users/perarne/code/kowalski/res/demodata/final";
    const char* wbTargetFile = "";
    const char* binTargetFile = "/Users/perarne/code/kowalski/res/demodata/final/demoproject.kwl";
    
    kwlEngineDataBinary pdb;
    kwlMemset(&pdb, 0, sizeof(kwlEngineDataBinary));
    
    //view ed
    if (0)
    {
        kwlEngineDataBinary_loadFromBinaryFile(&pdb, kwlPath, kwlDefaultLogCallback);
        kwlEngineDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlEngineDataBinary_free(&pdb);
        
        kwlEngineDataBinary_loadFromBinaryFile(&pdb, binTargetFile, kwlDefaultLogCallback);
        kwlEngineDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlEngineDataBinary_free(&pdb);
    }
    
    //build wave banks
    if (1)
    {
        kwlBuildWaveBanks(xmlPath,
                          xsdPath,
                          wbTargetDir,
                          1,
                          kwlDefaultLogCallback);
    }
    
    //compare wbs
    if (0)
    {
        const char* musicOld = "/Users/perarne/code/kowalski/res/demodata/final/music.kwb";
        const char* musicNew = "/Users/perarne/code/kowalski/res/demodata/final/wbtestbuild/music.kwb";
        kwlWaveBankBinary w;
        kwlWaveBankBinary_loadFromBinaryFile(&w, musicNew, kwlDefaultLogCallback);
        kwlWaveBankBinary_dump(&w, kwlDefaultLogCallback);
        kwlWaveBankBinary_free(&w);
        
        kwlWaveBankBinary_loadFromBinaryFile(&w, musicOld, kwlDefaultLogCallback);
        kwlWaveBankBinary_dump(&w, kwlDefaultLogCallback);
        kwlWaveBankBinary_free(&w);
        
    }
    
    //load kwl from xml
    if (0)
    {
        kwlEngineDataBinary_loadFromXMLFile(&pdb,
                                        xmlPath,
                                        xsdPath,
                                        1,
                                        kwlDefaultLogCallback);
        kwlEngineDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlEngineDataBinary_writeToFile(&pdb, binTargetFile);
        kwlEngineDataBinary_free(&pdb);
        
        kwlEngineDataBinary_loadFromBinaryFile(&pdb, binTargetFile, kwlDefaultLogCallback);
        kwlEngineDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlEngineDataBinary_free(&pdb);
        kwlDebugPrintAllocationReport();
    }
    
    //load kwl from bin
    if (0)
    {
        kwlEngineDataBinary_loadFromBinaryFile(&pdb,
                                           kwlPath,
                                           kwlDefaultLogCallback);
        kwlEngineDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlEngineDataBinary_free(&pdb);
        
        kwlEngineDataBinary_loadFromBinaryFile(&pdb,
                                               binTargetFile,
                                               kwlDefaultLogCallback);
        //kwlEngineDataBinary_dump(&pdb, kwlDefaultLogCallback);
        kwlEngineDataBinary_free(&pdb);
        
    }
    
    //load kwb from xml
    if (0)
    {
        /*
        kwlWaveBankBinary wbb;
        kwlWaveBankBinary_create(&wbb,
                                      xmlPath,
                                      xsdPath,
                                      waveBankId,
                                      kwlDefaultLogCallback);
        kwlWaveBankBinary_dump(&wbb, kwlDefaultLogCallback);
        kwlWaveBankBinary_free(&wbb);*/
    }
    
    //load kwb from bin
    if (0)
    {
        kwlWaveBankBinary wbb;
        
        kwlWaveBankBinary_loadFromBinaryFile(&wbb,
                                             kwbPath,
                                             kwlDefaultLogCallback);
        kwlWaveBankBinary_dump(&wbb, kwlDefaultLogCallback);
        kwlWaveBankBinary_free(&wbb);
        
    }
    
    //validate xml
    if (0)
    {
        kwlResultCode result = kwlValidateProjectData(xmlPath, xsdPath, 1, kwlDefaultLogCallback);

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
    if (1)
    {
        kwlBuildEngineData(xmlPath,
                           xsdPath,
                           binTargetFile,
                           1,
                           kwlDefaultLogCallback);
    }
    
    
    
    return 0;
}

