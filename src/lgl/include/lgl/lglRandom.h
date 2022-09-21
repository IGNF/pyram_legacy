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
 \brief Pseudo-random numbers generators
 **/
#ifndef __lglRandom_h__
#define __lglRandom_h__
//===========================================================
// namespace	: lgl
//  pseudo random numbers generation classes 
//  adapted from the numerical recipes in C
//===========================================================
// author		: Laurent Guigues
// history		: 
//	13/07/04		creation
//===========================================================
#include "lgl/lglGeneral.h"

namespace lgl {

	//==================================================================
	/** Pseudo-random integers generator */
	class RandomInteger {
	public:

		/** ctor. seed==-1 does clock initilization. */
		RandomInteger(I32 seed = -1, I32 inferior_bound = 0, I32 superior_bound = 1);
		~RandomInteger() {
		}

		/** return a pseudo random number */
		I32 get();
		/** sets the generartion interval */
		void setBounds(I32 inferior_bound, I32 superior_bound) {
			p = inferior_bound;
			q = superior_bound - inferior_bound + 1;
		}
		void setSeed(I32 seed) {
			if (seed < 0)
				init_on_clock(seed);
			else
				s = seed;
			iff = 0;
		}

	private:
		void init_on_clock(I32);
		I32 s;    // seed
		I32 p, q;  // bounds {p,...,p+q-1}
		I8 iff;
		UI32 maxran, v[97], y;
	};
	//==================================================================

	//==================================================================
	/** Pseudo-random float generator */
	class RandomFloat {
	public:

		/** ctor. seed==-1 does clock initilization. */
		RandomFloat(I32 seed = -1, F32 inferior_bound = 0, F32 superior_bound = 1);
		~RandomFloat() {
		}

		/** return a pseudo random number */
		F32 get();
		/** sets the generartion interval */
		void setBounds(F32 inferior_bound, F32 superior_bound) {
			p = inferior_bound;
			q = superior_bound - inferior_bound + 1;
		}
		void setSeed(I32 seed) {
			if (seed < 0)
				init_on_clock(seed);
			else
				s = seed;
			iff = 0;
		}

	private:
		void init_on_clock(I32);
		I32 s;    // seed
		F32 p, q;  // bounds [p,p+q]
		I8 iff;
		UI32 maxran, v[97], y;
	};
//==================================================================

}

//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
