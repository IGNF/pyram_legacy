
#include <ignNumeric.h>

namespace ign 
{
	namespace numeric
	{
		
		///
		///
		///
		bool IsNumeric(const std::string& number)
		{
			char* end = 0;
			std::strtod(number.c_str(), &end);
			
			return end != 0 && *end == 0;
		}
		
		///
		///
		///		
		bool IsInteger(const std::string& number)
		{
			return IsNumeric(number.c_str()) && std::strchr(number.c_str(), '.') == 0;
		}
		
	}
}