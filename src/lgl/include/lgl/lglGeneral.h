/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
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
/*! \file 
 \brief General definitions of the library : types, macros...
 */
#ifndef __lglGeneral_h__
#define __lglGeneral_h__
//===========================================================
// namespace	: lgl
//  General definitions of the library : types, macros...
//===========================================================
// author		: Laurent Guigues
// history		: 
//	11/07/04		creation
//===========================================================
// Basic inclusions
// Streams
#if (__GNUC__) && (__GNUC__ <3)
#include <strstream.h>
#else
#include <sstream>
#endif
#include <iostream>
#include <fstream>
// Basic math functions

//===========================================================

//============ Windows specifics ============================
#if defined(_WIN32) || defined(WIN32) || defined(__NT__)
#define __CONSOLE__
#define _G_HAVE_BOOL
#ifndef __WINDOWS__
#define __WINDOWS__
#endif //  __WINDOWS__
#endif // defined(_WIN32) || defined(WIN32) || defined(__NT__)
//============ EO Windows specifics ==========================

//===========================================================
#undef M_PI

// if lGl uses wxWidgets : includes headers
// For compilers that support precompilation, includes "wx/wx.h".
// #include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP

//===========================================================

//===========================================================
// Strings
#include "lgl/lglString.h"
// Basic types handler
#include "lgl/lglType.h"
//===========================================================

/// lGl : A C++ library needed by the multiscale image segmentation library SxS
namespace lgl {

	extern bool lglLOG_enabled;
	void enable_lglLOG(bool enable);

	//===========================================================
	// LOG / WARNING / ERROR
	//===========================================================

#define lglLOG(A) { if (lgl::lglLOG_enabled) { std::cout << A; } }
#define ENDL std::endl
#define FLUSH std::cout.flush();
#define lglERROR(S) { lglLOG( ":-( lglError   : " << S << " - program halted - " << ENDL); FLUSH; exit(1); }
#define lglWARNING(S) { lglLOG ( ":-| lglWarning : " << S << ENDL); FLUSH; }

//#endif 
	//===========================================================
	// ASSERT
	//===========================================================
#ifdef _DEBUG
#define lglASSERT(A) { if (!(A)) lglERROR( "lglAssertion failed (FILE:" << __FILE__	<< " LINE:" << __LINE__ << ")"); }
#else
#define lglASSERT(A) 
#endif
	//===========================================================

	//===========================================================
	inline void PressEnter() {
		lglLOG("-------- (press enter) --------" << ENDL);
		std::cin.get();
	}
	//===========================================================

	//===========================================================
	// SYSTEM SPECIFICS (bill's shit)
	//===========================================================
#if defined(_WIN32) || defined(WIN32) || defined(__NT__)
//#define __CONSOLE__
//#define _G_HAVE_BOOL
#ifndef __WINDOWS__
#define __WINDOWS__
#endif //  __WINDOWS__
#endif // defined(_WIN32) || defined(WIN32) || defined(__NT__)
#ifdef __WINDOWS__
// silly warnings
#pragma warning(disable:4786)
#pragma warning(disable:4660)
#pragma warning(disable:4661)
// forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable:4800)
#endif

	//===========================================================
	// non standard friendship declaration in bill's system
	// non standard typename use in bill's system
	//===========================================================
#ifdef __WINDOWS__
#ifndef FRIEND_CLASS
#define FRIEND_CLASS friend
#endif // FRIEND_CLASS
#ifndef TYPENAME
#define TYPENAME
#endif // TYPENAME
#else
#define FRIEND_CLASS friend class
#define TYPENAME typename
#endif
#endif
	//===========================================================
	// non standard nested template methods declaration in bill's system
	//===========================================================
#ifdef __WINDOWS__
#define NESTED_TEMPLATE(T) 
#else 
#define NESTED_TEMPLATE(T) <T>
#endif

	//===========================================================
#ifdef __WINDOWS__
#define PARAM_IFSTREAM_TEXT std::ios::in 
#define PARAM_IFSTREAM_BINARY std::ios::in | std::ios::binary
#define PARAM_OFSTREAM_TEXT std::ios::out
#define PARAM_OFSTREAM_BINARY std::ios::out | std::ios::binary
#define PARAM_OFSTREAM_BINARY_INOUT std::ios::in | std::ios::out | std::ios::binary
#else
#define PARAM_IFSTREAM_TEXT std::ios::in 
#define PARAM_IFSTREAM_BINARY std::ios::in
#define PARAM_OFSTREAM_TEXT std::ios::out
#define PARAM_OFSTREAM_BINARY std::ios::out
#define PARAM_OFSTREAM_BINARY_INOUT std::ios::in | std::ios::out
#endif

	// General macros

#ifndef ABS
#define ABS(A) ((A)>0?(A):-(A))
#endif

#ifndef SQR
				template<class T>
				inline T SQR(T a) {
					return a * a;
				}
#endif

//===========================================================

}

//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
