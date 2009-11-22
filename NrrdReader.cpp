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
    
    if ( nrrd->dim > 3 )
	{
		theMsg->printf("ERROR: for now, nrrd input can only handle data with dimension 3 or less.");
		fclose(fp);
		return 0;
	}

    const int dims[3] = 
	{ 
		(nrrd->dim > 0) ? nrrd->axis[0].size : 1,
		(nrrd->dim > 1) ? nrrd->axis[1].size : 1,
		(nrrd->dim > 2) ? nrrd->axis[2].size : 1 
	};

    fclose(fp);
    
    if (data)
        HxData::registerData(data, filename);

    return 1;
}

