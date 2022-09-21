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
 \brief 2D arrays
 */

#ifndef __lglArray2D_h__
#define __lglArray2D_h__
//===========================================================
// namespace	: lgl
// classes		: Array2D<T>
//				  Array2D_iterator<T,RefT>
// Two dimensional arrays
//===========================================================
// author		: Laurent Guigues
// history		: 
//	28/07/04	: creation
//===========================================================
#include <ostream>

#include "lgl/lglGeneral.h"
#include "lgl/lglArray.h"
#include "lgl/lglFunction.h"

namespace lgl {

	template<class T> class Array2D;
	template<class T, class RefT> class Array2D_iterator;
	template<class T> std::ostream& operator <<(std::ostream&, Array2D<T> const &);

	//===========================================================
	/// Two dimensional array
	template<class T>
	class Array2D: virtual public Array, virtual public BinaryFunction<I32, I32, T> {
	public:
		/// Pointer on an array member
		typedef T* TPtr;
		/// Array members iterator
		typedef Array2D_iterator<T, T&> iterator;
		/// Array members const_iterator
		typedef Array2D_iterator<T, const T&> const_iterator;

		///@Construction/destruction
		///@{

		/// Default ctor
		Array2D() :
				m_data(NULL) {
			m_build(0, 0);
		}
		/// #lines,#col ctor
		Array2D(int nl, int nc) :
				m_data(NULL) {
			m_build(nl, nc);
		}
		/// #lines,#col ctor given by a point's first two coords (0,1)
		Array2D(const IPoint4D& p) :
				m_data(NULL) {
			m_build(p(0), p(1));
		}
		/// #lines,#col,value ctor
		Array2D(int nl, int nc, T v) :
				m_data(NULL) {
			m_build(nl, nc);
			operator =(v);
		}
		/// Copy ctor
		Array2D(Array2D<T> const & t) :
				m_data(NULL) {
			m_build(t.size());
			operator =(t);
		}
		/// Dtor
		virtual ~Array2D() {
			m_free();
		}

		///@}

		///@Size/resize
		///@{

		/// Returns the dimension of the array
		DimensionType dimension() const {
			return 2;
		}

		/// Sets size to (nl,nc);
		void resize(int nl, int nc) {
			if (nl == m_size(0) && nc == m_size(1))
				return;
			m_free();
			m_build(nl, nc);
		}
		/// Sets size to (p(0),p(1))
		void resize(const IPoint4D& p) {
			resize(p(0), p(1));
		}

		///@}

		///@Data access
		///@{

		/// Value access read/write
		inline T& operator ()(const int& l, const int& c) {
			lglASSERT( l>=0 && c>=0 && l<m_size(0) && c<m_size(1) );
			return m_data[l][c];
		}
		/// Value access read only
		inline T operator ()(const int& l, const int& c) const {
			lglASSERT( l>=0 && c>=0 && l<m_size(0) && c<m_size(1) );
			return m_data[l][c];
		}
		/// Value access read/write
		inline T & operator ()(const IPoint4D& p) {
			lglASSERT( p(0)>=0 && p(1)>=0 && p(0)<m_size(0) && p(1)<m_size(1) );
			return m_data[p(0)][p(1)];
		}
		/// Value access read only
		inline T operator ()(const IPoint4D& p) const {
			lglASSERT( p(0)>=0 && p(1)>=0 && p(0)<m_size(0) && p(1)<m_size(1) );
			return m_data[p(0)][p(1)];
		}

		/// Subscripting: pointer to row i
		inline T* operator[](const int i) {
			return m_data[i];
		}

		///@}

		///@Assignment
		///@{

		/// Assigns value v to the array's cells
		Array2D<T> & operator =(T v) {
			iterator it = begin(), fin = end();
			for (; it != fin; ++it)
				*it = v;
			return (*this);
		}
		/// Sets this values to those of src
		Array2D<T> & operator =(Array2D<T> const & src) {
			resize(src.size());
			iterator it = begin(), fin = end();
			const_iterator itsrc = src.begin();
			for (; it != fin; ++it, ++itsrc)
				*it = *itsrc;
			return (*this);
		}

		///@}

		///@Iteration
		///@{

		iterator begin() {
			if (m_data == NULL)
				return iterator(NULL);
			else
				return iterator(m_data[0]);
		}

		const_iterator begin() const {
			if (m_data == NULL)
				return const_iterator(NULL);
			else
				return const_iterator(m_data[0]);
		}

		iterator end() {
			if (m_data == NULL)
				return iterator(NULL);
			else
				return iterator(m_data[0] + m_size(0) * m_size(1));
		}
		const_iterator end() const {
			if (m_data == NULL)
				return const_iterator(NULL);
			else
				return const_iterator(m_data[0] + m_size(0) * m_size(1));
		}

		///@}
	protected:

		T ** m_data;

		void m_build(const IPoint4D& p) {
			m_build(p(0), p(1));
		}
		void m_build(int l, int c) {
			if (l <= 0 || c <= 0) {
				m_data = NULL;
				m_size(0) = m_size(1) = 0;
				return;
			}
			m_data = new TPtr[l];
			if (m_data == NULL)
				lglERROR("Array2D<T>::m_build : "<<l<<","<<c<<") : allocation failed");
			TPtr pt = new T[l * c];
			if (pt == NULL)
				lglERROR("Array2D<T>::m_build : "<<l<<","<<c<<") : allocation failed");
			for (int i = 0; i < l; ++i)
				m_data[i] = pt + i * c;
			m_size(0) = l;
			m_size(1) = c;
		}

		void m_free() {
			if (m_data == NULL)
				return;
			delete[] m_data[0];
			delete[] m_data;
			m_data = NULL;
		}
	};
	//=================================================================================
	// EO class Array2D<T>
	//=================================================================================

	//=================================================================================
	/// ostream insertion of an Array2D
	template<class T>
	std::ostream& operator <<(std::ostream& s, Array2D<T> const & p) {
		int x, y;
		for (x = 0; x < p.size(0); ++x) {
			s << "[ ";
			for (y = 0; y < p.size(1); ++y) {
				s << p(x, y) << " ";
			}
			s << "]" << std::endl;
		}
		return (s);
	}
	//=================================================================================

	//=================================================================================
	/// iterator class for Array2D
	template<class T, class RefT>
	class Array2D_iterator {

	public:
		typedef Array2D_iterator<T, RefT> self;
		typedef T value_type;
		typedef RefT RefType;

		Array2D_iterator() :
				m_ptr(0) {
		}
		Array2D_iterator(T * p) :
				m_ptr(p) {
		}

		inline bool operator !=(const self & i) const {
			return (m_ptr != i.m_ptr);
		}
		inline bool operator ==(const self & i) const {
			return (m_ptr == i.m_ptr);
		}

		inline RefType operator *() {
			return (*m_ptr);
		}

		inline self & operator =(self const & i) {
			m_ptr = i.m_ptr;
			return (*this);
		}
		inline self & operator ++() {
			++m_ptr;
			return (*this);
		}
		inline self & operator +=(int offset) {
			m_ptr += offset;
			return (*this);
		}

	private:
		T * m_ptr;
	};
//=================================================================================
// EO Array2D_iterator
//=================================================================================

}
//=================================================================================
// EO namespace lgl
//=================================================================================

//=================================================================================
// EOF
//=================================================================================
#endif

