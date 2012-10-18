

#include <stdio.h>

#include "kwl_projectdatabinaryrepresentation.h"
#include "kwl_wavebankbinaryrepresentation.h"
#include "kwl_binarybuilding.h"
#include "kwl_toolsutil.h"

int main(int argc, const char * argv[])
{
    kwlProjectDataBinaryRepresentation pdb;
    kwlProjectDataBinaryRepresentation_deserialize(&pdb,
                                                   "/Users/perarne/code/kowalski/res/demodata/final/demoproject.kwl",
                                                   kwlDefaultLogCallback);
    kwlProjectDataBinaryRepresentation_dump(&pdb, kwlDefaultLogCallback);
    kwlProjectDataBinaryRepresentation_free(&pdb);
    
    kwlWaveBankBinaryRepresentation wbb;
    kwlWaveBankBinaryRepresentation_deserialize(&wbb,
                                                "/Users/perarne/code/kowalski/res/demodata/final/numbers.kwb",
                                                kwlDefaultLogCallback);
    //kwlWaveBankBinaryRepresentation_dump(&wbb, kwlDefaultLogCallback);
    kwlWaveBankBinaryRepresentation_free(&wbb);
    
    kwlBuildEngineData("/Users/perarne/code/kowalski/res/demodata/master/demoproject.xml",
                       NULL);
    
    return 0;
}

