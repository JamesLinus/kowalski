

#include <stdio.h>

#include "kwl_projectdatabinary.h"

int main(int argc, const char * argv[])
{
    kwlProjectDataBinary pdb;
    kwlProjectDataBinary_load(&pdb, "/Users/perarne/code/kowalski/res/demodata/final/demoproject.kwl");
    //kwlViewWaveBank("/Users/perarne/code/kowalski/res/demodata/final/numbers.kwb");
    //kwlValidateProjectData("/Users/perarne/code/kowalski/res/demodata/master/demoproject.xml", NULL);
    
    return 0;
}

