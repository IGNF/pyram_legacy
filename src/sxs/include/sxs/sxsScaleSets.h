/* 

 SxS : A C++ implementation of the scale climbing algorithm for multiscale image segmentation.
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
#ifndef __sxsScaleSets_h__
#define __sxsScaleSets_h__
//===========================================================
// namespace	: sxs
// class		: ScaleSets
/*! \file 
 \brief Scale-Sets structures (stratified hierarchies) which are output by the ScaleClimbing algorithm
 */
//===========================================================
// author		: Laurent Guigues
// history		: 
//	11/07/04		creation
//===========================================================
#include <vector>
#include <list>
#include <limits>
#include "lgl/lglBitManager.h"
#include "lgl/lglSparseArray.h"

#include "sxs/sxsGeneral.h"


namespace sxs {
	//================================================================
	/// base type used as a BitVector of marks on Sets
	typedef lgl::UI8 sxsSET_MARK_TYPE;
	//================================================================

	//================================================================
	/// A Scale-Sets structure, i.e. a stratified hierarchy.
	class ScaleSets {
	public:
		//================================================================
		/// A Set of the ScaleSets
		class Set;
		//================================================================
		/// The Slot class used to mark the Sets
		typedef lgl::BitSlot Slot;
		//================================================================

		//================================================================
		/// Ctor
		ScaleSets();
		/// Dtor
		virtual ~ScaleSets();
		/// Copy ctor
		ScaleSets(const ScaleSets&);
		//================================================================

		//================================================================
		/// Loads the ScaleSets from a .sxs file
		lgl::BOOL load(const lgl::Filename& filename);
		/// Saves the ScaleSets in a .sxs file
		lgl::BOOL save(const lgl::Filename& filename);
		//================================================================

		//================================================================
		/// inserts the Set* in the structure (updates father relationships according to the sons of the set)
		void insert(Set*);
		/// removes the Set* from the structure (updates father/son relationships)
		void remove(Set*);
		//================================================================

		//================================================================
		/// Returns the number of sets
		lgl::I32 size() const {
			return (lgl::I32) m_list.size();
		}
		/// Retunrs the first free index
		IndexType firstFreeIndex() const {
			return m_first_free_index;
		}
		//================================================================

		//================================================================
		/// Dumps the structure of the ScaleSets in text format
		void textDump(std::ostream& o) const;
		//================================================================
		/// Clears the ScaleSets
		void clear();
		//================================================================

		//================================================================
		/// returns a pointer on the Set of index i
		inline Set* operator[](IndexType i) const {
			return m_set[i];
		}
		//================================================================
		/// returns a pointer on the root Set
		inline Set* getRoot() const {
			return m_list.back();
		}
		//================================================================
		/// returns a const ref on the list of Set*
		inline const std::list<Set*>& Sets() const {
			return m_list;
		}
		//================================================================

		//================================================================
		/// allocates a Slot
		lgl::BOOL allocateSlot(Slot& s, lgl::UI8 length = 1) {
			return m_marker.allocate(length, s);
		}
		/// frees the Slot s
		void freeSlot(Slot& s) {
			m_marker.free(s);
		}
		//================================================================

		/// Returns the memory used by the SparseArray of Sets
		lgl::UI32 memoryUsage(lgl::BOOL verbose) const {
			return m_set.memoryUsage(verbose);
		}
		//================================================================
		// ALGORITHMS
		//================================================================
		/// Removes the Sets whose Slot s is marked
		void removeMarkedSets(const Slot& s);
		/// Deletes the sets whose persistence is lower than a given threshold.
		/// Performs a top-down traversal to compute the scale of disappearance of the sets
		/// and simultaneously removes the sets of too low persistence.
		virtual void removeNonPersistentSets(lgl::F32 minimum_persistence = 1.);
		//================================================================

		//================================================================
		// verifies the coherence of the Slot allocation :
		// all unallocated bits should be set to 0
		lgl::BOOL verifySlots();
		//================================================================

	protected:
		//================================================================
		/// list of Sets
		std::list<Set*> m_list;
		/// SparseArray for direct Set acces by index
		lgl::SparseArray<Set*> m_set;
		/// list of the roots of the tree
		// useless : the scale-sets should always have a single root which is the last
		// element of m_list.
		//			std::list<Set*>				m_root;
		IndexType m_first_free_index;
		//================================================================
		/// BitManager for the Sets marks
		lgl::BitManager<sxsSET_MARK_TYPE> m_marker;
		//================================================================

		//================================================================
		/// Recursive function used to mark non persistent sets in removeNonPersistentSets
		void m_rec_mark_non_persistent(Set* s, const Slot& m, lgl::F32 scale_of_disappearance,
				lgl::F32 minimum_persistence);
		//================================================================
		void m_rec_text_dump(Set* s, std::ostream& o) const;
	};
	//================================================================
	// EO class ScaleSets
	//================================================================

	//================================================================
	/// A set of a ScaleSets
	class ScaleSets::Set {
	public:
		//================================================================
		/// \brief Minimum attribution class of a Set
		class Attribute {
		public:
			/// Virtual dtor
			virtual ~Attribute() {
			}

			/// TO DO : Generic attribute identification/creation
			///

			/// \brief Reads the attribute values from a binary ifstream.
			///
			/// \return true if succesfull
			virtual lgl::BOOL readBinary(std::ifstream&) {
				return true;
			}
			/// \brief Writes the attribute values in a binary ofstream
			///
			/// \return true if succesfull
			virtual lgl::BOOL writeBinary(std::ofstream&) {
				return true;
			}
		};
		//================================================================
		typedef Set* pSet;
		//================================================================

		//================================================================
		/// ctor
		Set(IndexType i = -1, lgl::F32 scale = 0) :
				m_index(i), m_app(scale), m_att(0), m_mark(0) {
			m_fat = this;
		}
		/// dtor
		~Set() {
			if (m_att)
				delete m_att;
		}
		//================================================================

		//================================================================
		/// Returns a reference on the index of the set
		//inline IndexType& getIndex() { return m_index; }
		/// Returns a the index of the set
		inline IndexType index() const {
			return m_index;
		}
		//================================================================

		//================================================================
		/// returns a reference on the pointer on the father of the set
		inline pSet& father() {
			return m_fat;
		}
		/// returns a pointer on the father of the set
		inline Set* father() const {
			return m_fat;
		}
		/// returns a reference on the vector of sons of the set
		inline std::vector<Set*>& sons() {
			return m_son;
		}
		/// returns a const reference on the vector of sons of the set
		inline const std::vector<Set*>& sons() const {
			return m_son;
		}
		//================================================================

		//================================================================
		/// returns TRUE iff the Set belong to the base of the ScaleSets
		inline lgl::BOOL isBase() const {
			return (m_son.size() == 0);
		}
		/// returns TRUE iff the Set is a root of the ScaleSets
		inline lgl::BOOL isRoot() const {
			return (m_fat == this);
		}
		//================================================================

		//================================================================
		/// returns a reference on the scale of appearance of the set
		inline lgl::F32& scaleOfAppearance() {
			return m_app;
		}
		/// returns a const reference on the scale of appearance of the set
		inline const lgl::F32& scaleOfAppearance() const {
			return m_app;
		}
		//================================================================
		/// returns the scale of disappearance of the set which is the scale
		/// of appearance of the sets's father (when the scale-sets is persistent).
		inline lgl::F32 scaleOfDisappearance() const {
			return m_fat->m_app;
		}
		/// returns the persistence of the set which is the ratio between
		/// the set's scale of disappearance and the set's scale of appearance
		/// (when the scale-sets is persistent).
		inline lgl::F32 persistence() const {
			if (m_app <= 0)
				return (std::numeric_limits<lgl::F32>::max)() * 1e-8;
			return (m_fat->m_app / m_app);
		}
		//================================================================
		/// returns a pointer on the Attribute of the set
		inline Attribute* attribute() {
			return m_att;
		}
		/// returns a const pointer on the Attribute of the set
		inline const Attribute* attribute() const {
			return m_att;
		}
		/// Sets the Attribute of the Set. The Set becomes responsible for its deletion.
		inline void setAttribute(Attribute* a) {
			if (m_att)
				delete m_att;
			m_att = a;
		}
		//================================================================

		//================================================================
		/// Sets the mark corresponding to Slot s to v
		inline void setMark(const lgl::BitSlot& s, sxsSET_MARK_TYPE v) {
			lgl::BitManager<sxsSET_MARK_TYPE>::set(m_mark, s, v);
		}
		/// Gets the value of the mark corresponding to Slot s
		inline sxsSET_MARK_TYPE getMark(const lgl::BitSlot& s) const {
			return lgl::BitManager<sxsSET_MARK_TYPE>::get(m_mark, s);
		}
		/// Gets the full mark vector of the Set
		inline sxsSET_MARK_TYPE getMarkVector() const {
			return m_mark;
		}
		//================================================================

		//================================================================
		/// Returns a pointer on the maximal ancestor of the Set whose scale of appearance is lower than scale
		/// Returns this if the Set's scale of disapearance is lower than scale
		Set* getMaximalAncestor(lgl::F32 scale);
		//================================================================
		//================================================================
		/// Returns a pointer on the minimal ancestor of the Set whose Slot s is set to v.
		/// Returns null pointer if no ancestor verifies the condition
		Set* getMinimalMarkedAncestor(const lgl::BitSlot& s, sxsSET_MARK_TYPE v);
		//================================================================

		//================================================================
		/// Reads the Set from a binary ifstream
		/// calls the read method of T to fill the attribute
		/// the ScaleSets sxs is used to convert son indices to Set*
		lgl::BOOL readBinary(std::ifstream& s, ScaleSets* sxs);
		/// Writes the Set in a binary ofstream
		/// calls the write method of T to write the attribute
		lgl::BOOL writeBinary(std::ofstream& s);
		//================================================================

		//================================================================
		inline std::list<Set*>::iterator& self_position() {
			return m_self_position;
		}
		//================================================================

	protected:
		//================================================================
		/// index
		IndexType m_index;
		/// scale of appearance
		lgl::F32 m_app;
		/// father
		Set* m_fat;
		/// vector of sons
		std::vector<Set*> m_son;
		/// attribute
		Attribute* m_att;
		/// vector of marks
		sxsSET_MARK_TYPE m_mark;
		/// Iterator pointing the Set's position in the ScaleSets list of sets
		std::list<Set*>::iterator m_self_position;
		//================================================================
	};
//================================================================
// EO class ScaleSets::Set
//================================================================

}

//===========================================================
// EOF
//===========================================================
#endif

