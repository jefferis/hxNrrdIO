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
	
	HxUniformScalarField3* field = NULL;
	
	switch ( nrrd->type )
	{
		case nrrdTypeUChar:  field = new HxUniformScalarField3(dims,MC_UINT8,nrrd->data); break;
		case nrrdTypeChar:   field = new HxUniformScalarField3(dims,MC_INT8,nrrd->data); break;
		case nrrdTypeUShort: field = new HxUniformScalarField3(dims,MC_UINT16,nrrd->data); break;
		case nrrdTypeShort:  field = new HxUniformScalarField3(dims,MC_INT16,nrrd->data); break;
		case nrrdTypeInt:    field = new HxUniformScalarField3(dims,MC_INT32,nrrd->data); break;
		case nrrdTypeFloat:  field = new HxUniformScalarField3(dims,MC_FLOAT,nrrd->data); break;
		case nrrdTypeDouble: field = new HxUniformScalarField3(dims,MC_DOUBLE,nrrd->data); break;
		default: break;
	}
	
	if(field == NULL)
	{
		theMsg->printf("ERROR: unknown nrrd input type.");
		return 0;
	}
		
	// Shouldn't need to check for data loading
	HxData::registerData(field, filename);
	
    return 1;
}

