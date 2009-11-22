/////////////////////////////////////////////////////////////////
/*
 *  Amira Reader for the nrrd (Nearly Raw Raster Data) format:
 *  http://teem.sourceforge.net/nrrd/format.html
 *  Currently only supports 3d single channel data
 *  Copyright 2009 Gregory Jefferis. All rights reserved.
 *  License GPL >=3
 *  Certain portions of this code were copied/amended from the CMTK
 *  library: http://www.nitrc.org/projects/cmtk/
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
	
	// First fetch axis spacing
	double spacing[3] = { 1.0, 1.0, 1.0 };
	for ( size_t ax = 0; ax < nrrd->dim; ++ax )
	{
		switch ( nrrdSpacingCalculate( nrrd, ax, spacing+ax, nrrd->axis[ax].spaceDirection ) )
		{
			case nrrdSpacingStatusScalarNoSpace:
				break;
			case nrrdSpacingStatusDirection:
				break;
			case nrrdSpacingStatusScalarWithSpace:
				theMsg->printf("WARNING: nrrdSpacingCalculate returned nrrdSpacingStatusScalarWithSpace\n");
				spacing[ax] = nrrd->axis[ax].spacing;
				break;
			case nrrdSpacingStatusNone:
			default:
				theMsg->printf("WARNING: no pixel spacings in Nrrd for axis %d ; setting to 1.0\n",ax);
				spacing[ax] = 1.0;
				break;
		}
	}
	
	// Now let's set the physical dimensions
	// This is done by defining the bounding box, the range of the voxel centres
	// given in the order: xmin,xmax,ymin ...
	float *bbox = field->bbox();
	bbox[0] = isnan(nrrd->spaceOrigin[0])?0.0f:(float) nrrd->spaceOrigin[0];
	bbox[2] = isnan(nrrd->spaceOrigin[1])?0.0f:(float) nrrd->spaceOrigin[1];
	bbox[4] = isnan(nrrd->spaceOrigin[2])?0.0f:(float) nrrd->spaceOrigin[2];
	
	// When a dimension is 1, Amira still seems to have a defined spacing
	bbox[1] = bbox[0] + (float) spacing[0] * ( dims[0] == 1 ? 1 : (dims[0] - 1) );
	bbox[3] = bbox[2] + (float) spacing[1] * ( dims[1] == 1 ? 1 : (dims[1] - 1) );
	bbox[5] = bbox[4] + (float) spacing[2] * ( dims[2] == 1 ? 1 : (dims[2] - 1) );
	
	// Shouldn't need to check for data loading
	HxData::registerData(field, filename);
	
    return 1;
}

