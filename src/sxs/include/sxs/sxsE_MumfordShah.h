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
#ifndef __sxsE_MumfordShah_h__
#define __sxsE_MumfordShah_h__
//===========================================================
// namespace	: sxs
//
/*! \file 
 \brief Classes implementing the Mumford-Shah energy for piece-wise constant image modeling
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	30/07/04		creation
//===========================================================
#include "sxs/sxsC_BoundaryMeasure.h"
#include "sxs/sxsD_ImageCovariance.h"


namespace sxs {

	//===========================================================
	// Mumford-Shah energy associated with piece-wise constant image modelling
	//===========================================================

	//=================================================================================
	/// \brief Graph nodes attributes for the Mumford-Shah energy
	class E_MumfordShah_NodeAttribute: public C_BoundaryMeasure_NodeAttribute, public D_ImageCovariance_NodeAttribute {
	};
	//==================================================================================

	//=================================================================================
	/// \brief Graph edges attributes for the Mumford-Shah energy
	class E_MumfordShah_EdgeAttribute: public C_BoundaryMeasure_EdgeAttribute, public D_ImageCovariance_EdgeAttribute {
	};
	//=================================================================================

	//=================================================================================
	/// \brief ScaleClimbingUser for the Mumford-Shah energy
	///
	class E_MumfordShah_User: public C_BoundaryMeasure_User, public D_ImageCovariance_User {

	public:

		/// \brief Allocates a new MumfordShahNodeAttribute.
		virtual ScaleClimbingNodeAttribute* SCCB_Q_newNodeAttribute() {
			E_MumfordShah_NodeAttribute* a = new E_MumfordShah_NodeAttribute;
			// Explicit upcast is needed because of multiple path inheritence
			C_BoundaryMeasure_NodeAttribute* aa = a;
			ScaleClimbingNodeAttribute* aaa = aa;
			return aaa;
		}

		/// \brief Allocates a new MumfordShahEdgeAttribute.
		virtual ScaleClimbingEdgeAttribute* SCCB_Q_newEdgeAttribute() {
			E_MumfordShah_EdgeAttribute* a = new E_MumfordShah_EdgeAttribute;
			// Explicit upcast is needed because of multiple path inheritence
			C_BoundaryMeasure_EdgeAttribute* aa = a;
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

