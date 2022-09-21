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
#ifndef __sxsGeneral_h__
#define __sxsGeneral_h__
//===========================================================
// namespace	: sxs
/*! \file 
 \brief General definitions of the library : types, macros...
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	12/07/04		creation
//===========================================================
#include "lgl/lglGeneral.h"

#include "sxs/sxsVersion.h"

#ifdef __WINDOWS__
// dominance inheritance warning 
#pragma warning(disable:4250)
// performance warning for bool cast
#pragma warning(disable:4800)
#endif

//==================================================================================
// LOG MESSAGES
//==================================================================================
#if defined(__SXS_COMPILE_LOG__)
//==================================================================================
/// Displays a log message
#define sxsLOG(A) lglLOG ( "[sxs]" << A ) 
#else
#define sxsLOG(A) 
//==================================================================================
#endif
//==================================================================================

//==================================================================================
// WARNINGS/ERRORS
//==================================================================================
#if defined(__SXS_COMPILE_ERROR__)
//==================================================================================
/// Displays a warning message
#define sxsWARNING(A) lglWARNING ( "[sxs]" << A ) 
/// Displays an error message
#define sxsERROR(A) lglERROR ( "[sxs]" << A ) 
//==================================================================================
#else
//==================================================================================
#define sxsERROR(A) 
#define sxsWARNING(A) 
//==================================================================================
#endif
//==================================================================================

namespace sxs {

	/// Type of index for the Sets of a ScaleSets
	typedef lgl::I32 IndexType;

}

//===========================================================
// EOF
//===========================================================
#endif
