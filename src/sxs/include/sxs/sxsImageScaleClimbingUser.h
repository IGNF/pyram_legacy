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
#ifndef __sxsImageScaleClimbingUser_h__
#define __sxsImageScaleClimbingUser_h__
//===========================================================
// namespace	: sxs
// classe		: ImageScaleClimbingUser
/*! \file 
 \brief
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	14/07/04		creation
//===========================================================
#include "lgl/lglImage.h"
#include "lgl/lglInterPixelBoundaryTracker.h"

#include "sxs/sxsGeneral.h"
#include "sxs/sxsScaleClimbingUser.h"


namespace sxs {

	class ImageScaleClimbing;

	//===========================================================
	/// Abstract definition of an User of the ImageScaleClimbing algorithm.
	///
	/// Child of ScaleClimbingUser.
	/// Adds the callbacks related to the graph construction from an image.
	class ImageScaleClimbingUser: public virtual ScaleClimbingUser {
	public:
		//===========================================================
		///@name Misc

		///@{

		//===========================================================
		/// Ctor
		ImageScaleClimbingUser() {
		}
		/// Dtor
		virtual ~ImageScaleClimbingUser() {
		}

		/// Access to the ImageScaleClimbing algorithm owning this User
		ImageScaleClimbing& getImageScaleClimbing();
		/// Const access to the ImageScaleClimbing algorithm owning this User
		const ImageScaleClimbing& getImageScaleClimbing() const;
		//===========================================================
		///@}

		//===========================================================
		///@name Graph creation modes
		//===========================================================

		///@{

		/// \brief The differents modes to build the graph
		typedef enum {
			BuildGraphBasic, /// Basic mode : Edgel by Edgel addition (valid for 2D/3D images).
			BuildGraph2DBoundaryTracking /// For 2D images : Boundary tracking (cf. lglInterPixelBoundaryTracking.h)
		} BuildGraphMode;
		///@}

		//===========================================================
		///@name Graph creation mode
		//===========================================================
		///@{
		/// \brief Queries the mode that must be used to build the graph
		virtual BuildGraphMode SCCB_Q_buildGraphMode() {
			return BuildGraphBasic;
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Graph attributes allocation callbacks
		//===========================================================

		///@{

		/// \brief Queries the allocation of a new Node attribute.
		///
		/// The user must allocate and return a new ScaleClimbingNodeAttribute which will be held by a Node.
		/// If the effective class of the attribute is a descendant of ScaleClimbingNodeAttribute then the user
		/// is responsible for the allocation (operator new) of the good object type (The ImageScaleClimbing algorithm is then responsible for its deletion).
		/// If this is not done, future dynamic down casts will fail...
		virtual ScaleClimbingNodeAttribute* SCCB_Q_newNodeAttribute() {
			return new ScaleClimbingNodeAttribute;
		}

		/// \brief Queries the allocation of a new Edge attribute.
		///
		/// The user must allocate and return a new ScaleClimbingEdgeAttribute which will be held by an Edge.
		/// If the effective class of the attribute is a descendant of ScaleClimbingEdgeAttribute then the user
		/// is responsible for the allocation (operator new) of the good object type (The ImageScaleClimbing algorithm is then responsible for its deletion).
		/// If this is not done, future dynamic down casts will fail...
		virtual ScaleClimbingEdgeAttribute* SCCB_Q_newEdgeAttribute() {
			return new ScaleClimbingEdgeAttribute;
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Graph elements creation related callbacks : Complexity energy (C) updates
		//===========================================================

		//===========================================================
		///@{

		/// \brief Called after the creation of a new Node (called in ImageScaleClimbing::buildGraph()).
		///
		/// The attribute of the Node has been allocated (by a callback to SCCB_Q_newNodeAttribute()).
		/// The user can thus fill the Node's attribute with custom values
		virtual void SCCB_C_onNodeCreation(Node n) {
		}

		/// \brief Called at the insertion of an ImageSite in a Node (called in ImageScaleClimbing::buildGraph()).
		///
		/// The attribute of the Node has been allocated (by a callback to SCCB_Q_newNodeAttribute()).
		/// The user can thus fill the Node's attribute with custom values
		virtual void SCCB_C_addSiteToNode(Node n, const lgl::ImageSite& s) {
		}

		/// \brief Called after the creation of a new Edge (called in ImageScaleClimbing::buildGraph()).
		///
		/// The attribute of the Edge has been allocated (by a callback to SCCB_Q_newEdgeAttribute()).
		/// The user can thus fill the Edge's attribute with custom values
		virtual void SCCB_C_onEdgeCreation(Edge e) {
		}

		/// \brief Called at the insertion of an interpixel edge element (edgel) in an Edge
		/// (called in ImageScaleClimbing::buildGraph() when the creation mode is BuildGraphBasic).
		///
		/// \param left is the ImageSite which is on the left of the edgel and which corresponds to the Node e.from()
		/// \param right is the ImageSite which is on the right of the edgel and which corresponds to the Node e.to()
		/// The user can fill the Edge's attribute with custom values
		virtual void SCCB_C_addEdgelToEdge(Edge e, const lgl::ImageSite& left,
				const lgl::ImageSite& right) {
		}

		/// \brief Sets the ign::image::segment::lgl::InterPixelBoundary associated with an Edge
		/// (called in ImageScaleClimbing::buildGraph() when the creation mode is BuildGraph2DBoundaryTracking).
		///
		/// \param bnd : the InterPixelBoundary associated with the edge e
		virtual void SCCB_C_setEdgeBoundary(Edge e, const lgl::InterPixelBoundary& bnd) {
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Graph elements creation related callbacks : Fidelity energy (D) updates
		//===========================================================

		//===========================================================
		///@{

		/// \brief Called after the creation of a new Node (called in ImageScaleClimbing::buildGraph()).
		///
		/// The attribute of the Node has been allocated (by a callback to SCCB_Q_newNodeAttribute()).
		/// The user can thus fill the Node's attribute with custom values
		virtual void SCCB_D_onNodeCreation(Node n) {
		}

		/// \brief Called at the insertion of an ImageSite in a Node (called in ImageScaleClimbing::buildGraph()).
		///
		/// The attribute of the Node has been allocated (by a callback to SCCB_Q_newNodeAttribute()).
		/// The user can thus fill the Node's attribute with custom values
		virtual void SCCB_D_addSiteToNode(Node n, const lgl::ImageSite& s) {
		}

		/// \brief Called after the creation of a new Edge (called in ImageScaleClimbing::buildGraph()).
		///
		/// The attribute of the Edge has been allocated (by a callback to SCCB_Q_newEdgeAttribute()).
		/// The user can thus fill the Edge's attribute with custom values
		virtual void SCCB_D_onEdgeCreation(Edge e) {
		}

		/// \brief Called at the insertion of an interpixel edge element (edgel) in an Edge
		/// (called in ImageScaleClimbing::buildGraph() when the creation mode is BuildGraphBasic).
		///
		/// \param left is the ImageSite which is on the left of the edgel and which corresponds to the Node e.from()
		/// \param right is the ImageSite which is on the right of the edgel and which corresponds to the Node e.to()
		/// The user can fill the Edge's attribute with custom values
		virtual void SCCB_D_addEdgelToEdge(Edge e, const lgl::ImageSite& left,
				const lgl::ImageSite& right) {
		}

		/// \brief Sets the ign::image::segment::lgl::InterPixelBoundary associated with an Edge
		/// (called in ImageScaleClimbing::buildGraph() when the creation mode is BuildGraph2DBoundaryTracking).
		///
		/// \param bnd : the InterPixelBoundary associated with the edge e
		virtual void SCCB_D_setEdgeBoundary(Edge e, const lgl::InterPixelBoundary& bnd) {
		}
		///@}
		//===========================================================

		//===========================================================
		///@name Notification callbacks (N)
		/// Ponctual callbacks notifying the current algorithm state, progress...

		///@{

		/// Called back before the base segmentation computation graph (beginning of ImageScaleClimbing::computeBaseSegmentation())
		virtual void SCCB_N_preComputeBaseSegmentationNotification() {
		}
		/// Called back after the base segmentation computation graph (end of ImageScaleClimbing::computeBaseSegmentation())
		virtual void SCCB_N_postComputeBaseSegmentationNotification() {
		}
		/// Called back before the graph contruction (beginning of ImageScaleClimbing::buildGraph())
		virtual void SCCB_N_preBuildGraphNotification() {
		}
		/// Called back after the graph contruction (end of ImageScaleClimbing::buildGraph())
		virtual void SCCB_N_postBuildGraphNotification() {
		}

		///@}
		//===========================================================

	};
//===========================================================
// EO class ImageScaleClimbingUser
//===========================================================

}
//===========================================================
// EOF
//===========================================================
#endif
