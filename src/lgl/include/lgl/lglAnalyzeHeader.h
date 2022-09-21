/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
 � 2004 Laurent Guigues (laurent.guigues@ign.fr)

 This program is free software; you can use, modify and/or redistribute it
 under the terms of the CeCILL license, which is a French law-Compatible and
 GNU GPL-Compatible free software license published by CEA, CNRS and INRIA.

 As a counterpart to the access to the source code and rights to copy,
 modify and redistribute granted by the license, users are provided only
 with a limited warranty and the software's author, the holder of the
 economic rights, and the successive licensors have only limited liability.
 See the CeCILL license for more details.

 You should have received a copy of the CeCILL license along with this software
 (files CeCILL_LICENCE_V1-fr.txt (french) and CeCILL_LICENSE_V1-en.txt (english)).
 If not visit http://www.cecill.info.
 The fact that you are presently reading this means that you have had
 knowledge of the CeCILL license and that you accept its terms.

 */
/*! \file 
 \brief Read/write Analyze headers (3D medical images format (.hdr))
 */
#ifndef __lglAnalyzeHeader_h__
#define __lglAnalyzeHeader_h__
//===========================================================
// namespace	: lgl
// classe		: AnalyzeHeader
// Handles Analyze Headers (.hdr) for 3D medical images 
//===========================================================
// author		: Laurent Guigues
// history		: 
//	02/09/04	: creation 
//===========================================================
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "lgl/lglImage.h"

namespace lgl {

	/// Handler for "Analyze" header files (free 3D medical images headers)
	class AnalyzeHeader {
	public:
		//==============================================================
		/// Structure storing general information
		typedef struct Header_key /*      header_key       */
		{ /* off + size*/
			int sizeof_hdr; /* 0 + 4     */
			char data_type[10]; /* 4 + 10    */
			char db_name[18]; /* 14 + 18   */
			int extents; /* 32 + 4    */
			short int session_error; /* 36 + 2    */
			char regular; /* 38 + 1    */
			char hkey_un0; /* 39 + 1    */
		} h_k; /* total=40  */
		//==============================================================

		//==============================================================
		/// Structure storing the image characteristics : dimensions, voxel size, type, etc.
		typedef struct Image_characteristics /*      image_dimension  */
		{ /* off + size*/
			short int dim[8]; /* 0 + 16    */
			char vox_units[4]; /* 16 + 4    */
			char cal_units[8]; /* 20 + 4    */
			short int unused1; /* 24 + 2    *//* I USE IT AS NUMBER OF CHANNELS */
			short int datatype; /* 30 + 2    */
			short int bitpix; /* 32 + 2    */
			short int dim_un0; /* 34 + 2    */
			float pixdim[8]; /* 36 + 32   */
			/*
				pixdim[] specifies the voxel dimensions:
				pixdim[1] - voxel width
				pixdim[2] - voxel height
				pixdim[3] - interslice distance
				..etc
				*/
			float vox_offset; /* 68 + 4    */
			float funused1; /* 72 + 4    */
			float funused2; /* 76 + 4    */
			float funused3; /* 80 + 4    */
			float cal_max; /* 84 + 4    */
			float cal_min; /* 88 + 4    */
			int compressed; /* 92 + 4    */
			int verified; /* 96 + 4    */
			int glmax, glmin; /* 100 + 8   */
		} i_c; /* total=108 */
		//==============================================================

		//==============================================================
		/// Structure storing data concerning the history of the acquisition (metadata)
		typedef struct History /*      data_history     */
		{ /* off + size*/
			char descrip[80]; /* 0 + 80    */
			char aux_file[24]; /* 80 + 24   */
			char orient; /* 104 + 1   */
			char originator[10]; /* 105 + 10  */
			char generated[10]; /* 115 + 10  */
			char scannum[10]; /* 125 + 10  */
			char patient_id[10]; /* 135 + 10  */
			char exp_date[10]; /* 145 + 10  */
			char exp_time[10]; /* 155 + 10  */
			char hist_un0[3]; /* 165 + 3   */
			int views; /* 168 + 4   */
			int vols_added; /* 172 + 4   */
			int start_field; /* 176 + 4   */
			int field_skip; /* 180 + 4   */
			int omax, omin; /* 184 + 8   */
			int smax, smin; /* 192 + 8   */
		} h; /* total=200 */
		//==============================================================

		//==============================================================
		/// Full header structure
		struct Analyze_struct /*      dsr              */
		{ /* off + size*/
			struct Header_key hk; /* 0 + 40    */
			struct Image_characteristics ic; /* 40 + 108  */
			struct History hist; /* 148 + 200 */
		}; /* total = 348 */
		//==============================================================
		//==============================================================
		/// Ctor. Creates a default header
		AnalyzeHeader();
		/// Dtor
		~AnalyzeHeader() {
		}
		;

		/// Reads a .hdr file

		/// \param 	filename
		/// \return true if read successful
		bool read(const Filename& filename);

		/// Returns true iff the header was encoded with the same machine type (little/big endian).

		/// If returns false (after header read) then the data (.img) should be read with endian bytes coding reversal (assuming header and data were encoded the same way).
		BOOL rightEndian() const {
			return m_rightEndian;
		}

		/// Returns a const ref on the header information
		const Analyze_struct& getData() const {
			return m_data;
		}
		/// Returns a ref on the header information
		Analyze_struct& getData() {
			return m_data;
		}
		/// Returns the size of the image
		ImageSite size() const {
			int nbc = 1;
			if (m_data.ic.unused1 > 0)
				nbc = m_data.ic.unused1;
			if (m_data.ic.dim[1] == 0)
				return ImageSite(nbc);
			else if (m_data.ic.dim[2] == 0)
				return ImageSite(nbc, m_data.ic.dim[1]);
			else if (m_data.ic.dim[3] == 0)
				return ImageSite(nbc, m_data.ic.dim[1], m_data.ic.dim[2]);
			else
				return ImageSite(nbc, m_data.ic.dim[1], m_data.ic.dim[2], m_data.ic.dim[3]);
		}
		/// Returns the code of the type of the image (basicTypeCode)
		int basicTypeCode() const {
			return analyzeCodeToBasicTypeCode(m_data.ic.datatype);
		}

		/// Sets a default header (0x0x0 image)
		void setDefault();

		/// Sets the header information to the image's characteristics
		void setData(const Image& i);

		/// Writes the header
		bool write(const lgl::Filename& filename);

		/// Writes the header of the image
		bool write(const Image& i, const lgl::Filename& filename) {
			setData(i);
			return write(filename);
		}

		static int basicTypeCodeToAnalyzeCode(int c) {
			return m_anlz_tc[c];
		}
		static int analyzeCodeToBasicTypeCode(int c) {
			int d = 0;
			if (c != 0)
				d = (int) (1. + log((double) c + 0.1) / log(2.));
			return m_tc_anlz[d];
		}

	protected:
		/// the data
		Analyze_struct m_data;
		/// For hdr oriented img reading : is the header coded with the right endian convention (same as the current machine's one) ?
		BOOL m_rightEndian;
		/// Table of analyze codes for basic types
		static const int m_anlz_tc[12];
		/// Table of basic type codes for analyze codes
		static const int m_tc_anlz[9];
	};
// EO AnalyzeHeaderHandler

}
//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
