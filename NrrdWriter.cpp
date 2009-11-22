/////////////////////////////////////////////////////////////////
/*
 *  Template of a write routine
 */
/////////////////////////////////////////////////////////////////
#include <NrrdIO/NrrdIOAPI.h>

#include <Amira/HxData.h>
#include <Amira/HxMessage.h>
#include <hxfield/HxUniformScalarField3.h>
#include <teem/nrrd.h>

NRRDIO_API
int NrrdWriter(HxUniformScalarField3* field, const char* filename)
{
	int compressed = 0;
	// Identify data type
	int nrrdType = nrrdTypeUnknown;
	switch ( field->primType() )
    {
		case McPrimType::mc_uint8:  nrrdType = nrrdTypeUChar; break;
		case McPrimType::mc_int8:   nrrdType = nrrdTypeChar; break;
		case McPrimType::mc_uint16: nrrdType = nrrdTypeUShort; break;
		case McPrimType::mc_int16:  nrrdType = nrrdTypeShort; break;
		case McPrimType::mc_int32:  nrrdType = nrrdTypeInt; break;
		case McPrimType::mc_float:  nrrdType = nrrdTypeFloat; break;
		case McPrimType::mc_double: nrrdType = nrrdTypeDouble; break;
		default: break;
    }

	if(nrrdType == nrrdTypeUnknown)
	{
		theMsg->printf("ERROR: unsupported output type: %s for nrrd",field->primType().getName());
		return 0;
	}

	void* data = field->lattice.dataPtr();

	Nrrd *nrrd = nrrdNew();
	NrrdIoState *nios = nrrdIoStateNew();

	if ( compressed ) {
		if (nrrdEncodingGzip->available() )
		{
			nrrdIoStateEncodingSet( nios, nrrdEncodingGzip );
			nrrdIoStateSet( nios, nrrdIoStateZlibLevel, 9 );
		}
		else theMsg->printf("WARNING: Nrrd library does not support Gzip compression encoding.\nPlease add -DTEEM_ZLIB to compiler options when building Nrrd library.\n");
	}

	try
	{
		if ( nrrdWrap_va( nrrd, data, nrrdType, (size_t)3,
		    (size_t)field->lattice.dimsInt()[0],
		    (size_t)field->lattice.dimsInt()[1],
		    (size_t)field->lattice.dimsInt()[2] ) )
		{
			throw( biffGetDone(NRRD) );
		}

		nrrdSpaceDimensionSet( nrrd, 3 );

		// TODO: Would be nice to set space units.  How does Amira store this?
//		if ( writeVolume->MetaKeyExists(CMTK_META_SPACE_UNITS_STRING) )
//		{
//			nrrd->spaceUnits[0] = strdup( writeVolume->m_MetaInformation[CMTK_META_SPACE_UNITS_STRING].c_str() );
//			nrrd->spaceUnits[1] = strdup( writeVolume->m_MetaInformation[CMTK_META_SPACE_UNITS_STRING].c_str() );
//			nrrd->spaceUnits[2] = strdup( writeVolume->m_MetaInformation[CMTK_META_SPACE_UNITS_STRING].c_str() );
//		}

		int kind[NRRD_DIM_MAX] = { nrrdKindDomain, nrrdKindDomain, nrrdKindDomain };
		nrrdAxisInfoSet_nva( nrrd, nrrdAxisInfoKind, kind );

		// TODO: Would be nice to write some kind of space if this exists

		// Fetch bounding box information and voxel size
		float* bbox = field->bbox();
		McVec3f voxelSize = field->getVoxelSize();

		// Just deal with space directions orthogonal to data axes
		// TODO: Fetch transformation and use that
		double spaceDir[NRRD_DIM_MAX][NRRD_SPACE_DIM_MAX];
		for ( int i = 0; i < 3; ++i )
		{
			for ( int j = 0; j < 3; ++j )
			{
				if (i == j) spaceDir[i][j] = (double) voxelSize[i];
			}
		}
		nrrdAxisInfoSet_nva( nrrd, nrrdAxisInfoSpaceDirection, spaceDir );

		double origin[NRRD_DIM_MAX] = { bbox[0], bbox[2], bbox[4] };
		if ( nrrdSpaceOriginSet( nrrd, origin ) )
		{
			throw( biffGetDone(NRRD) );
		}

		nrrdAxisInfoSet_va( nrrd, nrrdAxisInfoLabel, "x", "y", "z" );

		if ( nrrdSave( filename, nrrd, nios ) )
		{
			throw( biffGetDone(NRRD) );
		}
    }
	catch ( char* err )
    {
		theMsg->printf("ERROR: NrrdIO library returned error '%s'\n", err);
		free( err );
		return 0;
    }

	nrrdIoStateNix( nios );
	nrrdNix(nrrd);

    return 1; 
}
