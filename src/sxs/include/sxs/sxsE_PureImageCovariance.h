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
#ifndef __sxsE_PureImageCovariance_h__
#define __sxsE_PureImageCovariance_h__
//===========================================================
// namespace	: sxs
//
/*! \file 
 \brief Classes implementing a pure covariance segmentation model (constant complexity/covariance deviations)
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	30/07/04		creation
//===========================================================
#include "sxs/sxsC_Constant.h"
#include "sxs/sxsD_ImageCovariance.h"


namespace sxs {

	//=================================================================================
	/// \brief Graph nodes attributes for the pure image covariance energy
	class E_PureImageCovariance_NodeAttribute: public D_ImageCovariance_NodeAttribute {
	};
	//==================================================================================

	//=================================================================================
	/// \brief Graph edges attributes for the pure image covariance energy
	class E_PureImageCovariance_EdgeAttribute: public D_ImageCovariance_EdgeAttribute {
	};
	//=================================================================================

	//=================================================================================
	/// \brief ScaleClimbingUser for the pure image covariance energy
	///
	class E_PureImageCovariance_User: public C_Constant_User, public D_ImageCovariance_User {

	public:

		/// \brief Allocates a new MumfordShahNodeAttribute.
		virtual ScaleClimbingNodeAttribute* SCCB_Q_newNodeAttribute() {
			E_PureImageCovariance_NodeAttribute* a = new E_PureImageCovariance_NodeAttribute;
			// Explicit upcast is needed because of multiple path inheritence
			C_Constant_NodeAttribute* aa = a;
			ScaleClimbingNodeAttribute* aaa = aa;
			return aaa;
		}

		/// \brief Allocates a new MumfordShahEdgeAttribute.
		virtual ScaleClimbingEdgeAttribute* SCCB_Q_newEdgeAttribute() {
			E_PureImageCovariance_EdgeAttribute* a = new E_PureImageCovariance_EdgeAttribute;
			// Explicit upcast is needed because of multiple path inheritence
			C_Constant_EdgeAttribute* aa = a;
			ScaleClimbingEdgeAttribute* aaa = aa;
			return aaa;
		}

	};
//=================================================================================

}
//===========================================================
// EO namespace sxs
//===========================================================

#endif

