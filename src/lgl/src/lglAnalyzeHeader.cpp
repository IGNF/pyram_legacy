#include "lgl/lglAnalyzeHeader.h"
#include "lgl/lglMachine.h"

namespace lgl {
	/*
		/// Acceptable values for hdr.ic.datatype
		#define HDRH_DT_UNKNOWN			0
		#define HDRH_DT_BINARY			1
		#define HDRH_DT_UNSIGNED_CHAR	2
		#define HDRH_DT_SIGNED_SHORT	4
		#define HDRH_DT_SIGNED_INT		8
		#define HDRH_DT_FLOAT			16
		#define HDRH_DT_COMPLEX			32
		#define HDRH_DT_DOUBLE			64
		#define HDRH_DT_RGB				128
		#define HDRH_DT_ALL				255
		*/
	/**
	 Unknown = 0; =>0
		I1Code = 1; =>1
		BoolCode = 2; =>1
		CharCode = 3; =>2
		I8Code = 4; =>2
		UI8Code = 5; =>2
		I16Code = 6; =>4
		UI16Code = 7; =>4
		I32Code = 8; =>8
		UI32Code = 9; =>8
		F32Code = 10; =>16
		F64Code = 11; =>64
		*/
	const int AnalyzeHeader::m_anlz_tc[12] = { 0, 1, 1, 2, 2, 2, 4, 4, 8, 8, 16, 64 };

	const int AnalyzeHeader::m_tc_anlz[9] = { Unknown, I1Code, UI8Code, I16Code, I32Code, F32Code, Unknown, F64Code, Unknown };

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
	AnalyzeHeader::AnalyzeHeader() {
		// test

		setDefault();
	}

	void AnalyzeHeader::setDefault() {
		m_data.hk.sizeof_hdr = 348;
		strcpy(m_data.hk.data_type, "dsr");
		strcpy(m_data.hk.db_name, "/home");
		m_data.hk.extents = 0;
		m_data.hk.session_error = 0;
		m_data.hk.regular = 'r';
		m_data.hk.hkey_un0 = '0';

		/* image dimension */
		m_data.ic.dim[0] = 4;
		m_data.ic.dim[1] = 0;
		m_data.ic.dim[2] = 0;
		m_data.ic.dim[3] = 0;
		m_data.ic.dim[4] = 1;
		m_data.ic.dim[5] = 0;
		m_data.ic.dim[6] = 0;
		m_data.ic.dim[7] = 0;

		strcpy(m_data.ic.vox_units, "mm");
		strcpy(m_data.ic.cal_units, "Bq/cc");
		// I USE UNUSED1 TO STORE THE NUMBER OF CHANNELS
		m_data.ic.unused1 = 1;
		m_data.ic.datatype = 0; // unknown
		m_data.ic.bitpix = 16;
		m_data.ic.dim_un0 = 0;

		m_data.ic.pixdim[0] = 0.0;
		m_data.ic.pixdim[1] = 1.0;
		m_data.ic.pixdim[2] = 1.0;
		m_data.ic.pixdim[3] = 1.0;
		m_data.ic.pixdim[4] = 0.0;
		m_data.ic.pixdim[5] = 0.0;
		m_data.ic.pixdim[6] = 0.0;
		m_data.ic.pixdim[7] = 0.0;
		m_data.ic.vox_offset = 0.0;
		m_data.ic.funused1 = 1.0;
		m_data.ic.funused2 = 0.0;
		m_data.ic.funused3 = 0.0;
		m_data.ic.cal_max = 0.0;
		m_data.ic.cal_min = 0.0;
		m_data.ic.compressed = 0;
		m_data.ic.verified = 0;
		m_data.ic.glmax = 32767;
		m_data.ic.glmin = 0;
		/* data_history */
		strcpy(m_data.hist.descrip, "image");
		strcpy(m_data.hist.aux_file, "none");
	}

//////////////////////////////////////////////////////////////////////
// read
//////////////////////////////////////////////////////////////////////
	bool AnalyzeHeader::read(const lgl::Filename& name) {
		FILE *fph;
#ifdef LGL_WITH_BOOST
		if ((fph = fopen(name.generic_string().c_str(), "rb")) == NULL)
#else
	#if wxUSE_UNICODE
			if ( (fph=fopen(name.GetFullPath().ToAscii().data(),"rb")) == NULL)
	#else
			if ((fph = fopen(name.GetFullPath().c_str(), "rb")) == NULL)
#endif // #if wxUSE_UNICODE
#endif
			return false;

		/// Tests wether the bytes are written in reverse order than the current machine settings
		/// (little/big endians)
		/// The first record should be the file size, hence if reading it doesn't match
		/// the actual file size assume the header has bytes inverted
		/// gets the file size
		fseek(fph, 0, SEEK_END);
		int file_size = ftell(fph);
		fseek(fph, 0, SEEK_SET);

		/// reads the records
		if (fread(&m_data.hk.sizeof_hdr, sizeof(int), 1, fph)) {
		}
		if (fread(m_data.hk.data_type, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hk.db_name, sizeof(char), 18, fph)) {
		}
		if (fread(&m_data.hk.extents, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hk.session_error, sizeof(short int), 1, fph)) {
		}
		if (fread(&m_data.hk.regular, sizeof(char), 1, fph)) {
		}
		if (fread(&m_data.hk.hkey_un0, sizeof(char), 1, fph)) {
		}
		if (fread(m_data.ic.dim, sizeof(short int), 8, fph)) {
		}
		if (fread(m_data.ic.vox_units, sizeof(char), 4, fph)) {
		}
		if (fread(m_data.ic.cal_units, sizeof(char), 8, fph)) {
		}
		if (fread(&m_data.ic.unused1, sizeof(short int), 1, fph)) {
		}
		if (fread(&m_data.ic.datatype, sizeof(short int), 1, fph)) {
		}
		if (fread(&m_data.ic.bitpix, sizeof(short int), 1, fph)) {
		}
		if (fread(&m_data.ic.dim_un0, sizeof(short int), 1, fph)) {
		}
		if (fread(m_data.ic.pixdim, sizeof(float), 8, fph)) {
		}
		if (fread(&m_data.ic.vox_offset, sizeof(float), 1, fph)) {
		}
		if (fread(&m_data.ic.funused1, sizeof(float), 1, fph)) {
		}
		if (fread(&m_data.ic.funused2, sizeof(float), 1, fph)) {
		}
		if (fread(&m_data.ic.funused3, sizeof(float), 1, fph)) {
		}
		if (fread(&m_data.ic.cal_max, sizeof(float), 1, fph)) {
		}
		if (fread(&m_data.ic.cal_min, sizeof(float), 1, fph)) {
		}
		if (fread(&m_data.ic.compressed, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.ic.verified, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.ic.glmax, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.ic.glmin, sizeof(int), 1, fph)) {
		}
		if (fread(m_data.hist.descrip, sizeof(char), 80, fph)) {
		}
		if (fread(m_data.hist.aux_file, sizeof(char), 24, fph)) {
		}
		if (fread(&m_data.hist.orient, sizeof(char), 1, fph)) {
		}
		if (fread(m_data.hist.originator, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hist.generated, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hist.scannum, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hist.patient_id, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hist.exp_date, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hist.exp_time, sizeof(char), 10, fph)) {
		}
		if (fread(m_data.hist.hist_un0, sizeof(char), 3, fph)) {
		}
		if (fread(&m_data.hist.views, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.vols_added, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.start_field, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.field_skip, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.omax, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.omin, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.smax, sizeof(int), 1, fph)) {
		}
		if (fread(&m_data.hist.smin, sizeof(int), 1, fph)) {
		}
		fclose(fph);

		m_rightEndian = TRUE;
		/// if opposite endian : reverse all data
		if (m_data.hk.sizeof_hdr != file_size) {
			m_rightEndian = FALSE;
			/// swaps the records
			Machine::swapEndians(&m_data.hk.sizeof_hdr, 1);
			Machine::swapEndians(m_data.hk.data_type, 10);
			Machine::swapEndians(m_data.hk.db_name, 18);
			Machine::swapEndians(&m_data.hk.extents, 1);
			Machine::swapEndians(&m_data.hk.session_error, 1);
			Machine::swapEndians(&m_data.hk.regular, 1);
			Machine::swapEndians(&m_data.hk.hkey_un0, 1);
			Machine::swapEndians(m_data.ic.dim, 8);
			Machine::swapEndians(m_data.ic.vox_units, 4);
			Machine::swapEndians(m_data.ic.cal_units, 8);
			Machine::swapEndians(&m_data.ic.unused1, 1);
			Machine::swapEndians(&m_data.ic.datatype, 1);
			Machine::swapEndians(&m_data.ic.bitpix, 1);
			Machine::swapEndians(&m_data.ic.dim_un0, 1);
			Machine::swapEndians(m_data.ic.pixdim, 8);
			Machine::swapEndians(&m_data.ic.vox_offset, 1);
			Machine::swapEndians(&m_data.ic.funused1, 1);
			Machine::swapEndians(&m_data.ic.funused2, 1);
			Machine::swapEndians(&m_data.ic.funused3, 1);
			Machine::swapEndians(&m_data.ic.cal_max, 1);
			Machine::swapEndians(&m_data.ic.cal_min, 1);
			Machine::swapEndians(&m_data.ic.compressed, 1);
			Machine::swapEndians(&m_data.ic.verified, 1);
			Machine::swapEndians(&m_data.ic.glmax, 1);
			Machine::swapEndians(&m_data.ic.glmin, 1);
			Machine::swapEndians(m_data.hist.descrip, 80);
			Machine::swapEndians(m_data.hist.aux_file, 24);
			Machine::swapEndians(&m_data.hist.orient, 1);
			Machine::swapEndians(m_data.hist.originator, 10);
			Machine::swapEndians(m_data.hist.generated, 10);
			Machine::swapEndians(m_data.hist.scannum, 10);
			Machine::swapEndians(m_data.hist.patient_id, 10);
			Machine::swapEndians(m_data.hist.exp_date, 10);
			Machine::swapEndians(m_data.hist.exp_time, 10);
			Machine::swapEndians(m_data.hist.hist_un0, 3);
			Machine::swapEndians(&m_data.hist.views, 1);
			Machine::swapEndians(&m_data.hist.vols_added, 1);
			Machine::swapEndians(&m_data.hist.start_field, 1);
			Machine::swapEndians(&m_data.hist.field_skip, 1);
			Machine::swapEndians(&m_data.hist.omax, 1);
			Machine::swapEndians(&m_data.hist.omin, 1);
			Machine::swapEndians(&m_data.hist.smax, 1);
			Machine::swapEndians(&m_data.hist.smin, 1);
		}
		///

		//	lglLOG("AnalyzeCode   = "<<m_data.ic.datatype<<ENDL);
		//	lglLOG("basicTypeCode = "<<basicTypeCode()<<ENDL);

		return true;

	}
//===============================================================

//===============================================================
/// Sets the header information to the image's characteristics
	void AnalyzeHeader::setData(const Image& i) {
		// I USE UNUSED1 TO STORE THE NUMBER OF CHANNELS
		m_data.ic.unused1 = i.size()(0);
		m_data.ic.dim[1] = i.size()(1);
		m_data.ic.dim[2] = i.size()(2);
		m_data.ic.dim[3] = i.size()(3);
		m_data.ic.datatype = basicTypeCodeToAnalyzeCode(i.type());
		m_data.ic.bitpix = BasicType::size(i.type());

		//	lglLOG("basicTypeCode = "<<i.type()<<ENDL);
		//	lglLOG("AnalyzeCode   = "<<m_data.ic.datatype<<ENDL);
		//	lglLOG("Analyze hdr write : bitpix="<<m_data.ic.bitpix<<ENDL);
	}
//===============================================================

//===============================================================
/// Writes the header 
	bool AnalyzeHeader::write(const lgl::Filename& filename) {
		FILE *fph;
#ifndef LGL_WITH_BOOST
#if wxUSE_UNICODE
		if ( (fph=fopen(filename.GetFullPath().ToAscii().data(),"wb")) == NULL)
#else
		if ((fph = fopen(filename.GetFullPath().c_str(), "wb")) == NULL)
#endif //#if wxUSE_UNICODE
#else
		if ( (fph=fopen(filename.generic_string().c_str(),"wb")) == NULL)
#endif
		{
			return false;
		}
		fwrite(&m_data.hk.sizeof_hdr, sizeof(int), 1, fph);
		fwrite(m_data.hk.data_type, sizeof(char), 10, fph);
		fwrite(m_data.hk.db_name, sizeof(char), 18, fph);
		fwrite(&m_data.hk.extents, sizeof(int), 1, fph);
		fwrite(&m_data.hk.session_error, sizeof(short int), 1, fph);
		fwrite(&m_data.hk.regular, sizeof(char), 1, fph);
		fwrite(&m_data.hk.hkey_un0, sizeof(char), 1, fph);

		fwrite(m_data.ic.dim, sizeof(short int), 8, fph);
		fwrite(m_data.ic.vox_units, sizeof(char), 4, fph);
		fwrite(m_data.ic.cal_units, sizeof(char), 8, fph);
		fwrite(&m_data.ic.unused1, sizeof(short int), 1, fph);
		fwrite(&m_data.ic.datatype, sizeof(short int), 1, fph);
		fwrite(&m_data.ic.bitpix, sizeof(short int), 1, fph);
		fwrite(&m_data.ic.dim_un0, sizeof(short int), 1, fph);
		fwrite(m_data.ic.pixdim, sizeof(float), 8, fph);
		fwrite(&m_data.ic.vox_offset, sizeof(float), 1, fph);
		fwrite(&m_data.ic.funused1, sizeof(float), 1, fph);
		fwrite(&m_data.ic.funused2, sizeof(float), 1, fph);
		fwrite(&m_data.ic.funused3, sizeof(float), 1, fph);
		fwrite(&m_data.ic.cal_max, sizeof(float), 1, fph);
		fwrite(&m_data.ic.cal_min, sizeof(float), 1, fph);
		fwrite(&m_data.ic.compressed, sizeof(int), 1, fph);
		fwrite(&m_data.ic.verified, sizeof(int), 1, fph);
		fwrite(&m_data.ic.glmax, sizeof(int), 1, fph);
		fwrite(&m_data.ic.glmin, sizeof(int), 1, fph);

		fwrite(m_data.hist.descrip, sizeof(char), 80, fph);
		fwrite(m_data.hist.aux_file, sizeof(char), 24, fph);
		fwrite(&m_data.hist.orient, sizeof(char), 1, fph);
		fwrite(m_data.hist.originator, sizeof(char), 10, fph);
		fwrite(m_data.hist.generated, sizeof(char), 10, fph);
		fwrite(m_data.hist.scannum, sizeof(char), 10, fph);
		fwrite(m_data.hist.patient_id, sizeof(char), 10, fph);
		fwrite(m_data.hist.exp_date, sizeof(char), 10, fph);
		fwrite(m_data.hist.exp_time, sizeof(char), 10, fph);
		fwrite(m_data.hist.hist_un0, sizeof(char), 3, fph);
		fwrite(&m_data.hist.views, sizeof(int), 1, fph);
		fwrite(&m_data.hist.vols_added, sizeof(int), 1, fph);
		fwrite(&m_data.hist.start_field, sizeof(int), 1, fph);
		fwrite(&m_data.hist.field_skip, sizeof(int), 1, fph);
		fwrite(&m_data.hist.omax, sizeof(int), 1, fph);
		fwrite(&m_data.hist.omin, sizeof(int), 1, fph);
		fwrite(&m_data.hist.smax, sizeof(int), 1, fph);
		fwrite(&m_data.hist.smin, sizeof(int), 1, fph);

		fclose(fph);
		return true;
	}
//===============================================================

}
// namespace lgl

// EOF

