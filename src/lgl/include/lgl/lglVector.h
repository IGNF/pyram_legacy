/* 

lGl : A C++ library needed by the multiscale image segmentation library SxS
2004 Laurent Guigues (laurent.guigues@ign.fr)

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
/**	\file	
 \brief Mathematical vectors based on std::vector
 **/
#ifndef __lglVector_h__
#define __lglVector_h__
//===========================================================
// namespace	: lgl
// classe		: Vector<T>
// Mathematical vectors based on std::vector
//===========================================================
// author		: Laurent Guigues
// history		: 
//	28/07/04	: creation
//===========================================================
#include <vector>
#include "lgl/lglGeneral.h"

namespace lgl {

	//==========================================================================================
	/// \brief Mathematical vectors based on std::vector
	template<class T> class Vector: public std::vector<T> {
	public:
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;

		Vector(int n = 0, const T& t = 0) :
				std::vector<T>(n) {
			for (iterator i = this->begin(); i != this->end(); ++i)
				(*i) = t;
		}

		inline T& operator()(I32 i) {
			return this->operator[](i);
		}
		inline const T& operator()(I32 i) const {
			return this->operator[](i);
		}

		Vector<T> operator+(const Vector& v) {
			Vector<T> o((int) this->size());
			const_iterator i, j;
			iterator k;
			for (i = this->begin(), j = v.begin(), k = o.begin(); i != this->end(); ++i, ++j, ++k)
				(*k) = (*i) + (*j);
			return o;
		}
		void operator+=(const Vector& v) {
			iterator i;
			const_iterator j;
			for (i = this->begin(), j = v.begin(); i != this->end(); ++i, ++j)
				(*i) += (*j);
		}
	};
	//==========================================================================================
	// EO class Vector
	//==========================================================================================

	typedef Vector<I32> IVector;
	typedef Vector<F32> FVector;

	template<class T>
	std::ostream& operator<<(std::ostream& s, const Vector<T>& v) {
		s << "( ";
		for (typename Vector<T>::const_iterator i = v.begin(); i != v.end(); ++i)
			s << (*i) << " ";
		s << ")";
		return s;
	}

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

