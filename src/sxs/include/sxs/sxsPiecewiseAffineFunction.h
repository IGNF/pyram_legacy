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
#ifndef __sxsPiecewiseAffineFunction_h__
#define __sxsPiecewiseAffineFunction_h__
//===========================================================
// namespace	: sxs
// classes		: AffinePiece, PiecewiseAffineFunction
/*! \file 
 \brief Piecewise affine functions used to model the partial energy functions of a hierarchy.
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	12/07/04		creation
//===========================================================
#undef M_PI
#include "sxs/sxsGeneral.h"
#include <list>

namespace sxs {

	//===========================================================
	/// An affine piece defined by its origin and slope.
	class AffinePiece {
	protected:
		/// x,y origin and slope
		lgl::F32 m_x, m_y, m_s;
	public:
		AffinePiece(lgl::F32 x = 0, lgl::F32 y = 0, lgl::F32 s = 0) :
				m_x(x), m_y(y), m_s(s) {
		}

		inline lgl::F32& x() {
			return m_x;
		}
		inline lgl::F32& y() {
			return m_y;
		}
		inline lgl::F32& s() {
			return m_s;
		}
		inline const lgl::F32& x() const {
			return m_x;
		}
		inline const lgl::F32& y() const {
			return m_y;
		}
		inline const lgl::F32& s() const {
			return m_s;
		}
	};
	//===========================================================
	// EO class AffinePiece
	//===========================================================

	//===========================================================
	/// Piecewise Affine Functions (PAF)
	class PiecewiseAffineFunction: public std::list<AffinePiece> {
	public:
		//===========================================================
		/// Maximum number of pieces in a function.
		/// Used to optimize the functional dynamic programming procedure by
		/// partially computing sums of PAFs
		static lgl::UI32& maxNboPieces() {
			static lgl::UI32 n = 32;
			return n;
		}
		//===========================================================

		//===========================================================
		/// default ctor
		PiecewiseAffineFunction() {
		}
		/// copy ctor
		PiecewiseAffineFunction(const PiecewiseAffineFunction& a) :
				std::list<AffinePiece>(a) {
			;
		}
		/// ctor with a piece
		PiecewiseAffineFunction(const AffinePiece& a) {
			push_back(a);
		}
		//===========================================================

		//===========================================================
		/// returns the function's value at x
		lgl::F32 operator ()(lgl::F32 x) const;
		//===========================================================

		//===========================================================
		/// addition of another PAF. Addition is trunctated to
		/// the last maxNboPieces() of the result
		void operator +=(const PiecewiseAffineFunction& f);
		//===========================================================

		//===========================================================
		/// infimum with an AffinePiece. Returns the intersection point (scale of appearance).
		///
		/// Hypotheses : *this is continous, deacreasing and concave and the
		/// slope of p is smallest than the slope of the last piece of *this.
		/// Returns FALSE if the last hypothesis is violated.
		lgl::BOOL infimum(const AffinePiece& p, lgl::F32& intersection);
		//===========================================================

		//===========================================================
		/// read from a binary ifstream
		bool readBinary(std::ifstream& f);
		/// write in a binary ifstream
		bool writeBinary(std::ofstream& f);
		//===========================================================

	};
	//===========================================================
	// EO PiecewiseAffineFunction
	//===========================================================

	//===========================================================
	inline std::ostream& operator <<(std::ostream& s, const AffinePiece& a) {
		s << "(" << a.x() << "," << a.y() << ";" << a.s() << ")";
		return s;
	}
	//===========================================================

	//===========================================================
	inline std::ostream& operator <<(std::ostream& s, const PiecewiseAffineFunction& a) {
		s << "[";
		for (PiecewiseAffineFunction::const_iterator i = a.begin(); i != a.end(); ++i) {
			s << (*i);
		}
		s << "]";
		return s;
	}
//===========================================================

}
//===========================================================
// EOF
//===========================================================
#endif

