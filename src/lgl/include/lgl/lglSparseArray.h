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
/**	\file	
 \brief map<int,T>-like objects which allow constant time random access.
 **/
#ifndef __lglSparseArray_h__
#define __lglSparseArray_h__
//===========================================================
// namespace	: lgl
// class		: SparseArray
//	map<int,T>-like objects which allow constant time random access.
//  The array is handled by a 256-ary tree of 4 levels  
//===========================================================
// author		: Laurent Guigues
// history		: 
//	??/10/98		first release at IGN/MATIS
//	12/07/04		rewritten in lGl
//===========================================================

#include "lgl/lglGeneral.h"

namespace lgl {
	//=================================================================
	/// Union between an UI32 and 4 UI8
	typedef union {
		UI32 i;
		UI8 b[4];
	} UI32_4xUI8;
	//=================================================================

	//=================================================================
	// Iterators
	template<class T, class RefT> class SparseArrayIterator;
	// Holes iterator !
	// In a sparse array, it is useful to iterate the holes...
	template<class T, class RefT> class SparseArrayHoleIterator;
	//=================================================================

	//=================================================================
	// SparseArray
	template<class T> class SparseArray;
	template<class T> std::ostream& operator <<(std::ostream&, SparseArray<T>&);
	//=================================================================

	//=================================================================
	///	\brief map<int,T>-like objects which allow constant time random access.
	///
	///  The array is handled by a 256-ary tree of 4 levels
	template<class T> class SparseArray {
	public:
		//=================================================================
		// typedefs
		typedef UI32 index;
		typedef SparseArrayIterator<T, T&> iterator;
		typedef SparseArrayIterator<T, const T&> const_iterator;
		typedef SparseArrayHoleIterator<T, T&> hole_iterator;
		typedef SparseArrayHoleIterator<T, const T&> const_hole_iterator;

		FRIEND_CLASS SparseArrayIterator<T, T&> ;FRIEND_CLASS SparseArrayIterator<T, const T&> ;FRIEND_CLASS SparseArrayHoleIterator<T, T&> ;FRIEND_CLASS SparseArrayHoleIterator<
				T, const T&> ;
		//=================================================================

		//=================================================================
		/** Constructor.
		 - free gives the particular value of T which marks an unused cell (e.g. null pointer).
			- memset_allowed=TRUE causes a memset initialization of the newly created cells to the value free. */
		SparseArray(T free, BOOL memset_allowed = FALSE);
		/** Copy constructor */
		SparseArray(const SparseArray&);
		/** Set */
		SparseArray& operator =(const SparseArray&);
		/** Destructor */
		~SparseArray();
		//=================================================================

		//=================================================================
		// Iteration
		iterator begin();
		const_iterator begin() const;
		inline iterator end();
		inline const_iterator end() const;
		iterator begin(index i);
		const_iterator begin(index i) const;
		// Hole iteration
		hole_iterator begin_hole();
		const_hole_iterator begin_hole() const;
		inline hole_iterator end_hole();
		inline const_hole_iterator end_hole() const;
		hole_iterator begin_hole(index rang);
		const_hole_iterator begin_hole(index rang) const;
		//=================================================================

		//=================================================================
		/** random acces.
		 cost = 3 null pointer tests if the cell is allocated.
			WARNING : this method is apparently const, however
			it can cause memory allocations and indirect change of the structure... */
		T& operator [](index rang) const;
		//=================================================================

		//=================================================================
		/** counts the number of elements in the array */
		UI32 size() const;
		//=================================================================

		//=================================================================
		/** Frees unused memory (256 elements segments whose value is free and useless intermediate tree segments).
		 returns the memory freed.   */
		UI32 freeUnusedMemory(BOOL verbose = FALSE);
		/** returns the total memory used by the array */
		UI32 memoryUsage(BOOL verbose = FALSE) const;
		//=================================================================

		//=================================================================
		/** envoi dans un flux. donne memsize et card  */
		friend std::ostream& operator <<NESTED_TEMPLATE(T) (std::ostream&, SparseArray<T>&); // OK
		//=================================================================

		//=================================================================
	protected :
		/** The free cell value */
		T m_free;
		/** when a pointer is on &m_end, its is "past_the_end" */
		T m_end;
		/** Root of the tree (256 ptr) */
		void** m_root;
		/** Memset allowed on T ? */
		BOOL m_memset_allowed;
		/** Sizeof a 256 pointers array */
		static UI32 m_size256ptr;
		/** Sizeof a 256 array of type T */
		static UI32 m_size256T;
		//=================================================================

	};
	//=================================================================
	// EO class SparseArray
	//=================================================================

	//=================================================================
	//=================================================================
	/** SparseArray elements iterator */
	template<class T, class RefT> class SparseArrayIterator {
	public:
		//=================================================================
		FRIEND_CLASS SparseArray<T> ;
		typedef typename SparseArray<T>::index index;
		//=================================================================
		typedef SparseArrayIterator<T, RefT> self;
		//=================================================================
		inline SparseArrayIterator() {
		}
		inline SparseArrayIterator(const self&);
		inline self& operator =(const self&);
		inline ~SparseArrayIterator() {
		}
		//=================================================================
		bool operator ==(const self& i) const {
			return (p3 == i.p3);
		}
		bool operator !=(const self& i) const {
			return (p3 != i.p3);
		}
		//=================================================================
		RefT operator ()() const {
			return *p3;
		}
		RefT operator *() const {
			return *p3;
		}
		//=================================================================
		self& operator++();
		//=================================================================
		index pos() const {
			return cur.i;
		}
		//=================================================================

	private:
		//=================================================================
		inline SparseArrayIterator(void** r, T free, void* end);
		//=================================================================
		T m_free;
		T* m_end;
		void **m_root;
		void **p0, **p1, **p2;
		T *p3;
		UI32_4xUI8 cur;
		//=================================================================
	};
	//=================================================================
	// EO SparseArrayIterator
	//=================================================================

	//=================================================================
	/** SparseArray holes iterator */
	template<class T, class RefT> class SparseArrayHoleIterator {
	public:
		//=================================================================
		FRIEND_CLASS SparseArray<T> ;
		typedef typename SparseArray<T>::index index;
		//=================================================================
		typedef SparseArrayHoleIterator<T, RefT> self;
		//=================================================================
		inline SparseArrayHoleIterator() {
		}
		inline SparseArrayHoleIterator(const self&);
		inline self& operator =(const self&);
		inline ~SparseArrayHoleIterator() {
		}
		//=================================================================
		bool operator ==(const self& i) const {
			return (p3 == i.p3);
		}
		bool operator !=(const self& i) const {
			return (p3 != i.p3);
		}
		//=================================================================
		RefT operator ()() const {
			return *p3;
		}
		RefT operator *() const {
			return *p3;
		}
		//=================================================================
		self& operator++();
		//=================================================================
		index pos() const {
			return cur.i;
		}
		//=================================================================

	private:
		//=================================================================
		inline SparseArrayHoleIterator(void** r, T free, void* end, BOOL init);
		//=================================================================
		T m_free;
		T* m_end;
		bool m_memset_allowed;
		void **m_root;
		void **p0, **p1, **p2;
		T *p3;
		UI32_4xUI8 cur;
		//=================================================================

	};
//=================================================================
// EO SparseArrayHoleIterator
//=================================================================

//=================================================================
// template codes inclusion
#include "lgl/lglSparseArray_code.h"
//=================================================================
}
//=================================================================
// EO namespace lgl
//=================================================================

//=================================================================
// EOF
//=================================================================
#endif

