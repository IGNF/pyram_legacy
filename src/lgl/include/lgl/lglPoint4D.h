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
/**	\file	
 \brief General points of 0 to 4 dimension
 **/
#ifndef __lglPoint4D_h__
#define __lglPoint4D_h__
//===========================================================
// namespace	: lgl
// classe		: Point4D<T>
// General points of 0 to 4 dimension
//===========================================================
// author		: Laurent Guigues
// history		: 
//	28/07/04	: creation
//===========================================================
#include <iostream>
#include "lgl/lglGeneral.h"

namespace lgl {

	//==========================================================================================
	/// Type for storing objects' dimension (points,arrays)
	typedef UI8 DimensionType;
	//==========================================================================================
	/// The Dimension class definition : useless at the moment...
	// LGL_DEFINE_BASIC_TYPE_EMBEDDING_CLASS ( Dimension, DimensionType );
	//==========================================================================================

	//==========================================================================================
	/// Points of dimension 0 to 4
	template<class T>
	class Point4D {
	public:
		/// Builds a null dimension point
		Point4D() :
				m_dim(0) {
			memset(m_v, 0, 4 * sizeof(T));
		}
		/// Builds a point of dimension 1. // ICI Il y avait m_v[4] sur le dernier, surement un bug, j'ai mis 3 pour gérer un warning.
		Point4D(T x) :
				m_dim(1) {
			m_v[0] = x;
			m_v[1] = m_v[2] = m_v[3] = 0;
		}
		/// Builds a point of dimension 2
		Point4D(T x, T y) :
				m_dim(2) {
			m_v[0] = x;
			m_v[1] = y;
			m_v[2] = m_v[3] = 0;
		}
		/// Builds a point of dimension 3
		Point4D(T x, T y, T z) :
				m_dim(3) {
			m_v[0] = x;
			m_v[1] = y;
			m_v[2] = z;
			m_v[3] = 0;
		}
		/// Builds a point of dimension 3
		Point4D(T x, T y, T z, T t) :
				m_dim(4) {
			m_v[0] = x;
			m_v[1] = y;
			m_v[2] = z;
			m_v[3] = t;
		}
		/// Copy ctor
		Point4D(const Point4D& p) :
				m_dim(p.m_dim) {
			memcpy(m_v, p.m_v, 4 * sizeof(T));
		}
		/// Operator =
		Point4D& operator =(const Point4D& p) {
			m_dim = p.m_dim;
			memcpy(m_v, p.m_v, 4 * sizeof(T));
			return *this;
		}
		/// Operator = with a value : sets all coordinates of the Point to v. The dimension is unchanged
		Point4D& operator =(T v) {
			for (int i = 0; i < m_dim; ++i)
				m_v[i] = v;
			return *this;
		}
		/// Operator ==
		BOOL operator ==(const Point4D& p) const {
			if (m_dim != p.m_dim)
				return false;
			for (int i = 0; i < m_dim; ++i)
				if (m_v[i] != p.m_v[i])
					return false;
			return true;
		}
		/// Operator !=
		BOOL operator !=(const Point4D& p) const {
			if (m_dim != p.m_dim)
				return true;
			for (int i = 0; i < m_dim; ++i)
				if (m_v[i] != p.m_v[i])
					return true;
			return false;
		}
		/// Operator < : tests wether all valid coordinates of *this are inferior to those of p. Returns false if the dimensions of the points are different.
		BOOL operator <(const Point4D& p) const {
			if (m_dim != p.m_dim)
				return false;
			for (int i = 0; i < m_dim; ++i)
				if (m_v[i] >= p.m_v[i])
					return false;
			return true;
		}
		/// Operator <=
		BOOL operator <=(const Point4D& p) const {
			if (m_dim != p.m_dim)
				return false;
			for (int i = 0; i < m_dim; ++i)
				if (m_v[i] > p.m_v[i])
					return false;
			return true;
		}
		/// Operator >
		BOOL operator >(const Point4D& p) const {
			if (m_dim != p.m_dim)
				return false;
			for (int i = 0; i < m_dim; ++i)
				if (m_v[i] <= p.m_v[i])
					return false;
			return true;
		}
		/// Operator >=
		BOOL operator >=(const Point4D& p) const {
			if (m_dim != p.m_dim)
				return false;
			for (int i = 0; i < m_dim; ++i)
				if (m_v[i] < p.m_v[i])
					return false;
			return true;
		}

		Point4D<T> operator +(const Point4D<T>& p) const {
			Point4D<T> o(*this);
			for (int i = 0; i < m_dim; ++i)
				o.m_v[i] += p.m_v[i];
			return o;
		}
		Point4D<T> operator -(const Point4D<T>& p) const {
			Point4D<T> o(*this);
			for (int i = 0; i < m_dim; ++i)
				o.m_v[i] -= p.m_v[i];
			return o;
		}

		/// Returns the dimension of the point
		inline DimensionType dimension() const {
			return m_dim;
		}
		/// Returns a ref on the the dimension of the point
		inline DimensionType& dimension() {
			return m_dim;
		}

		/// Return a ref on the ith coordinate of the Point
		inline T& operator ()(I32 i) {
			return m_v[i];
		}
		/// Return a const ref on the ith coordinate of the Point
		inline const T& operator ()(I32 i) const {
			return m_v[i];
		}

	protected:
		/// Dimension of the point
		DimensionType m_dim;
		/// Values of the point (coordinates)
		T m_v[4];
	};
	//==========================================================================================
	// EO class Point4D
	//==========================================================================================

	template<class T>
	std::ostream& operator <<(std::ostream& s, const Point4D<T>& p) {
		s << "(";
		int i = 0;
		for (; i < p.dimension() - 1; i++)
			s << p(i) << ",";
		s << p(i) << ")";
		return s;
	}

	/// Integer point
	typedef Point4D<I32> IPoint4D;
	/// Float point
	typedef Point4D<F32> FPoint4D;

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

