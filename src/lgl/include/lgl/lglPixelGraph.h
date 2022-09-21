#ifndef __lglPixelGraph_h__
#define __lglPixelGraph_h__
//===========================================================
// namespace	: lgl
// classe		: PixelGraph
//  Graphs whose nodes are the pixels of a 2D or a 3D image and 
//  whose edges are a subset of the 8 (2D) or 22 (3D) neigbourhood relationship between pixels
//===========================================================
// author		: Laurent Guigues
// history		: 
//	29/07/04	: creation
//===========================================================
#include "lgl/lglGeneral.h"
#include "lgl/lglImageRam.h"

//#include <ostream>

namespace lgl {

	class PixelGraph {
	public:

		void symetrize();
		void connected_components(ImageRam& cc, int& nbcc);

	protected:
		ImageRam* m_i;
	};

}
#endif
