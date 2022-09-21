#ifndef _IGN_NUMERIC_TYPES_H_
#define _IGN_NUMERIC_TYPES_H_

#include <cctype>
#include <cstddef>

namespace ign
{
    namespace numeric
    {
		
		/** Type numerique*/
		enum NumericType
		{
			eUnsignedChar = 0,
			eSignedShort = 1,
			eUnsignedShort = 2,
			eSignedInt = 3,
			eUnsignedInt = 4,
			eFloat = 5,
			eBool = 6,
			eOther = 7
		};
		
		size_t  TypeSize(NumericType typeIO);
	}
}
		
#endif
