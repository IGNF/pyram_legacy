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
#ifndef __sxsC_Constant_h__
#define __sxsC_Constant_h__
//===========================================================
#include "sxs/sxsGeneral.h"
#include "sxs/sxsImageScaleClimbingUser.h"
//===========================================================
// namespace	: sxs
// classes		: 
//	C_Constant_User
//  Defines a constant (unit) Complexity Energy (C) on all regions
/*! \file 
 \brief ImageScaleCimbingUser implementing a constant complexity energy (C) on the regions.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	25/07/04		creation
//===========================================================


namespace sxs {

	/// Minimal graph node attribution when using the energy class C_Constant
	typedef ScaleClimbingNodeAttribute C_Constant_NodeAttribute;
	/// Minimal graph edge attribution when using the energy class C_Constant
	typedef ScaleClimbingEdgeAttribute C_Constant_EdgeAttribute;

	//===========================================================
	///  \brief Constant complexity Energy (C) on regions
	class C_Constant_User: public virtual ImageScaleClimbingUser {
	public:
//		C_Constant() {}

		//===========================================================
		// Energy evaluation callbacks
		//===========================================================
		/// Returns the simplicity energy value of a Node
		virtual lgl::F32 SCCB_C_eval(Node n) {
			return 1.;
		}
		/// Returns the simplicity energy value of an Edge, that is of the union of its two end Nodes
		virtual lgl::F32 SCCB_C_eval(Edge e) {
			return 1.;
		}
		//===========================================================

	};
//===========================================================
// EO C_Constant_User
//===========================================================

}
//===========================================================
// EO namespace sxs
//===========================================================

#endif

