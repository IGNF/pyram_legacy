/* 

 SxS : A C++ implementation of the scale climbing algorithm for multiscale image segmentation.
 © 2004 Laurent Guigues (laurent.guigues@ign.fr)

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
#ifndef __sxsImageScaleSets_h__
#define __sxsImageScaleSets_h__
//===========================================================
// namespace	: sxs
// class		: ImageScaleSets
/*! \file 
 \brief ScaleSets whose base correspond to a label image
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	17/09/04		creation
//===========================================================
#include "lgl/lglImageRam.h"

#include "sxs/sxsGeneral.h"
#include "sxs/sxsScaleSets.h"
#include <map>

namespace sxs {

	//======================================================
	/// \brief Handles a ScaleSets whose base correspond to the labels of an Image.
	///
	class ImageScaleSets: public ScaleSets {
	public:
//			typedef std::set<pSet> SetOfSets;

		//======================================================
		/// Ctor
		ImageScaleSets() :
				m_input(0), m_base(0) {
		}
		/// Dtor
		~ImageScaleSets() {
			if (m_base != 0)
				delete m_base;
			if (m_input != 0)
				delete m_input;
		}
		//======================================================

		//======================================================
		/// Loads the ImageScaleSets from an ".iss" file
		lgl::BOOL load(const lgl::Filename& filename);
		/// \brief Save the ImageScaleSets in an ".iss" file
		///
		/// If original_image_filename is set to "" (empty string) then saves the InputImage in a new file
		/// else the .iss references the file original_image_filename as the InputImage.
		lgl::BOOL save(const lgl::Filename& filename,
				const lgl::Filename& original_image_filename);
		//======================================================

		//======================================================
		/// \brief Sets the input image.
		///
		/// The image should have be created by dynamic allocation by the operator new.
		/// The pointer is captured by the object which becomes responsible for its deletion.
		void setInputImage(lgl::ImageRam* i) {
			if (m_input)
				delete m_input;
			m_input = i;
		}
		/// Returns a reference on the input image
		lgl::ImageRam& getInputImage() {
			return *m_input;
		}
		/// Returns a const reference on the input image
		const lgl::ImageRam& getInputImage() const {
			return *m_input;
		}
		//======================================================

		//======================================================
		/// \brief Sets the label image of the base.
		///
		/// The image should have be created by dynamic allocation by the operator new.
		/// The pointer is captured by the object which becomes responsible for its deletion.
		void setBaseImage(lgl::ImageRam* i) {
			if (m_base)
				delete m_base;
			m_base = i;
		}
		/// Returns a reference on the base image
		lgl::ImageRam& getBaseImage() {
			return *m_base;
		}
		/// Returns a const reference on the base image
		const lgl::ImageRam& getBaseImage() const {
			return *m_base;
		}
		//======================================================

		//======================================================
		/// \name Algorithms
		///
		/// \{
		///
		/// \brief Returns the LabelImage corresponding to the section of scale s of the ScaleSets
		///
		/// \param s : scale of the section
		/// \param section : output label image
		void getSection(lgl::F32 s, lgl::pImageRam &section);
		/// \brief Returns the LabelImage corresponding to the s-section of the input image (whose labels must correspond to Set indices).
		/// Usefull to get a section on a partial base image fast (typically on a crop)
		///
		/// \param in : input label image (unchanged, however the use of its auto-iterator disables const qualifier)
		/// \param scale : scale of the section
		/// \param section : output label image
		void getSection(lgl::ImageRam &in, lgl::F32 scale, lgl::pImageRam &section);

		//======================================================
		/// \}
		//======================================================

		//================================================================
		/// Deletes the sets whose persistence is lower than a given threshold.
		/// Performs a top-down traversal to compute the scale of disappearance of the sets
		/// and simultaneously removes the sets of too low persistence.
		/// Also updates the base image if base sets are non persistent.
		void removeNonPersistentSets(lgl::F32 minimum_persistence = 1.);
		//================================================================

		//================================================================
		/// Tests that the base image labels are in a 1 to 1 mapping with the base sets indices
		lgl::BOOL testBaseImageCoherence();
		//================================================================

	protected:
		/// The input image
		lgl::ImageRam* m_input;
		/// The base image
		lgl::ImageRam* m_base;

		//================================================================
		/// Recursive function used to mark non persistent sets in removeNonPersistentSets.
		/// Also collects non persistent base sets and maps their index to the one of their minimal persistent ancestor (npmap)
		/// (pa is the current minimal persistent ancestor index)
		void m_rec_mark_non_persistent_and_map_npbase(Set* s, const Slot& m, lgl::F32 scale_of_disappearance,
				lgl::F32 minimum_persistence, int pa, std::map<int, int>& npbase);
		//================================================================

	};
//======================================================
// EO class ImageScaleSets
//======================================================

}

//===========================================================
// EOF
//===========================================================
#endif
