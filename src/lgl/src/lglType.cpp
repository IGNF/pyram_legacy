#include "lgl/lglType.h"


namespace lgl {
	///  Number of basic types
	const int BasicType::Number = 12;
	/*
		const int BasicType::Unknown = 0;
		const int BasicType::I1Code = 1;
		const int BasicType::BoolCode = 2;
		const int BasicType::CharCode = 3;
		const int BasicType::I8Code = 4;
		const int BasicType::UI8Code = 5;
		const int BasicType::I16Code = 6;
		const int BasicType::UI16Code = 7;
		const int BasicType::I32Code = 8;
		const int BasicType::UI32Code = 9;
		const int BasicType::F16Code = 10;
		const int BasicType::F32Code = 11;
		*/
	/// Strings associated with types (codes)
	const char BasicType::m_name[12][32] = { "unknown         ", "bit             ", "boolean         ", "character       ", "8 bits int      ",
			"uns. 8 bits int ", "16 bits int     ", "uns. 16 bits int", "32 bits int     ", "uns. 32 bits int", "16 bits float   ",
			"32 bits float   " };

	/// Lengthes of the types (codes)
	const long BasicType::m_size[12] = { 0, 1, 1, 8, 8, 8, 16, 16, 32, 32, 16, 32 };

}


