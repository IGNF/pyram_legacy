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
 \brief 2D points
 **/
#ifndef __lglPoint2D_h__
#define __lglPoint2D_h__
//===========================================================
// namespace	: lgl
// classe		: Point2D<T>
// 2D points
//===========================================================
// author		: Laurent Guigues
// history		: 
//	28/07/04	: creation
//===========================================================
#include <iostream>
#include "lgl/lglGeneral.h"

namespace lgl {

	//==========================================================================================
	/// Points of dimension 2
	template<class T> class Point2D {
	public:
		/// Builds the point (v,v)
		Point2D(T v = 0) {
			m_v[0] = m_v[1] = v;
		}
		/// Builds the point (x,y)
		Point2D(T v0, T v1) {
			m_v[0] = v0;
			m_v[1] = v1;
		}
		/// Copy ctor
		Point2D(const Point2D& p) {
			m_v[0] = p.m_v[0];
			m_v[1] = p.m_v[1];
		}
		/// Operator =
		inline Point2D& operator =(const Point2D& p) {
			m_v[0] = p.m_v[0];
			m_v[1] = p.m_v[1];
			return *this;
		}
		/// Operator = with a value : sets all coordinates of the Point to v.
		inline Point2D& operator =(T v) {
			m_v[0] = m_v[1] = v;
			return *this;
		}
		/// Operator ==
		inline BOOL operator ==(const Point2D& p) const {
			return ((m_v[0] == p.m_v[0]) && (m_v[1] == p.m_v[1]));
		}
		/// Operator !=
		inline BOOL operator !=(const Point2D& p) const {
			return ((m_v[0] != p.m_v[0]) || (m_v[1] != p.m_v[1]));
		}
		/// Operator < : tests wether all coordinates of *this are inferior to those of p.
		inline BOOL operator <(const Point2D& p) const {
			return ((m_v[0] < p.m_v[1]) && (m_v[1] < p.m_v[1]));
		}
		/// Operator < : tests wether all coordinates of *this are inferior or equal to those of p.
		inline BOOL operator <=(const Point2D& p) const {
			return ((m_v[0] <= p.m_v[1]) && (m_v[1] <= p.m_v[1]));
		}
		/// Operator < : tests wether all coordinates of *this are superior to those of p.
		inline BOOL operator >(const Point2D& p) const {
			return ((m_v[0] > p.m_v[1]) && (m_v[1] > p.m_v[1]));
		}
		/// Operator < : tests wether all coordinates of *this are superior or equal to those of p.
		inline BOOL operator >=(const Point2D& p) const {
			return ((m_v[0] >= p.m_v[1]) && (m_v[1] >= p.m_v[1]));
		}

		/// returns the sum of the two points (sum of coordinates)
		inline Point2D<T> operator +(const Point2D<T>& p) const {
			return Point2D<T>(m_v[0] + p.m_v[0], m_v[1] + p.m_v[1]);
		}
		/// adds the point p to this (sum of coordinates)
		inline void operator +=(const Point2D<T>& p) {
			m_v[0] += p.m_v[0];
			m_v[1] += p.m_v[1];
		}
		/// returns the difference of the two points (sum of coordinates)
		inline Point2D<T> operator -(const Point2D<T>& p) const {
			return Point2D<T>(m_v[0] - p.m_v[0], m_v[1] - p.m_v[1]);
		}
		/// substracts the point p to this (substraction of coordinates)
		inline void operator -=(const Point2D<T>& p) {
			m_v[0] -= p.m_v[0];
			m_v[1] -= p.m_v[1];
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
		/// coordinates // ICI Il y avait 2 mais j'ai mis 3 pour gérer un warning.
		T m_v[3];
	};
	//==========================================================================================
	// EO class Point2D
	//==========================================================================================

	template<class T>
	std::ostream& operator <<(std::ostream& s, const Point2D<T>& p) {
		s << "(" << p(0) << "," << p(1) << ")";
		return s;
	}

	/// Integer point
	typedef Point2D<I32> IPoint2D;
	/// Float point
	typedef Point2D<F32> FPoint2D;

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

