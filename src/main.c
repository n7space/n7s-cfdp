#include <stdio.h>
#include "dataview.h"
#include "cfdp_core.h"

int main(int argc, char* argv[])
{
    asn1SccMySeq my_seq;
    asn1SccMySeq_Initialize(&my_seq);

    cfdp_core entity;
    cfdp_core_put(&entity);
    
    return 0;
}