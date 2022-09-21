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
 \brief Basic types management
 **/
#ifndef __lglType_h__
#define __lglType_h__
//===========================================================
// namespace	: lgl
// Macros and classes for handling basic types 
//===========================================================
// author		: Laurent Guigues
// history		: 
//	11/07/04		creation
//===========================================================
#include <limits>
#include "lgl/lglString.h"

namespace lgl {

	//===========================================================
	// Type macros
	//===========================================================

	typedef bool I1;                /// 1 bit coded value
	typedef bool BOOL;              /// Boolean
#ifndef FALSE
	const bool FALSE = false;
#endif
#ifndef TRUE
	const bool TRUE = true;
#endif

	typedef char CHAR;              /// Character
	typedef char I8;                /// 8 bits signed integer
	typedef unsigned char UI8;      /// 8 bits unsigned integer
	typedef short I16;              /// 16 bits signed integer
	typedef unsigned short UI16;    /// 16 bits unsigned integer
	typedef int I32;                /// 32 bits signed integer
	typedef unsigned int UI32;      /// 32 bits unsigned integer
	typedef float F32;              /// 32 bits floating number
	typedef double F64;             /// 64 bits floating number

	enum {
		Unknown = 0,
		I1Code = 1,
		BOOLCode = 2,
		CHARCode = 3,
		I8Code = 4,
		UI8Code = 5,
		I16Code = 6,
		UI16Code = 7,
		I32Code = 8,
		UI32Code = 9,
		F32Code = 10,
		F64Code = 11
	};

	typedef int TypeCode;

	/*
		#define Unknown 0
		#define I1Code 1
		#define BOOLCode 2
		#define CHARCode 3
		#define I8Code 4
		#define UI8Code 5
		#define I16Code 6
		#define UI16Code 7
		#define I32Code 8
		#define UI32Code 9
		#define F32Code 10
		#define F64Code 11
		*/

	/*
		const int BasicType::Unknown = 0;
		const int BasicType::I1Code = 1;
		const int BasicType::BOOLCode = 2;
		const int BasicType::CHARCode = 3;
		const int BasicType::I8Code = 4;
		const int BasicType::UI8Code = 5;
		const int BasicType::I16Code = 6;
		const int BasicType::UI16Code = 7;
		const int BasicType::I32Code = 8;
		const int BasicType::UI32Code = 9;
		const int BasicType::F32Code = 10;
		const int BasicType::F64Code = 11;
		*/

	//===========================================================
	/// Classe providing general informations on basic types (in particular associating them with codes).
	class BasicType {
	public:
		///  Number of basic types
		static const int Number;
		/*
			static const TypeCode Unknown;
			static const TypeCode I1Code;
			static const TypeCode BOOLCode;
			static const TypeCode CHARCode;
			static const TypeCode I8Code;
			static const TypeCode UI8Code;
			static const TypeCode I16Code;
			static const TypeCode UI16Code;
			static const TypeCode I32Code;
			static const TypeCode UI32Code;
			static const TypeCode F32Code;
			static const TypeCode F64Code;
			*/

		/// Returns the code of the type
		static TypeCode code() {
			return Unknown;
		}
		/// Returns the code of type I1
		static TypeCode code(I1) {
			return I1Code;
		}
		/// Returns the code of type BOOL
		//  Today BOOL == I1
		//		static TypeCode code(BOOL) { return BOOLCode; }
		/// Returns the code of type CHAR
		static TypeCode code(CHAR) {
			return CHARCode;
		}
		/// Returns the code of type I8
		// Today I8 == CHAR
		// static TypeCode code(I8) { return I8Code; }
		/// Returns the code of type UI8
		static TypeCode code(UI8) {
			return UI8Code;
		}
		/// Returns the code of type SI16
		static TypeCode code(I16) {
			return I16Code;
		}
		/// Returns the code of type UI16
		static TypeCode code(UI16) {
			return UI16Code;
		}
		/// Returns the code of type I32
		static TypeCode code(I32) {
			return I32Code;
		}
		/// Returns the code of type UI32
		static TypeCode code(UI32) {
			return UI32Code;
		}
		/// Returns the code of type F32
		static TypeCode code(F32) {
			return F32Code;
		}
		/// Returns the code of type F64
		static TypeCode code(F64) {
			return F64Code;
		}

		/// Returns the name of the given code
	//	static String name(TypeCode code) {
	//		return String(m_name[code], *wxConvCurrent);
	//	}

		/// Returns the size of the type (bits)
		static long size(TypeCode code) {
			return m_size[code];
		}
	protected:
		/// Strings associated with types (codes)
		static const CHAR m_name[12][32];
		/// Lengthes of the types (codes)
		static const long m_size[12];

	};
//===========================================================

//===========================================================
// Macros switching to the appropriate (usually template) function according to TypeCode
//===========================================================
/// Calls the 1 parameter function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// Based on a switch on TYPECODE.
#define SwitchOnTypeFunctionCall1( TYPECODE, FUNCTION ) \
	switch (TYPECODE) { \
	case lgl::I8Code : FUNCTION ( (lgl::I8)0 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 ); break; \
	case lgl::UI16Code : FUNCTION ( (lgl::UI16)0 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 ); break; \
	case lgl::UI32Code : FUNCTION ( (lgl::UI32)0 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro SwitchOnTypeFunctionCall1"); \
	}
/// Calls the 2 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The second parameter is PAR2
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall2( TYPECODE, FUNCTION, PAR2 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall2"); \
	}
/// Calls the 3 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The following parameters are PAR2,PAR3
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall3( TYPECODE, FUNCTION, PAR2, PAR3 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2, PAR3 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2, PAR3 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2, PAR3 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2, PAR3 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2, PAR3 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2, PAR3 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2, PAR3 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2, PAR3 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall3"); \
	}
/// Calls the 4 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The following parameters are PAR2,PAR3,PAR4
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall4( TYPECODE, FUNCTION, PAR2, PAR3, PAR4 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2, PAR3, PAR4 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2, PAR3, PAR4 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2, PAR3, PAR4 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2, PAR3, PAR4 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2, PAR3, PAR4 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2, PAR3, PAR4 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2, PAR3, PAR4 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2, PAR3, PAR4 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall4"); \
	}
/// Calls the 5 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The following parameters are PAR2,PAR3,PAR4,PAR5
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall5( TYPECODE, FUNCTION, PAR2, PAR3, PAR4, PAR5 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2, PAR3, PAR4, PAR5 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2, PAR3, PAR4, PAR5 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall5"); \
	}
/// Calls the 6 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The following parameters are PAR2,PAR3,PAR4,PAR5,PAR6
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall6( TYPECODE, FUNCTION, PAR2, PAR3, PAR4, PAR5, PAR6 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2, PAR3, PAR4, PAR5, PAR6 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall6"); \
	}
/// Calls the 7 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The following parameters are PAR2,PAR3,PAR4,PAR5,PAR6,PAR7
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall7( TYPECODE, FUNCTION, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall7"); \
	}
/// Calls the 8 parameters function FUNCTION with a first argument of type corresponding to the code TYPECODE.
/// The following parameters are PAR2,PAR3,PAR4,PAR5,PAR6,PAR7,PAR8
/// Based on a switch on TYPECODE.
#define lglSwitchOnTypeFunctionCall8( TYPECODE, FUNCTION, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ) \
	switch (TYPECODE) { \
	case lgl::I8Code	: FUNCTION ( (lgl::I8)0	, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::UI8Code : FUNCTION ( (lgl::UI8)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::I16Code : FUNCTION ( (lgl::I16)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::UI16Code: FUNCTION ( (lgl::UI16)0, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::I32Code : FUNCTION ( (lgl::I32)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::UI32Code: FUNCTION ( (lgl::UI32)0, PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::F32Code : FUNCTION ( (lgl::F32)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	case lgl::F64Code : FUNCTION ( (lgl::F64)0 , PAR2, PAR3, PAR4, PAR5, PAR6, PAR7, PAR8 ); break; \
	default : lglERROR("Unknown TypeCode "<<TYPECODE<<" in macro lglSwitchOnTypeFunctionCall8"); \
	}
/*
	//===========================================================
	/// General information on type T
	template <class T>
	class Type : public BasicType
	{
	public :
	static virtual long size() { return sizeof(T); }
	};
	//===========================================================

	//===========================================================
	// Macro to perform specializations on basic types
	#define LGL_TYPE_SPECIALIZE(A,B) \
/// Type class specialization on A 
	template<> class Type< A > {
	public: \
/// Returns the code of the Type
	static virtual int code() { return B ; } \
/// Returns the name of the Type 
	static virtual CHAR* name() { return m_name[ B ]; } \
}; 
	//===========================================================

	//===========================================================
	// Type specializations
	LGL_TYPE_SPECIALIZE(I1,I1Code)
	LGL_TYPE_SPECIALIZE(BOOL,BOOLCode)
	LGL_TYPE_SPECIALIZE(CHAR,CHARCode)
	LGL_TYPE_SPECIALIZE(I8,I8Code)
	LGL_TYPE_SPECIALIZE(SI8,SI8Code)
	LGL_TYPE_SPECIALIZE(I16,I16Code)
	LGL_TYPE_SPECIALIZE(SI16,SI16Code)
	LGL_TYPE_SPECIALIZE(I32,I32Code)
	LGL_TYPE_SPECIALIZE(UI32,SI32Code)
	LGL_TYPE_SPECIALIZE(F32,F32Code)
	LGL_TYPE_SPECIALIZE(F64,F64Code)
	//===========================================================


	//===========================================================
	/// Macro creating a class called NAME
	/// which behaves as a type TYPE basic type.
	/// Used to create methods of functionally identical argument list
	/// however who have different signatures, hence whose
	/// behaviour depends on the semantic of its arguments.
	#define LGL_DEFINE_BASIC_TYPE_EMBEDDING_CLASS( NAME, TYPE ) \
class NAME { \
public : \
NAME( TYPE t=0 ) : m_val(t) {} \
NAME( const NAME& n ) : m_val(n.m_val) {} \
~NAME() {} \
NAME& operator = ( const NAME& n ) { m_val = n.m_val; return *this; } \
NAME& operator = ( TYPE n ) { m_val = n; return *this; } \
TYPE operator() () { return m_val; } \
protected : \
TYPE m_val; \
};
	//===========================================================
	*/
}
//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
