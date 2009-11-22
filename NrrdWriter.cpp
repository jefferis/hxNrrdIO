/////////////////////////////////////////////////////////////////
/*
 *  Template of a write routine
 */
/////////////////////////////////////////////////////////////////
#include <NrrdIO/NrrdIOAPI.h>

#include <Amira/HxData.h>
#include <Amira/HxMessage.h>
#include <hxfield/HxUniformScalarField3.h>

NRRDIO_API
int NrrdWriter(HxUniformScalarField3* data, const char* filename)
{
    FILE* fp = fopen(filename,"wb");
    
    if (!fp) {
	theMsg->ioError(filename);
	return 0;
    }

    /*
     * Write data into file ...
     */
    
    fclose(fp);
    
    return 1; 
}
