/////////////////////////////////////////////////////////////////
/*
 *  Template of a read routine
 */
/////////////////////////////////////////////////////////////////
#include <NrrdIO/NrrdIOAPI.h>

#include <hxfield/HxUniformScalarField3.h>
#include <Amira/HxMessage.h>
#include <teem/nrrd.h>

NRRDIO_API
int NrrdReader(const char* filename)
{
	    
	Nrrd *nrrd = nrrdNew();
	if ( nrrdLoad( nrrd, filename, NULL ) )
		throw biffGetDone(NRRD);
    
    if ( nrrd->dim > 3 )
	{
		theMsg->printf("ERROR: for now, nrrd input can only handle data with dimension 3 or less.");
		return 0;
	}

    const int dims[3] = 
	{ 
		(nrrd->dim > 0) ? nrrd->axis[0].size : 1,
		(nrrd->dim > 1) ? nrrd->axis[1].size : 1,
		(nrrd->dim > 2) ? nrrd->axis[2].size : 1 
	};
	
	if(nrrd->type != nrrdTypeUChar)
	{
		theMsg->printf("ERROR: for now, nrrd input can only uchar data.");
		return 0;
	}
	
	// Create a new scalar field (assume byte for now)
	HxUniformScalarField3* field = 
		new HxUniformScalarField3(dims,McPrimType::mc_uint8,nrrd->data);

	// Shouldn't need to check for data loading
	HxData::registerData(field, filename);

    return 1;
}

