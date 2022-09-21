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
#ifndef __sxsC_Boundary_Measure_h__
#define __sxsC_Boundary_Measure_h__
//===========================================================
#include "sxs/sxsGeneral.h"
#include "sxs/sxsImageScaleClimbingUser.h"
//===========================================================
// namespace	: sxs
// classes		: 
//	C_BoundaryMeasure_User
//	C_BoundaryMeasure_NodeAttribute
//	C_BoundaryMeasure_EdgeAttribute
//  Defines the Complexity Energy (C) based on the measure
//  of the boundary of a region and the related minimal
//  graph attributes needed. 
/*! \file 
 \brief ImageScaleCimbingUser implementing complexity energies (C) based on a measure of the boundary of the regions (e.g. length).
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	25/07/04		creation
//===========================================================


namespace sxs {

	//===========================================================
	/// Minimal graph node attribution when using the energy class C_BoundaryMeasure
	class C_BoundaryMeasure_NodeAttribute: public ScaleClimbingNodeAttribute {
	public:
		/// ctor
		C_BoundaryMeasure_NodeAttribute(lgl::F32 boundary_measure = 0) :
				m_boundary_measure(boundary_measure) {
		}

		/// returns a ref on the boundary measure of the Node (sum of the boundaryMeasures of its incident Edges)
		inline lgl::F32& boundaryMeasure() {
			return m_boundary_measure;
		}
		/// returns the boundary measure of the Node (sum of the boundaryMeasures of its incident Edges)
		inline lgl::F32 boundaryMeasure() const {
			return m_boundary_measure;
		}

	protected:
		lgl::F32 m_boundary_measure;
	};
	//===========================================================
	// EO C_BoundaryMeasure_NodeAttribute
	//===========================================================

	//===========================================================
	/// Minimal graph edge attribution when using the energy class C_BoundaryMeasure
	class C_BoundaryMeasure_EdgeAttribute: public ScaleClimbingEdgeAttribute {
	public:
		/// ctor
		C_BoundaryMeasure_EdgeAttribute(lgl::F32 boundary_measure = 0) :
				m_boundary_measure(boundary_measure) {
		}

		/// returns a ref on the boundary measure of the Edge
		inline lgl::F32& boundaryMeasure() {
			return m_boundary_measure;
		}
		/// returns the boundary measure of the Edge
		inline lgl::F32 boundaryMeasure() const {
			return m_boundary_measure;
		}

	protected:
		lgl::F32 m_boundary_measure;
	};
	//===========================================================
	// EO C_BoundaryMeasure_EdgeAttribute
	//===========================================================

	//===========================================================
	///  Complexity Energy (C) based on the measure of the boundary of a region.
	///
	///  Edges and Nodes attribute boundaryMeasure() must be initialized by the user.
	class C_BoundaryMeasure_User: public virtual ImageScaleClimbingUser {
	public:
		C_BoundaryMeasure_User() {
		}

		//===========================================================
		// Energy evaluation callbacks
		//===========================================================
		/// Returns the simplicity energy value of a Node
		virtual lgl::F32 SCCB_C_eval(Node n) {
			C_BoundaryMeasure_NodeAttribute* a = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(n.getAttribute());
			return a->boundaryMeasure();
		}
		/// Returns the simplicity energy value of an Edge, that is of the union of its two end Nodes
		virtual lgl::F32 SCCB_C_eval(Edge e) {
			C_BoundaryMeasure_NodeAttribute* fa = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(e.getInitialNode().getAttribute());
			C_BoundaryMeasure_NodeAttribute* ta = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(e.getFinalNode().getAttribute());
			C_BoundaryMeasure_EdgeAttribute* ea = dynamic_cast<C_BoundaryMeasure_EdgeAttribute*>(e.getAttribute());
			return fa->boundaryMeasure() + ta->boundaryMeasure() - 2 * ea->boundaryMeasure();
		}
		//===========================================================

		//===========================================================
		/// Called at the insertion of an interpixel edge element (edgel) in an Edge (called in buildGraph()).
		///
		/// \param left is the ImageSite which is on the left of the edgel and which corresponds to the Node e.from()
		/// \param right is the ImageSite which is on the right of the edgel and which corresponds to the Node e.to()
		/// The user can at this point fill the Edge's attribute with custom values
		/// Here the e.attribute()->boundaryMeasure() is simply incremented
		virtual void SCCB_C_addEdgelToEdge(Edge e, const lgl::ImageSite& left,
				const lgl::ImageSite& right) {
			C_BoundaryMeasure_EdgeAttribute* ea = dynamic_cast<C_BoundaryMeasure_EdgeAttribute*>(e.getAttribute());
			C_BoundaryMeasure_NodeAttribute* fa = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(e.getInitialNode().getAttribute());
			C_BoundaryMeasure_NodeAttribute* ta = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(e.getFinalNode().getAttribute());
			// Increments the boundary length
			ea->boundaryMeasure()++;fa
			->boundaryMeasure()++;ta
			->boundaryMeasure()++;}
			//===========================================================

			//===========================================================
			// Graph management / energy update related callback
			//===========================================================
			// Complexity Energy (C) updates dedicated callbacks
			//===========================================================
			/// Called before the contraction of Edge e (beginning of ScaleClimbing::contract(e)).
			///
			/// Its Node from() will survive and to() will die.
			/// The user can hence update the attributes of from() in order to
			/// reflect its merging with to().
		virtual void SCCB_C_onEdgeContraction(Edge e) {
			C_BoundaryMeasure_NodeAttribute* fa = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(e.getInitialNode().getAttribute());
			C_BoundaryMeasure_NodeAttribute* ta = dynamic_cast<C_BoundaryMeasure_NodeAttribute*>(e.getFinalNode().getAttribute());
			C_BoundaryMeasure_EdgeAttribute* ea = dynamic_cast<C_BoundaryMeasure_EdgeAttribute*>(e.getAttribute());
			fa->boundaryMeasure() += ta->boundaryMeasure() - 2 * ea->boundaryMeasure();
		}
		/// CB when Edge "merged" is merged to Edge "to" ("merged" will be deleted just after)
		virtual bool SCCB_C_onEdgeMerging(Edge merged, Edge to) {
			C_BoundaryMeasure_EdgeAttribute* ma = dynamic_cast<C_BoundaryMeasure_EdgeAttribute*>(merged.getAttribute());
			C_BoundaryMeasure_EdgeAttribute* ta = dynamic_cast<C_BoundaryMeasure_EdgeAttribute*>(to.getAttribute());
			ta->boundaryMeasure() += ma->boundaryMeasure();
			return true;
		}
		//===========================================================

	};
//===========================================================
// EO C_BoundaryMeasure_User
//===========================================================

}


//===========================================================
// EOF
//===========================================================
#endif

