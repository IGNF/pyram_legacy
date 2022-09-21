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
/*! \file 
 \brief Abstract collections of objects which can be parsed sequentially (const iteration)
 */
#ifndef __lglCollection_h__
#define __lglCollection_h__
//===========================================================
// namespace	: lgl
// classe		: Collection
// Abstract definition of a collection of objects 
// which can be parsed sequentially (const iteration)
//===========================================================
// author		: Laurent Guigues
// history		: 
//	29/07/04	: creation
//===========================================================

namespace lgl {

	//===========================================================
	/// \brief Abstract definition of a Collection of objects which can be parsed sequentially (const auto-iteration).
	///
	/// Typical auto-iteration of a Collection : <C> for (c.start();!c.stop();++c) { ... (*c) ... } <\C>
	template<class T>
	class Collection {
	public:
		/// Ctor
		Collection() {
		}
		/// Virtual dtor
		virtual ~Collection() {
		}

		/// Restarts the iteration to the first object
		virtual void start() {
		}
		/// Returns true when the iteration is past the end
		virtual BOOL stop() {
			return true;
		}
		/// Returns the current object
		virtual const T& operator *() const = 0;
		/// Steps to the next object of the collection
		virtual void operator++() {
		}
	};
//===========================================================

}
//=================================================================================
// EO namespace lgl
//=================================================================================

//=================================================================================
// EOF
//=================================================================================
#endif
