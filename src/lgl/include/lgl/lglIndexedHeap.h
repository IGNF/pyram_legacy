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
 \brief Indexed priority queues handled by binary trees.
 */
#ifndef __lglIndexedHeap_h__
#define __lglIndexedHeap_h__
//===========================================================
// namespace	: lgl
// class		: IndexedHeap
//	Indexed priority queues handled by binary trees
//	Heap Allows :
//		log(n) insertion
//		constant time acces to the first element
//		log(n) removal of the first element
//		log(n) priority change of a random element 
//  Indexation Allows :
//		constant time access to a random element (for priority change)
// The index must be a descendent of unary_function<T,int&>
//===========================================================
// author		: Laurent Guigues
// history		: 
//	04/10/99		first release at IGN/MATIS
//	09/07/04		reshaped (CREATIS postdoc)
//===========================================================
#include <vector>
#include "lgl/lglGeneral.h"
#include "lgl/lglFunction.h"



namespace lgl {

	template<class T, class Comparator/*=Less<T>*/, class Indexer/*=Index<T> */> class IndexedHeap;
	template<class T, class C, class I> std::ostream& operator <<(std::ostream&, const IndexedHeap<T, C, I>&);

	//template <class T, class Comparator=std::less<T>, class Index=IndexIndex<T> > class SlicedIndexedHeap;

	//=============================================================================
	/// \brief Indexed priority queues handled by binary trees.
	///
	///	Heap Allows :
	///		- log(n) insertion
	///		- constant time acces to the first element
	///		- log(n) removal of the first element
	///		- log(n) priority change of a random element
	/// Indexation Allows :
	///		- constant time access to a random element (for priority change)
	///
	/// The Indexer is an unary_function<T,int&> whose operator()(T& t) returns a reference on an integer which
	/// is maintained by the IndexedHeap in order to provide at any time the position of the object t in the Heap
	/// (hence allowing constant time random access to an object).
	template<class T, class Comparator /*=Less<T>*/, class Indexer /*=Index<T>*/>
	class IndexedHeap {
//			friend class SlicedIndexedHeap<T,Comparator,Index>;
	public:

		//===========================================================================
		/// Constructor
		IndexedHeap() {
		}
		/// Constructor
		IndexedHeap(const Comparator& comp, const Indexer& ind);
		/// Destructor
		~IndexedHeap() {
		}
		/// Sets the comparator
		void set(const Comparator& comp);
		/// Sets the Index
		void set(const Indexer& ind);
		//===========================================================================

		//===========================================================================
		/// inserts an element in the Heap and returns its position
		I32 insert(T);
		/// return a reference on the first element of the Heap
		T& top();
		/// return a constant reference on the first element of the Heap
		const T& top() const;
		/// removes and returns the first element of the Heap
		T remove_top();
		/// removes and returns the nth element
		T remove(I32 n);
		/// returns the size of the Heap
		inline I32 size() const {
			return (I32) m_p.size();
		}
		/// empties the Heap
		void clear();
		//===========================================================================

		//===========================================================================
		/// returns a constant on the stack of elements
		const std::vector<T> & stack() const {
			return m_p;
		}
		/// returns a reference to the ith element of the stack
		T& operator [](I32 i) {
			return m_p[i];
		}
		/// returns a constant reference to the ith element of the stack
		const T& operator [](I32 i) const {
			return m_p[i];
		}
		/// returns the index (position) of t
		inline I32 index(T& t) {
			return (*m_i)(t);
		}
		//===========================================================================

		//===========================================================================
		/// returns the position of the father of i
		inline I32 father(I32 i) const;
		/// returns the position of the right son of i
		inline I32 rightson(I32 i) const;
		/// returns the position of the leftson of i
		inline I32 leftson(I32 i) const;
		//===========================================================================
		/// swaps ith and jth elements
		inline void swap(I32 i, I32 j);
		/// remonte un element dans le tas tant qu'il n'est pas a sa place.
		/// renvoie la position finale
		inline I32 upsort(I32);
		/// descend un element dans le tas tant qu'il n'est pas a sa place.
		/// renvoie la position finale
		inline I32 downsort(I32);
		//===========================================================================

	protected:
		/// binary tree handled by a vector
		std::vector<T> m_p;
		/// comparator pointer
		const Comparator* m_c;
		/// Index pointer
		const Indexer* m_i;
	};
//===========================================================================
// EO class IndexedHeap
//===========================================================================

#include "lgl/lglIndexedHeap_code.h"

}
//===========================================================================
// EO namespace lgl
//===========================================================================

//===========================================================================
// EOF
//===========================================================================
#endif
