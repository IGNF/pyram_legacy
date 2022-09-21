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
 \brief Matrices
 */
#ifndef __lglMatrix_h__
#define __lglMatrix_h__
//===========================================================
// namespace	: lgl
// class		: Matrix
// Matrices based on Array2D
//===========================================================
// author		: Laurent Guigues
// history		: 
//	28/07/04	: creation
//===========================================================
#include "lgl/lglArray2D.h"

namespace lgl {

	//===========================================================
	/// Matrix class based on Array2D
	template<class T>
	class Matrix: public Array2D<T> {
	public:

		typedef Vector<T> _Vector;
		typedef typename Array2D<T>::iterator iterator;
		typedef typename Array2D<T>::const_iterator const_iterator;

		Matrix() {
		}
		Matrix(int nl, int nc) :
				Array2D<T>(nl, nc) {
		}
		Matrix(const IPoint4D& p) :
				Array2D<T>(p(0), p(1)) {
		}
		Matrix(int nl, int nc, T v) :
				Array2D<T>(nl, nc, v) {
		}
		Matrix(Matrix<T> const & t);
		virtual ~Matrix() {
		}

		Matrix & operator =(T v);
		Matrix operator *(T d) const;
		Matrix operator /(T d) const;
		void operator *=(T d);
		void operator /=(T d);

		_Vector operator *(const _Vector& v) const;

		Matrix & operator =(Matrix const & m);
		Matrix operator *(Matrix const & m) const;
		Matrix operator +(Matrix const & m) const;
		Matrix operator -(Matrix const & m) const;
		Matrix & operator +=(Matrix const & m);
		Matrix & operator -=(Matrix const & m);

		Matrix transpose() const;

		inline BOOL isIn(int i, int j);
		inline BOOL isIn(const IPoint4D& p);

		//void inverse(double epsilon=1e-10);
		//Matrix getInverse(double epsilon=1e-10) const;

		/// Eigen vectors/values computation based on Jacobi rotations (adapted from numerical recipes in C).
		/// eigenvectors contains the eigen vectors arranged in collumns.
		/// eigenvalues contains the eigen values.
		void eigen_decomposition(Matrix& eigenvectors, _Vector& eigenvalues) const;

	};
//===========================================================
// EO class Matrix
//===========================================================

// template codes inclusion
#include "lgl/lglMatrix_code.h"

}
//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif

