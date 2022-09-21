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
/*! \file 
 \brief Code of the methods of class ScaleSets and the nested class Set
 */
#include <sxs/sxsScaleSets.h>
#include "lgl/lglMachine.h"
#include "lgl/lglString.h"
#include "lgl/lglGeneral.h"

//#include <wx/log.h>

namespace sxs {

	//================================================================
	// Returns a pointer on the maximal ancestor of *this whose scale of appearance is lower than scale
	ScaleSets::Set* ScaleSets::Set::getMaximalAncestor(lgl::F32 scale) {
		Set* p = this;
		while ((!p->isRoot()) && (p->scaleOfDisappearance() < scale))
			p = p->father();
		return p;
	}
	//================================================================

	//================================================================
	/// Returns a pointer on the minimal ancestor of *this whose Slot s is set to v

	ScaleSets::Set* ScaleSets::Set::getMinimalMarkedAncestor(const lgl::BitSlot& s, sxsSET_MARK_TYPE v) {
		Set* p = this->father();
		while ((p->getMark(s) != v) && (!p->isRoot()))
			p = p->father();
		if (p->getMark(s) == v)
			return p;
		return (Set*) 0;
	}
	//================================================================

	//================================================================
	/// Reads the Set from a binary ifstream
	/// calls the read method of T to fill the attribute
	/// the ScaleSets sxs is used to convert son indices to Set*

	lgl::BOOL ScaleSets::Set::readBinary(std::ifstream& s, ScaleSets* sxs) {
		// index
		s.read((char*) &m_index, sizeof(m_index));
#ifdef __MAC__ 
		lgl::Machine::swapEndians(m_index);
#endif //#ifdef __MAC__
		/// scale of appearance
		s.read((char*) &m_app, sizeof(m_app));
#ifdef __MAC__ 
		lgl::Machine::swapEndians(m_app);
#endif //#ifdef __MAC__
		/// vector of sons
		/// size
		int nbs;
		s.read((char*) &nbs, sizeof(nbs));
#ifdef __MAC__ 
		lgl::Machine::swapEndians(nbs);
#endif //#ifdef __MAC__
		/// indices of the sons
		for (int i = 0; i < nbs; ++i) {
			IndexType id;
			s.read((char*) &id, sizeof(id));
#ifdef __MAC__ 
			lgl::Machine::swapEndians(id);
#endif //#ifdef __MAC__
			Set* ps = sxs->operator[](id);
			if (ps)
				m_son.push_back(ps);
			else {
				lglERROR("ScaleSets::Set::read : the index of the son of the set read is unknown");
			}
		}
		/// attribute
		if (!m_att->readBinary(s))
			return false;
		return true;
	}
	//================================================================

	//================================================================
	/// Writes the Set in a binary ofstream
	/// calls the write method of T to write the attribute

	lgl::BOOL ScaleSets::Set::writeBinary(std::ofstream& s) {
		// index
		IndexType t1 = m_index;
#ifdef __MAC__
		lgl::Machine::swapEndians(t1);
#endif //#ifdef __MAC__
		s.write((char*) &t1, sizeof(IndexType));

		/// scale of appearance
		lgl::F32 t2 = m_app;
#ifdef __MAC__
		lgl::Machine::swapEndians(t2);
#endif //#ifdef __MAC__
		s.write((char*) &t2, sizeof(lgl::F32));

		/// vector of sons
		/// size
		int nbs = (int) m_son.size();
#ifdef __MAC__
		lgl::Machine::swapEndians(nbs);
#endif //#ifdef __MAC__
		s.write((char*) &nbs, sizeof(int));

		/// indices of the sons
		std::vector<Set*>::const_iterator i;
		for (i = m_son.begin(); i != m_son.end(); ++i) {
			IndexType id = (*i)->index();
#ifdef __MAC__
			lgl::Machine::swapEndians(id);
#endif //#ifdef __MAC__
			s.write((char*) &id, sizeof(IndexType));
		}
		/// attribute
		if (!m_att->writeBinary(s))
			return false;
		return true;
	}
	//================================================================

	//================================================================

	ScaleSets::ScaleSets() :
			m_set(0), m_first_free_index(0) {
	}
	//================================================================

	//================================================================

	ScaleSets::~ScaleSets() {
		std::list<Set*>::iterator i;
		for (i = m_list.begin(); i != m_list.end(); ++i) {
			delete *i;
		}
	}
	//================================================================

	//================================================================
	/// Loads the ScaleSets from a .sxs file

	lgl::BOOL ScaleSets::load(const lgl::Filename& filename) {
		bool verbose = false;
#ifndef LGL_WITH_BOOST
		if (verbose) wxLogWarning(_T("ScaleSets::load 1 filename '%s'"), filename.GetFullName().c_str());
#else
		// if (verbose) wxLogWarning(_T("ScaleSets::load 1 filename '%s'"), filename.generic_string().c_str());
		if (verbose) {
			std::string msg = lgl::string_format(std::string("ScaleSets::load 1 filename '%s'"), filename.generic_string().c_str());
			lglWARNING(msg);
		}
#endif
		clear();

		std::ifstream f;
#ifndef LGL_WITH_BOOST

	#if wxUSE_UNICODE
			f.open(filename.GetFullPath().ToAscii().data(),PARAM_IFSTREAM_BINARY);
	#else
			f.open(filename.GetFullPath().c_str(), PARAM_IFSTREAM_BINARY);
	#endif //#if wxUSE_UNICODE
#else
		f.open(filename.generic_string().c_str(), PARAM_IFSTREAM_BINARY);
#endif
		if (!f.good())
			return false;
		// number of sets
		int nb;
		f.read((char*) &nb, sizeof(int));
#ifdef __MAC__
		lgl::Machine::swapEndians(nb);
#endif //#ifdef __MAC__
		// reads the sets
		for (int i = 0; i < nb; ++i) {
			Set* s = new Set;
			// creates the atribute
			s->setAttribute(new Set::Attribute);
			if (!s->readBinary(f, this)) {
				f.close();
				return false;
			}
			insert(s);
		}
		f.close();

		return true;
	}
	//================================================================

	//================================================================
	/// Saves the ScaleSets in a .sxs file

	lgl::BOOL ScaleSets::save(const lgl::Filename& filename) {
		std::ofstream f;
#ifndef LGL_WITH_BOOST
	#if wxUSE_UNICODE
			f.open(filename.GetFullPath().ToAscii().data(),PARAM_OFSTREAM_BINARY);
	#else
			f.open(filename.GetFullPath().c_str(), PARAM_OFSTREAM_BINARY);
	#endif //#if wxUSE_UNICODE
#else
		f.open(filename.generic_string().c_str(), PARAM_OFSTREAM_BINARY);
#endif
		if (!f.good())
			return false;

		// number of sets
		int nb = (int) m_list.size();
#ifdef __MAC__
		lgl::Machine::swapEndians(nb);
#endif //#ifdef __MAC__
		f.write((char*) &nb, sizeof(int));

		// saves the sets in list order
		std::list<Set*>::iterator i;
		for (i = m_list.begin(); i != m_list.end(); ++i) {
			if (!(*i)->writeBinary(f)) {
				f.close();
				return false;
			}
		}
		f.close();
		return true;
	}
	//================================================================

	//================================================================

	void ScaleSets::insert(Set* s) {
		// TEST
		if (s->index() < m_first_free_index)
			lglERROR("ScaleSets::insert(Set*) : index of the Set below first free index");

		// list insertion
		m_list.push_back(s);
		// self position in m_list
		s->self_position() = --m_list.end();
		// array insertion
		m_set[s->index()] = s;
		// father relationship
		std::vector<Set*>::iterator i;
		for (i = s->sons().begin(); i != s->sons().end(); ++i)
			(*i)->father() = s;
		// first free
		if (s->index() >= m_first_free_index)
			m_first_free_index = s->index() + 1;
	}
	//================================================================

	//================================================================

	void ScaleSets::remove(Set* s) {
		// root deletion not allowed
		if (s->isRoot()) {
			lglWARNING("ScaleSets::remove : trying to remove the root !");
			return;
		}
		// s removed from its father's sons
		std::vector<Set*>::iterator i;
		for (i = s->father()->sons().begin(); (*i) != s; ++i)
			;
		s->father()->sons().erase(i);
		// s's sons captured by father
		for (i = s->sons().begin(); i != s->sons().end(); ++i) {
			s->father()->sons().push_back(*i);
			// father changes for the sons
			(*i)->father() = s->father();
		}
		// list removal
		m_list.erase(s->self_position());
		// array removal
		m_set[s->index()] = NULL;
		// set deletion
		delete s;
	}
	//================================================================

	//================================================================
	/// Clears the ScaleSets

	void ScaleSets::clear() {
		while (m_list.size() > 0) {
			Set* s = m_list.front();
			m_list.pop_front();
			m_set[s->index()] = NULL;
			delete s;
		}
		m_set.freeUnusedMemory(false);
		m_marker = lgl::BitManager<sxsSET_MARK_TYPE>(0);
		m_first_free_index = 0;
	}
	//================================================================

	//================================================================
	// ALGORITHMS
	//================================================================
	//================================================================
	/// Removes the Sets whose Slot s is marked

	void ScaleSets::removeMarkedSets(const Slot& m) {
		std::list<Set*>::iterator i;
		for (i = m_list.begin(); i != m_list.end();) {
			Set* s = *i;
			++i;
			// set deletion if marked
			if (s->getMark(m) == 1)
				remove(s);
		}
	}
	//================================================================

	//================================================================
	/// Deletes the sets whose persistence is lower than a given threshold.
	/// Performs a top-down traversal to compute the scale of disappearance of the sets
	/// and simultaneously removes the sets of too low persistence.

	void ScaleSets::removeNonPersistentSets(lgl::F32 minimum_persistence) {

		// slot allocation
		lgl::BitSlot m;
		if (!allocateSlot(m, 1))
			return;
		// recursive marking
		m_rec_mark_non_persistent(*m_list.rbegin(), m, (std::numeric_limits<lgl::F32>::max)(), minimum_persistence);
		// pruning the marked sets
		removeMarkedSets(m);
		// slot unalloc
		freeSlot(m);
	}
	//================================================================

	//================================================================
	/// Recursive marking function used in removeNonPersistentSets

	void ScaleSets::m_rec_mark_non_persistent(Set* s, const lgl::BitSlot& m, lgl::F32 scaleOfDisappearance, lgl::F32 minimum_persistence) {
		// mark
		if (scaleOfDisappearance <= minimum_persistence * s->scaleOfAppearance()) {
			s->setMark(m, 1);
		}
		// sod
		if (s->scaleOfAppearance() < scaleOfDisappearance)
			scaleOfDisappearance = s->scaleOfAppearance();
		// recursive call on childrens
		std::vector<Set*>::const_iterator i;
		for (i = s->sons().begin(); i != s->sons().end(); ++i) {
			m_rec_mark_non_persistent(*i, m, scaleOfDisappearance, minimum_persistence);
		}
	}
	//================================================================

	//================================================================
	/// Dumps the structure of the ScaleSets in text format

	void ScaleSets::textDump(std::ostream& o) const {
		if (m_list.size() == 0)
			return;
		m_rec_text_dump(*m_list.rbegin(), o);
	}
	//================================================================

	//================================================================
	/// Dumps the structure of the ScaleSets in text format

	void ScaleSets::m_rec_text_dump(Set* s, std::ostream& o) const {
		o << "(" << s->index() << "," << s->scaleOfAppearance() << ")";
		if (s->sons().size() == 0)
			return;
		o << "[";
		// recursive call on childrens
		std::vector<Set*>::const_iterator i;
		for (i = s->sons().begin(); i != s->sons().end(); ++i) {
			m_rec_text_dump(*i, o);
		}
		o << "]";
	}
	//================================================================

	//================================================================
	// verifies the coherence of the Slot allocation :
	// all unallocated bits should be set to 0

	lgl::BOOL ScaleSets::verifySlots() {
		std::list<Set*>::iterator i;
		for (i = m_list.begin(); i != m_list.end(); ++i) {
			if ((*i)->getMarkVector() & (!m_marker.usedBitsMap()))
				return false;
		}
		return true;
	}
//================================================================

}
