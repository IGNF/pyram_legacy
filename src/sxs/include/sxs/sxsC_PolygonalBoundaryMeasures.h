/* 

 SxS : A C++ implementation of the scale climbing algorithm for multiscale image segmentation.
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
#ifndef __sxsC_PolygonalBoundaryMeasures_h__
#define __sxsC_PolygonalBoundaryMeasures_h__
//===========================================================
// namespace	: sxs
//
/*! \file 
 \brief Measures on polygonal boundaries (length,curvature,#of segments...) usefull for C_PolygonalBoundary-based energies
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	08/12/04		creation
//===========================================================
#include "sxs/sxsC_PolygonalBoundary.h"


namespace sxs {

	lgl::F32 Length(std::vector<std::vector<PolygonalBoundary> >& bnd);

	lgl::F32 Concavity(std::vector<std::vector<PolygonalBoundary> >& bnd);

	lgl::F32 NumberOfSegments(std::vector<std::vector<PolygonalBoundary> >& bnd);

//=================================================================================

}
//===========================================================
// EO namespace sxs
//===========================================================

#endif

