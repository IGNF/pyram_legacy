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
 \brief Abstract arrays (0,1,2 or 3 dimensions)
 */
#ifndef __lglArray_h__
#define __lglArray_h__
//===========================================================
// namespace	: lgl
// classe		: Array<T>
// General array definition (0,1,2 or 3 dimensions)
//===========================================================
// author		: Laurent Guigues
// history		: 
//	28/07/04	: creation
//===========================================================
#include <ostream>

#include "lgl/lglGeneral.h"
#include "lgl/lglPoint4D.h"
#include "lgl/lglVector.h"

namespace lgl {

	//==========================================================================================
	/// Abstract array definition (0,1,2 or 3 dimensions)
	class Array {
	public:
		/// ctor
		Array() /*: m_dimension(0)*/{
		}

		virtual ~Array() {
		}
		/// Returns the dimension of the array
		virtual DimensionType dimension() const = 0; //{ return m_dimension; }
		/// Returns the size of the Array as a Point4D<int> of the same dimension than the Array
		/// whose values correspond to the size of the Array in each dimension
		IPoint4D size() const {
			return m_size;
		}
		I32 size(I32 i) const {
			return m_size(i);
		}

	protected:
		/// dimension
		//DimensionType		m_dimension;
		/// size
		IPoint4D m_size;
	};
//==========================================================================================
// EO class Array
//==========================================================================================

}
//==========================================================================================
// EO namespace lgl
//==========================================================================================

//==========================================================================================
// EOF
//==========================================================================================
#endif

