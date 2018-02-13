/////////////////////////////////////////////////////////////////
/*
 *  Routines to write data in Nrrd format
 *  Currently handles: raw, gzip and ascii encodings
 */
/////////////////////////////////////////////////////////////////
#include <hxNrrdIO/hxNrrdIOAPI.h>

#include <hxcore/HxData.h>
#include <hxcore/HxMessage.h>
#include <hxfield/HxUniformScalarField3.h>
#include <teem/nrrd.h>

int NrrdWriter(HxUniformScalarField3* field, const char* filename, int encoding)
{
	// Identify data type
	int nrrdType = nrrdTypeUnknown;
	switch ( field->primType() )
    {
		case McPrimType::MC_UINT8:  nrrdType = nrrdTypeUChar; break;
		case McPrimType::MC_INT8:   nrrdType = nrrdTypeChar; break;
		case McPrimType::MC_UINT16: nrrdType = nrrdTypeUShort; break;
		case McPrimType::MC_INT16:  nrrdType = nrrdTypeShort; break;
		case McPrimType::MC_INT32:  nrrdType = nrrdTypeInt; break;
		case McPrimType::MC_FLOAT:  nrrdType = nrrdTypeFloat; break;
		case McPrimType::MC_DOUBLE: nrrdType = nrrdTypeDouble; break;
		default: break;
    }

	if(nrrdType == nrrdTypeUnknown)
	{
		theMsg->printf("ERROR: unsupported output type: %s for nrrd",field->primType().getName());
		return 0;
	}

	void* data = field->lattice().dataPtr();

	Nrrd *nrrd = nrrdNew();
	NrrdIoState *nios = nrrdIoStateNew();

	if ( encoding == nrrdEncodingTypeGzip) {
		if (nrrdEncodingGzip->available() )
		{
			nrrdIoStateEncodingSet( nios, nrrdEncodingGzip );
			nrrdIoStateSet( nios, nrrdIoStateZlibLevel, 9 );
		}
		else theMsg->printf("WARNING: Nrrd library does not support Gzip compression encoding.\n Make sure Teem_ZLIB is on in CMAKE when building Nrrd library.\n");
	} else if ( encoding == nrrdEncodingTypeBzip2) {
		if (nrrdEncodingBzip2->available() )
		{
			nrrdIoStateEncodingSet( nios, nrrdEncodingBzip2 );
			// nrrdIoStateSet( nios, nrrdIoStateBzip2BlockSize, 9 );
		}
		else theMsg->printf("WARNING: Nrrd library does not support Bzip2 compression encoding.\n Make sure Teem_BZIP2 is on in CMAKE when building Nrrd library.\n");
	} else if ( encoding == nrrdEncodingTypeAscii) {
		nrrdIoStateEncodingSet( nios, nrrdEncodingAscii );
	} else {
		theMsg->printf("ERROR: Unimplemented nrrd encoding type: %d\n",encoding);
		return 0;
	}

	try
	{
		McDim3l dims = field->lattice().getDims();

		if ( nrrdWrap_va( nrrd, data, nrrdType, (size_t)3,
		    (size_t)dims[0],
		    (size_t)dims[1],
		    (size_t)dims[2] ) )
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
		McBox3f bbox = field->getBoundingBox();
		McVec3f voxelSize = field->getVoxelSize();

		// Just deal with space directions orthogonal to data axes
		// TODO: Fetch transformation and use that
		double spaceDir[NRRD_DIM_MAX][NRRD_SPACE_DIM_MAX];
		for ( int i = 0; i < 3; ++i )
		{
			for ( int j = 0; j < 3; ++j )
			{
				if (i == j) spaceDir[i][j] = (double) voxelSize[i];
				else spaceDir[i][j] = 0.0; // Can't assume that memory is zeroed
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
		theMsg->printf("ERROR: hxNrrdIO library returned error '%s'\n", err);
		free( err );
		return 0;
    }

	nrrdIoStateNix( nios );
	nrrdNix(nrrd);

    return 1; 
}

HXNRRDIO_API
int NrrdWriterRaw(HxUniformScalarField3* field, const char* filename)
{
	return NrrdWriter(field, filename, nrrdEncodingTypeRaw);
}

HXNRRDIO_API
int NrrdWriterGzip(HxUniformScalarField3* field, const char* filename)
{
	return NrrdWriter(field, filename, nrrdEncodingTypeGzip);
}

HXNRRDIO_API
int NrrdWriterAscii(HxUniformScalarField3* field, const char* filename)
{
	return NrrdWriter(field, filename, nrrdEncodingTypeAscii);
}
