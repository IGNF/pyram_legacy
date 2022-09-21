/*
 *  types.cpp
 *  ign-all
 *
 *  Created by Nicolas Bellaiche on 29/12/10.
 *  Copyright 2010 Institut Geographique National - France. All rights reserved.
 *
 */
#include <ignNumericTypes.h>

//using namespace ign;
//using namespace ign::numeric;

size_t  ign::numeric::TypeSize(NumericType typeIO)
{
	switch (typeIO) {
		case numeric::eOther:
			return 0;
		case numeric::eUnsignedChar:
			return sizeof(unsigned char);
		case numeric::eSignedShort:
			return sizeof(signed short);
		case numeric::eUnsignedShort:
			return sizeof(unsigned short);
		case numeric::eSignedInt:
			return sizeof(signed int);
		case numeric::eUnsignedInt:
			return sizeof(unsigned int);
		case numeric::eFloat:
			return sizeof(float);
		case numeric::eBool:
			return 1;
	}
	return 0;
}

