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
#ifndef __sxsImageScaleClimbing_h__
#define __sxsImageScaleClimbing_h__
//===========================================================
// namespace	: sxs
// class		: ScaleClimbingOnImage
/*! \file 
 \brief Class implementing multi-scale image segmentation by the scale climbing algorithm
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	01/09/04		creation
//===========================================================
//===========================================================
#include "lgl/lglNeighborhood.h"

#include "sxs/sxsScaleClimbing.h"
#include "sxs/sxsImageScaleClimbingUser.h"
#include "sxs/sxsImageScaleSets.h"


namespace sxs {

	//===========================================================
	/// \brief Class holding the parameters of the ImageScaleClimbing algorithm.
	///
	/// Typedefed as Parameters in the scope of ImageScaleClimbing,
	/// hence this class should be used as ImageScaleClimbing::Parameters
	/// \sa ImageScaleClimbing
	/// \sa ScaleClimbingParameters
	class ImageScaleClimbingParameters: public ScaleClimbingParameters {
	protected:
		/// Type of neighborhood used for the region adjacency graph construction from an image
		lgl::Neighborhood m_image_neighborhood;

		/// If m_masking_enabled is true, pixels whose label in the
		/// initial over-segmentation is m_masking_label will not be
		/// processed
		bool m_masking_enabled;

		/// If m_masking_enabled is true, pixels whose label in the
		/// initial over-segmentation is m_masking_label will not be
		/// processed
		int m_masking_label;

	public:
		/// \brief Ctor
		ImageScaleClimbingParameters();
		/// Dtor
		~ImageScaleClimbingParameters() {
		}
		/// Returns a reference to a static instance of the class initialized with default values
		//static ImageScaleClimbingParameters& static_instance() { static ImageScaleClimbingParameters p; return p;}

		/// Gets the type of Neighborhood used for region adjacency graph construction from an image
		lgl::Neighborhood getNeighborhood() const {
			return m_image_neighborhood;
		}
		/// Sets the type of Neighborhood used for region adjacency graph construction from an image
		void setNeighborhood(lgl::Neighborhood n = lgl::Neighborhood::d2_4n) {
			m_image_neighborhood = n;
		}

		/// Sets the masking mode
		/** Sets the masking mode. If masking is enabled, pixels whose
		 label in the initial over-segmentation have a certain value
			will not be processed. The programmer should use
			SCCB_N_postComputeBaseSegmentationNotification in the user
			class, or call setBaseSegmentation, in order to set the
			appropriate pixels in the over-segmentation label image
			m_base to that value.

			\param enable Whether masking is to be enabled or not

			\param label Which label is to be ignored
			*/
		void enable_masking(bool enable = true, int label = 0);

		/// queries whether masking is enabled or not
		bool is_masking_enabled() const {
			return m_masking_enabled;
		}
		/// gets the masking label
		int get_masking_label() const {
			return m_masking_label;
		}

	};
	//===========================================================

	//===========================================================
	/// \brief Implements the ScaleClimbing algorithm on image data
	class ImageScaleClimbing: public virtual ScaleClimbing {

	public:

		//===========================================================
		/// \name Construction, destruction
		/// \{
		//===========================================================
		/// Ctor
		ImageScaleClimbing();
		/// Dtor
		~ImageScaleClimbing();
		//===========================================================
		/// \}
		//===========================================================

		//===========================================================
		/// \name Input and parameters settings
		/// \{
		//===========================================================
		/// \brief Sets the parameters which MUST be an ImageScaleClimbingParameters
		/// as p is dynamically downcasted to an ImageScaleClimbingParameters
		void setParameters(const ScaleClimbingParameters* p);
		/// \brief Sets the input image
		///
		/// The input image should have be created by dynamic allocation by the operator new.
		/// The pointer is captured by the ImageScaleSets which becomes responsible for its deletion.
		void setImage(lgl::ImageRam*);
		/// \brief Sets an input base segmentation.
		///
		/// The label image should have be created by dynamic allocation by the operator new.
		/// The pointer is captured by the ImageScaleSets which becomes responsible for its deletion.
		void setBaseSegmentation(lgl::ImageRam*);
		/// \brief Sets the ScaleClimbingUser which MUST be an ImageScaleClimbingUser
		/// as u is dynamically downcasted to an ImageScaleClimbingUser
		void setUser(ScaleClimbingUser* u);
		/// \brief Sets the ScaleSets which MUST be an ImageScaleSets
		/// as u is dynamically downcasted to an ImageScaleSets
		void setScaleSets(ScaleSets& u);
		//===========================================================

		//===========================================================
		/// Access to the Image
		lgl::ImageRam& getImage() {
			return *m_image;
		}
		/// Const access to the Image
		const lgl::ImageRam& getImage() const {
			return *m_image;
		}
		/// Access to the BaseSegmentation
		lgl::ImageRam& getBaseSegmentation() {
			return *m_base;
		}
		/// Const access to the BaseSegmentation
		const lgl::ImageRam& getBaseSegmentation() const {
			return *m_base;
		}
		/// Access to the ImageScaleSets
		const ImageScaleSets& getImageScaleSets() const {
			return *m_iscale_sets;
		}
		/// Access to the ImageUser
		const ImageScaleClimbingUser& getImageUser() const {
			return *m_iuser;
		}
		/// Access to the ImageParameters
		const ImageScaleClimbingParameters& getImageParameters() const {
			return *m_iparam;
		}
		/// \}
		//===========================================================

		//===========================================================
		/// \name Level 0 methods
		/// \{
		//===========================================================
		/// \brief Runs the whole algorithm.
		///
		/// - If an InputImage has been set then computes the base segmentation using the method computeBaseSegmentation.
		/// - Allocates a new graph and calls buildGraph()
		///	- Calls the ScaleClimbing::run() method
		/// \return TRUE if normal termination
		lgl::BOOL run();
		/// \}
		//===========================================================

		//===========================================================
		/// \name Level 1 methods
		/// \{
		//===========================================================
		/// \brief Computes the over-segmentation of the image which will be us as base in the scale climbing algorithm
		lgl::BOOL computeBaseSegmentation();
		//===========================================================
		/// \brief Builds the graph from the current base segmentation.
		///
		/// Initializes the attributes of the graph's Nodes and Edges.
		/// \return TRUE if normal termination
		lgl::BOOL buildGraph();
		//===========================================================
		/// \}
		//===========================================================

		//===========================================================
	protected:
		//===========================================================
		/// The input image
		lgl::ImageRam* m_image;
		/// The label image of the base segmentation
		lgl::ImageRam* m_base;
		/// The ImageScaleClimbingUser (downcasted from ScaleClimbing::m_user)
		ImageScaleClimbingUser* m_iuser;
		/// The ImageScaleSets (downcasted from ScaleClimbing::m_scale_sets)
		ImageScaleSets* m_iscale_sets;
		/// The Parameters
		const ImageScaleClimbingParameters* m_iparam;

	};
//===========================================================
// EO class ImageScaleClimbing
//===========================================================

}

//===========================================================
// EOF
//===========================================================
#endif

