/* 

 lGl : A C++ library needed by the multiscale image segmentation library SxS
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
/*! \file 
 \brief Abstract (mathematical) functions.
 */
#ifndef __lglFunction_h__
#define __lglFunction_h__
//===========================================================
// namespace	: lgl
//  Abstract (mathematical) functions  
//===========================================================
// author		: Laurent Guigues
// history		: 
//	12/07/04		creation
//===========================================================
#include "lgl/lglGeneral.h"

namespace lgl {

	//===========================================================
	template<class F, class T> class UnaryFunction {
	public:
		virtual T operator ()(const F&) const = 0;
		virtual ~UnaryFunction() {
		}
	};
	//===========================================================

	//===========================================================
	template<class F1, class F2, class T> class BinaryFunction {
	public:
		virtual T operator ()(const F1&, const F2&) const = 0;
		virtual ~BinaryFunction() {
		}
	};
	//===========================================================

	//===========================================================
	template<class F1, class F2, class F3, class T> class TernaryFunction {
	public:
		virtual T operator ()(const F1&, const F2&, const F3&) = 0;
		virtual ~TernaryFunction() {
		}
	};
	//===========================================================

	//===========================================================
	/// Binary boolean function returning TRUE iff x<y
	template<class F> class Less: public BinaryFunction<F, F, BOOL> {
	public:
		static const Less<F>& staticInstance() {
			static Less<F> l;
			return l;
		}
		BOOL operator ()(const F& a, const F& b) const {
			return (a < b);
		}
	};
//===========================================================

}

//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
