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
 \brief A class handling the basic machine properties (e.g. endianness) and machine-dependent operations.
 */
#ifndef __lglMachine_h__
#define __lglMachine_h__
//===========================================================
// namespace	: lgl
// classe		: Machine
// A class handling the basic machine properties (e.g. endianness) and machine-dependent operations. 
//===========================================================
// author		: Laurent Guigues
// history		: 
//	02/09/04	: creation (inspired the matis/noyau lib systeme class) 
//===========================================================
#include "lgl/lglGeneral.h"

namespace lgl {

	/// \brief Handles the basic machine properties (e.g. endianness, bus size) and machine-dependent operations.
	class Machine {
	public:
		/// Type of machine */
		//@{
		/// Determines the type of Endians (little=Intel like / big=Sun like)
		inline static void testEndians();
		/// Is the machine an Intel like processor ? (equivalent to littleEndians)
		inline static bool Intel() {
			return m_Intel;
		}
		/// Is the machine an Intel like processor ?
		inline static bool littleEndians() {
			return m_Intel;
		}
		/// Is the machine a Sun like processor ? (== bigEndians)
		inline static bool Sun() {
			return !m_Intel;
		}
		/// Is the machine a Sun like processor ?
		inline static bool bigEndians() {
			return !m_Intel;
		}
		///
		inline static void changeEndians() {
			m_Intel = !m_Intel;
		}
		inline static void simulateIntel() {
			m_Intel = true;
		}
		inline static void simulateSun() {
			m_Intel = true;
		}

		///
		inline static void testBusSize();
		///
		inline static bool bits32() {
			return m_32;
		}
		///
		inline static bool bits64() {
			return m_64;
		}
		//@}

		///@Little/big endians swap methods
		///@{
		///
		inline static void swapEndians(bool &) {
		}
		///
		inline static void swapEndians(char &) {
		}
		///
		inline static void swapEndians(signed char &) {
		}
		///
		inline static void swapEndians(unsigned char &) {
		}
		///
		inline static void swapEndians(signed short &);
		///
		inline static void swapEndians(unsigned short &);
		///
		inline static void swapEndians(signed int &);
		///
		inline static void swapEndians(unsigned int &);
		///
		inline static void swapEndians(signed long &);
		///
		inline static void swapEndians(unsigned long &);
		///
		inline static void swapEndians(float &);
		///
		inline static void swapEndians(double &);
		///
		inline static void swapEndians(bool *, long) {
		}
		///
		inline static void swapEndians(char *, long) {
		}
		///
		inline static void swapEndians(signed char *, long) {
		}
		///
		inline static void swapEndians(unsigned char *, long) {
		}
		///
		inline static void swapEndians(signed short *, long);
		///
		inline static void swapEndians(unsigned short *, long);
		///
		inline static void swapEndians(signed int *, long);
		///
		inline static void swapEndians(unsigned int *, long);
		///
		inline static void swapEndians(signed long *, long);
		///
		inline static void swapEndians(unsigned long *, long);
		///
		inline static void swapEndians(float *, long);
		///
		inline static void swapEndians(double *, long);

		///@}
	protected:
		static bool m_Intel;
		static bool m_32;
		static bool m_64;
	};
	//============================================================================
	// EO class Machine
	//============================================================================

	//============================================================================
	// Inline methods implementation
	//============================================================================
	void Machine::testEndians() {
		int n = 1;
		char * p = (char *) &n;
		m_Intel = (*p == (char) 1);
	}

	void Machine::testBusSize() {
		if (sizeof(void *) == 4) {
			m_32 = true;
			m_64 = false;
		} else if (sizeof(void *) == 8) {
			m_32 = false;
			m_64 = true;
			std::cout << "A 64 bits machine ??? Waooo !!!" << std::endl;
		} else
			std::cout << "lgl::Machine::testBusSize : neither a 32 bits nor a 64 bits machine... Is it a MD12 of 1968 ?" << std::endl;
	}

#define ENDIAN_INVERT_4BYTES(a) ((((unsigned int)a) >> 24) | ((a) << 24) | (((a) & 0x00FF0000) >> 8) | (((a) & 0x0000FF00) << 8))

	void Machine::swapEndians(unsigned short & d) {
		d = (d >> 8) | (d << 8);
	}
	void Machine::swapEndians(short & d) {
		d = (((unsigned short) d) >> 8) | (d << 8);
	}
	void Machine::swapEndians(unsigned int & d) {
		d = ENDIAN_INVERT_4BYTES(d);
	}
	void Machine::swapEndians(int & d) {
		d = ENDIAN_INVERT_4BYTES(d);
	}
	void Machine::swapEndians(unsigned long & d) {
		swapEndians(*((int*) &d));
	}
	void Machine::swapEndians(long & d) {
		swapEndians(*((int*) &d));
	}
	void Machine::swapEndians(float & d) {
		swapEndians(*((int*) &d));
	}
	void Machine::swapEndians(double & d) {
		int tmp = *((int*) &d);
		*((int*) &d) = ENDIAN_INVERT_4BYTES(*(((int*)&d)+1));
		*(((int*) &d) + 1) = ENDIAN_INVERT_4BYTES(tmp);
	}

	void Machine::swapEndians(unsigned short* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(short* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(unsigned int* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(int* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(unsigned long* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(long* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(float* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}
	void Machine::swapEndians(double* d, long n) {
		for (long i = 0; i < n; i++, ++d)
			swapEndians(*d);
	}

}
//=================================================================================
// EO namespace lgl
//=================================================================================

//=================================================================================
// EOF
//=================================================================================
#endif
