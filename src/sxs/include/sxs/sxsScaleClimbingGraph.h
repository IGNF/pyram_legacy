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
#ifndef __sxsScaleClimbingGraph_h__
#define __sxsScaleClimbingGraph_h__
//===========================================================
// namespace	: sxs
// class		: ScaleClimbingNodeAttribute, ScaleClimbingEdgeAttribute, ScaleClimbingGraph
/*! \file 
 \brief Defines the Graph class used in the ScaleClimbing algorithm.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	12/07/04		creation
//===========================================================
//===========================================================
#include <limits>
#include "lgl/lglGraph.h"
#ifdef __SXS_CLIMBING_TESTS__
#include "lgl/lglGraphTesting.h"
#endif

#include "sxs/sxsGeneral.h"
#include "sxs/sxsPiecewiseAffineFunction.h"


namespace sxs {

	//===========================================================
	/// \brief Minimal attribute for the Nodes of a Graph used in the ScaleClimbing algorithm
	class ScaleClimbingNodeAttribute {
	public:
		ScaleClimbingNodeAttribute() :
				m_energy(NULL), m_pset(NULL) {
		}
		virtual ~ScaleClimbingNodeAttribute() {
			if (m_energy != NULL)
				delete m_energy;
		}
		/// Returns a reference on the pointer on the ScaleSets' Set to which it currently corresponds.
		/// Useful for building the sons' list of a new set
		void*& sxspSet() {
			return m_pset;
		}

		//===========================================================
		/// Returns a const ref on the partial energy function of the Node
		const PiecewiseAffineFunction& partialEnergy() const {
			return *m_energy;
		}
		/// Returns a ref on the pointer on the partial energy function of the Node
		PiecewiseAffineFunction*& ppartialEnergy() {
			return m_energy;
		}
		//===========================================================

	protected:
		//===========================================================
		/// Pointer on the partial energy function of the Node.
		/// It is pointed for fast transfer between Nodes and Edges at contraction.
		PiecewiseAffineFunction *m_energy;
		/// Pointer on the ScaleSets' Set to which it currently corresponds
		void* m_pset;
		//===========================================================
	};
	//===========================================================
	// EO ScaleClimbingNodeAttribute
	//===========================================================

	//===========================================================
	/// \brief Minimal attribute for the Edges of the Graph used in the ScaleClimbing algorithm
	class ScaleClimbingEdgeAttribute {
	public:
		ScaleClimbingEdgeAttribute() :
				m_energy(NULL), m_scale(0), m_heap_index(-1) {
		}
		virtual ~ScaleClimbingEdgeAttribute() {
			if (m_energy != NULL)
				delete m_energy;
		}

		//===========================================================
		lgl::I32& heapIndex() {
			return m_heap_index;
		}
		const lgl::I32& heapIndex() const {
			return m_heap_index;
		}
		//===========================================================

		//===========================================================
		PiecewiseAffineFunction& partialEnergy() {
			return *m_energy;
		}
		const PiecewiseAffineFunction& partialEnergy() const {
			return *m_energy;
		}
		PiecewiseAffineFunction*& ppartialEnergy() {
			return m_energy;
		}
		//===========================================================

		//===========================================================
		lgl::F32& scaleOfAppearance() {
			return m_scale;
		}
		//===========================================================

	protected:
		//===========================================================
		/// Pointer on the partial Energy of the union of the end nodes of the edge
		PiecewiseAffineFunction *m_energy;
		/// Scale of appearance of the union
		lgl::F32 m_scale;
		/// Index in the heap
		lgl::I32 m_heap_index;
		//===========================================================
	};
	//===========================================================
	// EO ScaleClimbingEdgeAttribute
	//===========================================================

	//===========================================================
	/// \brief Type of Graph used in the ScaleClimbing algorithm
	typedef lgl::Graph<ScaleClimbingNodeAttribute*, ScaleClimbingEdgeAttribute*> ScaleClimbingGraph;
	/// \brief Type of Node of the graphs used in the ScaleClimbing algorithm
	typedef lgl::Graph<ScaleClimbingNodeAttribute*, ScaleClimbingEdgeAttribute*>::Node ScaleClimbingNode;
	/// \brief Type of Edge of the graphs used in the ScaleClimbing algorithm
	typedef lgl::Graph<ScaleClimbingNodeAttribute*, ScaleClimbingEdgeAttribute*>::Edge ScaleClimbingEdge;
//===========================================================

}
//===========================================================
// EOF
//===========================================================
#endif
