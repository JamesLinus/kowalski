
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
    printf("Build engine data:\n");
    printf("    kowalski enginedata\n");
    printf("        -xml xmlfile\n");
    printf("            The path to the project data XML file (required).\n");
    printf("        -xsd xsdfile\n");
    printf("            The path to the project data XML schema (required).\n");
    printf("        -bin binfile\n");
    printf("            The output binary file (required).\n");
    printf("\n");
    printf("Build all wavebanks:\n");
    printf("    kowalski wavebanks\n");
    printf("        -xml xmlfile\n");
    printf("            The path to the project data XML file (required).\n");
    printf("        -xsd xsdfile\n");
    printf("            The path to the project data XML schema (required).\n");
    printf("        -dir outputdir\n");
    printf("            The directory to write wave banks to (required).\n");
    printf("\n");
    printf("Build specific wavebank:\n");
    printf("    kowalski wavebank\n");
    printf("        -xml xmlfile\n");
    printf("            The path to the project data XML file (required).\n");
    printf("        -xsd xsdfile\n");
    printf("            The path to the project data XML schema (required).\n");
    printf("        -id bankid\n");
    printf("            The id of the wave bank to build (required).\n");
    printf("        -dir outputdir\n");
    printf("            The directory to write wave banks to (required).\n");
    printf("\n");
    printf("Validate project data XML or engine data or wave bank binary file:\n");
    printf("    kowalski validate\n");
    printf("        -file filepath\n");
    printf("            The path to the file to validate (required).\n");
    printf("        -xsd xsdfile\n");
    printf("            The path to the project data XML schema (required if validating XML data).\n");
    printf("\n");
    printf("View contents of engine data or wave bank binary file:\n");
    printf("    kowalski view\n");
    printf("        -file filepath\n");
    printf("            The path to the file to view (required).\n");

}

enum
{
    XML_ARG = 0,
    XSD_ARG = 1,
    BIN_ARG = 2,
    DIR_ARG = 3,
    FILE_ARG = 4,
    ID_ARG = 5,
    NUM_VALID_ARG_NAMES = 6
};

static const char* argNames[NUM_VALID_ARG_NAMES] =
{
    "-xml",
    "-xsd",
    "-bin",
    "-dir",
    "-file",
    "-id",
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

static int checkRequiredArguments(int argc, const char * argv[], int numArgs, const char** names)
{
    //check for missing arguments
    int success = 1;
    for (int i = 2; i < numArgs; i++)
    {
        int argFound = 0;
        for (int j = 0; j < NUM_VALID_ARG_NAMES; j++)
        {
            if (strcmp(argNames[j], names[i]))
            {
                argFound = 1;
                break;
            }
        }
        
        if (!argFound)
        {
            printf("Missing required argument %s\n", names[i]);
            success = 0;
        }
    }
    
    //check for extra invalid arguments
    for (int i = 2; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            continue;
        }
        
        int match = 0;
        for (int j = 0; j < numArgs; j++)
        {
            if (strcmp(argv[i], names[j]) == 0)
            {
                match = 1;
                break;
            }
        }
        
        if (!match)
        {
            printf("Found extra invalid argument %s\n", argv[i]);
            success = 0;
        }
        
    }
    
    if (!success)
    {
        printf("\n");
        printUsage();
    }
    
    return success;
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
    if (0)
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
            const char* reqArgs[3] =
            {
                argNames[XML_ARG],
                argNames[XSD_ARG],
                argNames[BIN_ARG],
            };
            
            if (!checkRequiredArguments(argc, argv, 3, reqArgs))
            {
                return 0;
            }
            
            const char* xmlPath = getArgumentValue(argc, argv, argNames[XML_ARG]);
            const char* xsdPath = getArgumentValue(argc, argv, argNames[XSD_ARG]);
            const char* binPath = getArgumentValue(argc, argv, argNames[BIN_ARG]);
            
            const int validPaths = checkPathExists(xmlPath) &&  checkPathExists(xsdPath);
            if (!validPaths)
            {
                return 0;
            }

            kwlResultCode result = kwlBuildEngineData(xmlPath, xsdPath, binPath, kwlDefaultLogCallback);
            printf("Engine data binary written to '%s'\n", binPath);
            if (result != KWL_SUCCESS)
            {
                return 0;
            }
            
        }
        else if (strcmp(operation, validate) == 0)
        {
            const char* reqArgs[2] =
            {
                argNames[FILE_ARG],
                argNames[XSD_ARG],
            };
            
            if (!checkRequiredArguments(argc, argv, 2, reqArgs))
            {
                return 0;
            }
            
            const char* filePath = getArgumentValue(argc, argv, argNames[FILE_ARG]);
            const char* xsdPath = getArgumentValue(argc, argv, argNames[XSD_ARG]);
            
            const int validPaths = checkPathExists(filePath) &&  checkPathExists(xsdPath);
            if (!validPaths)
            {
                return 0;
            }
            
            /*kwlResultCode result = */kwlValidate(filePath, xsdPath, kwlDefaultLogCallback);
        }
        else if (strcmp(operation, view) == 0)
        {
            const char* reqArgs[1] =
            {
                argNames[FILE_ARG]
            };
            
            if (!checkRequiredArguments(argc, argv, 1, reqArgs))
            {
                return 0;
            }
        }
        else
        {
            printf("Invalid operation '%s'.", operation);
            return 0;
        }
    }
    
    const char* xmlPath = "/Users/perarne/code/kowalski/res/demodata/master/demoproject.xml";
    const char* xsdPath = "/Users/perarne/code/kowalski/src/tools/kowalski.xsd";
    const char* kwlPath = "/Users/perarne/code/kowalski/res/demodata/final/demoproject.kwl";
    const char* kwbPath = "/Users/perarne/code/kowalski/res/demodata/final/numbers.kwb";
    
    const char* waveBankId = "music";
    const char* wbId = "";
    const char* wbTargetDir = "/Users/perarne/code/kowalski/res/demodata/final/wbtestbuild";
    const char* wbTargetFile = "";
    const char* binTargetFile = "/Users/perarne/code/kowalski/res/demodata/final/demoproject_testwrite.kwl";
    
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
    
    
    //load kwl from xml
    if (1)
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
    if (0)
    {
        kwlBuildEngineData(xmlPath,
                           xsdPath,
                           binTargetFile,
                           kwlDefaultLogCallback);
    }
    
    //build wave bank
    if (0)
    {
        kwlBuildWaveBanks(xmlPath,
                          xsdPath,
                          wbTargetDir,
                          kwlDefaultLogCallback);
    }
    
    return 0;
}

