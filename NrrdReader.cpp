/////////////////////////////////////////////////////////////////
/*
 *  Amira Reader for the nrrd (Nearly Raw Raster Data) format:
 *  http://teem.sourceforge.net/nrrd/format.html
 *  Currently only supports 3d single channel data and 4d vector data
 *  Copyright 2009 Gregory Jefferis. All rights reserved.
 *  License LGPL >=2.1
 *  Certain portions of this code were copied/amended from the CMTK
 *  library: http://www.nitrc.org/projects/cmtk/
 */
/////////////////////////////////////////////////////////////////
#include <hxNrrdIO/hxNrrdIOAPI.h>

#include <hxfield/HxUniformScalarField3.h>
#include <hxcore/HxMessage.h>
#include <teem/nrrd.h>

HXNRRDIO_API
int NrrdReader(const char* filename)
{
	
	Nrrd *nrrd = nrrdNew();
	if ( nrrdLoad( nrrd, filename, NULL ) )
		throw biffGetDone(NRRD);
    
    if ( nrrd->dim > 4 )
	{
		theMsg->printf("ERROR: for now, nrrd input can only handle data with dimension 4 or less.");
		return 0;
	} else if ( nrrd->spaceDim > 3 ){
		theMsg->printf("ERROR: for now, nrrd input can only handle data with space dimension 3 or less.");
		return 0;		
	}
	
    const int dims[4] = 
	{ 
		(nrrd->dim > 0) ? nrrd->axis[0].size : 1,
		(nrrd->dim > 1) ? nrrd->axis[1].size : 1,
		(nrrd->dim > 2) ? nrrd->axis[2].size : 1,
		(nrrd->dim > 3) ? nrrd->axis[3].size : 1 
	};
	
	int pType = -1;
	switch ( nrrd->type )
	{
		case nrrdTypeUChar:  pType = McPrimType::MC_UINT8; break;
		case nrrdTypeChar:   pType = McPrimType::MC_INT8;  break;
		case nrrdTypeUShort: pType = McPrimType::MC_UINT16;break;
		case nrrdTypeShort:  pType = McPrimType::MC_INT16; break;
		case nrrdTypeInt:    pType = McPrimType::MC_INT32; break;
		case nrrdTypeFloat:  pType = McPrimType::MC_FLOAT; break;
		case nrrdTypeDouble: pType = McPrimType::MC_DOUBLE;break;
		default: break;
	}
	
	if(pType < 0)
	{
		theMsg->printf("ERROR: unknown nrrd input type.");
		return 0;
	}
	
	// First fetch axis spacing
	double spacing[3] = { 1.0, 1.0, 1.0 };
	int firstSpaceAxis = -1;
	int numSpaceAxesSoFar = 0;
	int nonSpatialDimension = -1;
	for ( unsigned int ax = 0; ax < nrrd->dim; ++ax )
	{
		switch ( nrrd->axis[ax].kind )
		{
			case nrrdKindUnknown:
			case nrrdKindDomain:
			case nrrdKindSpace: 
			case nrrdKindTime: firstSpaceAxis=firstSpaceAxis<0?ax:firstSpaceAxis; break;
			default: nonSpatialDimension = ax; continue;
		}
		switch ( nrrdSpacingCalculate( nrrd, ax, spacing+numSpaceAxesSoFar, nrrd->axis[ax].spaceDirection ) )
		{
			case nrrdSpacingStatusScalarNoSpace:
				break;
			case nrrdSpacingStatusDirection:
				break;
			case nrrdSpacingStatusScalarWithSpace:
				theMsg->printf("WARNING: nrrdSpacingCalculate returned nrrdSpacingStatusScalarWithSpace\n");
				spacing[numSpaceAxesSoFar] = nrrd->axis[ax].spacing;
				break;
			case nrrdSpacingStatusNone:
			default:
				theMsg->printf("WARNING: no pixel spacings in Nrrd for axis %d ; setting to 1.0\n",ax);
				spacing[numSpaceAxesSoFar] = 1.0;
				break;
		}
		numSpaceAxesSoFar++;
	}
	if ( firstSpaceAxis < 0 || firstSpaceAxis >1 )
	{
		theMsg->printf("ERROR: Unable to identify first spatial axis in nrrd. Got %d\n", firstSpaceAxis);
		return 0;
	}
	
	// Figure out size of non-spatial dimension (ie vector, colour etc)
	int nDataVar = 1;
	if ( nonSpatialDimension == 0) nDataVar = dims[nonSpatialDimension];
	else if ( nonSpatialDimension > 0) {
		// At the moment we can't handle having the vector dimension come later because
		// that would require shuffling the nrrd data block to prepare it for Amira
		theMsg->printf("ERROR: Only nrrds with vector values in the 0th dimension (not %d) are currently supported\n", nonSpatialDimension);
		return 0;
	}
	
	// Initialise coordinates (these will be uniform) starting at first space axis
	HxUniformCoord3* coord = new HxUniformCoord3(&dims[firstSpaceAxis]);
	
	// Now let's set the physical dimensions
	// This is done by defining the bounding box, the range of the voxel centres
	// given in the order: xmin,xmax,ymin ...
	McBox3f bbox = coord->getBoundingBox();
	bbox[0] = std::isnan(nrrd->spaceOrigin[0])?0.0f:(float) nrrd->spaceOrigin[0];
	bbox[2] = std::isnan(nrrd->spaceOrigin[1])?0.0f:(float) nrrd->spaceOrigin[1];
	bbox[4] = std::isnan(nrrd->spaceOrigin[2])?0.0f:(float) nrrd->spaceOrigin[2];
	
	// When a dimension is 1, Amira still seems to have a defined spacing
	bbox[1] = bbox[0] + (float) spacing[0] * ( dims[0+firstSpaceAxis] == 1 ? 1 : (dims[0+firstSpaceAxis] - 1) );
	bbox[3] = bbox[2] + (float) spacing[1] * ( dims[1+firstSpaceAxis] == 1 ? 1 : (dims[1+firstSpaceAxis] - 1) );
	bbox[5] = bbox[4] + (float) spacing[2] * ( dims[2+firstSpaceAxis] == 1 ? 1 : (dims[2+firstSpaceAxis] - 1) );
	coord->setBoundingBox(bbox);
	
	// Finally initialise lattice 
	HxLattice3* lattice = new HxLattice3(nDataVar, pType, coord, nrrd->data);
	HxField3* field = HxLattice3::create(lattice);
	
	// Shouldn't need to check for data loading
	HxData::registerData(field, filename);
	
    return 1;
}

