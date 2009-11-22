/////////////////////////////////////////////////////////////////
/*
 *  Template of a read routine
 */
/////////////////////////////////////////////////////////////////
#include <NrrdIO/NrrdIOAPI.h>

#include <Amira/HxData.h>
#include <Amira/HxMessage.h>
#include <teem/nrrd.h>
#include <zlib.h>

NRRDIO_API
int NrrdReader(const char* filename)
{
    FILE* fp = fopen(filename,"rb");
    
    if (!fp) {
		theMsg->ioError(filename);
		return 0;
    }

    HxData* data = 0;

    /*
     * Create data object and read file ...
     */
    
	Nrrd *nrrd = nrrdNew();
	if ( nrrdLoad( nrrd, filename, NULL ) )
		throw biffGetDone(NRRD);
    
    fclose(fp);
    
    if (data)
        HxData::registerData(data, filename);

    return 1;
}

