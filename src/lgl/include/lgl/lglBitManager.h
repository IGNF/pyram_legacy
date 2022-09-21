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
 \brief Classes handling the allocation/desallocation/setting/unsetting of contiguous sequences of bits of a base type T.
 */
#ifndef __lglBitManager_h__
#define __lglBitManager_h__
//===========================================================
// namespace	: lgl
// classes		: BitManager<T>
//				  BitSlot
//	Manages the allocation/desallocation/setting/unsetting 
//  of contiguous sequences of bits of a base type T.
// methods		: template <class T> std::ostream& operator < (std::ostream& s, T x);
//	pushes in the stream the value of x in binary notation (e.g. s<6 pushes "00000110")
//===========================================================
// author		: Laurent Guigues
// history		: 
//	19/02/99		first release at IGN/MATIS (FlagRow class)
//	09/07/04		reshaped
//===========================================================
#include <ostream>
#include "lgl/lglGeneral.h"


namespace lgl {

	//============================================================================
	// Class BitSlot
	//============================================================================
	/// A BitSlot is a sequence of successive bits in a vector of bits
	class BitSlot {

	private:
		/// Position of the first bit
		UI8 m_p;
		/// Length of the Slot-1 (at most 255 bits handled !)
		UI8 m_n;
	public:
		//============================================================================
		/// Constructor with position and length
		inline BitSlot(UI8 position = 0, UI8 length = 1) :
				m_p(position), m_n((UI8)(length - (UI8) 1)) {
		}
		/// Copy contructor
		inline BitSlot(const BitSlot& s) :
				m_p(s.m_p), m_n(s.m_n) {
		}
		/// Set operator
		inline BitSlot& operator=(const BitSlot& s) {
			m_p = s.m_p;
			m_n = s.m_n;
			return *this;
		}
		/// Destructor
		inline ~BitSlot() {
		}
		/// Gets position
		inline UI8 getPosition() const {
			return m_p;
		}
		/// Gets length (BE CAREFUL ! == m_n+1)
		inline UI8 getLength() const {
			return (UI8)(m_n + 1);
		}
		/// Gets length-1 (i.e. the field m_n)
		inline UI8 getN() const {
			return m_n;
		}
		//============================================================================
	};
	//============================================================================
	// EO Class BitSlot
	//============================================================================

	//============================================================================
	// Class BitManager
	//============================================================================
	/// \brief Handles a basic type as a vector of bits.
	///
	/// Allows allocation/desallocation of BitSlots,
	/// that is contiguous sequences of bits of the type T.
	/// Allows to set/get values of specific bits of a type T through BitSlots.
	template<class T> class BitManager {
	public:
		//============================================================================
		/// Default constructor
		BitManager(T used_bits_map = 0);
		/// Destructor
		~BitManager();
		/// Copy constructor
		BitManager(const BitManager& f);
		/// Affectation
		BitManager& operator =(const BitManager& f);
		//============================================================================

		//============================================================================
		/// Compares the state of two managers :
		/// TRUE iff the same bits are allocated for the two
		/// BE CAREFUL !! Does'nt mean that the Slot distribution is the same !!
		inline BOOL operator ==(BitManager& f) const;
		//============================================================================

		//============================================================================
		/// How many bits in a vector ?
		static inline int nbBits();
		//============================================================================

		//============================================================================
		/// Allocates a l bits Slot. returns TRUE if possible.
		inline BOOL allocate(UI8 l, BitSlot& s);
		/// Frees a BitSlot
		inline void free(BitSlot s);
		/// How many bits free ?
		inline int nbBitsFree() const;
		/// How many bits used ?
		inline int nbBitsUsed() const;
		/// returns the map of bits used
		inline T usedBitsMap() const;
		//============================================================================

		//============================================================================
		/// Gives a bitmask of the BitSlot
		static inline T mask(BitSlot s);
		/// Gives the opposite bitmask of the BitSlot
		static inline T negmask(BitSlot s);
		//============================================================================

		//============================================================================
		/// sets the bits of f corresponding to the BitSlot s to 1
		static inline void set(T& f, BitSlot s); // OK
		/// sets the bits of f corresponding to the BitSlot s to 0
		static inline void unset(T& f, BitSlot s); // OK
		/// sets all the bits of f to 1
		static inline void set(T& f); // OK
		/// sets the bits of f to 0
		static inline void unset(T& f); // OK
		/// sets the n bits of f corresponding to the BitSlot s to the n first bits of v
		static inline void set(T& f, BitSlot s, T v); // OK
		/// gets the value of the BitSlot s of f
		static inline T get(T f, BitSlot s); // OK
		/// bitwise not on the bits of f corresponding to BitSlot s
		static inline void bnot(T& f, BitSlot s);
		/// bitwise and on the bits of f corresponding to BitSlot s
		static inline void band(T& f, BitSlot s, T v);
		/// bitwise or on the bits of f corresponding to BitSlot s
		static inline void bor(T& f, BitSlot s, T v);
		/// multi-set (bitwise or)
		static inline void mset(T& f, T Mask); // OK
		/// multi-unset. (f and not Mask).
		static inline void munset(T& f, T Mask); // OK
		//============================================================================

		//============================================================================
	private:
		/// size of the bit vectors (8*sizeof(T))
		static const UI8 m_size;
		/// bituse map
		T m_use;
		/// number of allocated bits
		UI8 m_n;
		/// masks of BitSlots
		/// at i+j*size is the mask of the BitSlot beginning at i and of length j+1
		static T m_mask[64 * sizeof(T) * sizeof(T)];
		/// must the masks be computed ?
		static BOOL m_must_compute_mask;
		/// computes msk. called by the first ctor. sets m_must_compute_msk to TRUE.
		static void compute_mask();
		//============================================================================

	};
//============================================================================
// EO class BitManager
//============================================================================

//============================================================================
// template codes inclusion
#include "lgl/lglBitManager_code.h"
//============================================================================

}
//============================================================================
// EO namespace lgl
//============================================================================

//============================================================================
// EOF
//============================================================================
#endif

