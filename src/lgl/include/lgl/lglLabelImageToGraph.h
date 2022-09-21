#ifndef __lglLabelImageToGraph_h__
#define __lglLabelImageToGraph_h__
//===========================================================
// namespace	: lgl
//
/// @file  Freehand methods to build a Region Adjacency Graph (RAG) 
/// from a label image (2D or 3D).
//
//===========================================================
// author		: Laurent Guigues
// history		: 
//	30/07/04		creation
//===========================================================
#include "lgl/lglGeneral.h"
#include "lgl/lglNeighborhood.h"


namespace lgl {

	/// Builds a Region Adjacency Graph (RAG) of type G based on the Neighborhood N
	/// from a Label Image of type I.
	/// The graph is a simple graph, i.e. encodes topological adjacency.
	/// The graph's nodes indices correspond to the labels of the regions.
	/// If exterior_node is set to TRUE, builds a virtual node corresponding to the exterior domain of the image
	/// and connects all boundary regions to it.
	template <class I, class G>
	LabelImageToRAG ( const I& image, Neighborhood& N, G& graph, BOOL exterior_node = FALSE );
}

//===========================================================
// EO namespace lgl
//===========================================================
/// Inclusion of the code of template methods
#include "lgl/lglLabelImageToGraph_code.h"

//===========================================================
// EOF
//===========================================================
#endif
