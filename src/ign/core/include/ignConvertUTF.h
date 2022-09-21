/* -*-c++-*- OpenSceneGraph - Copyright (C) 2008 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#ifndef IGN_TOOLS_OSGDB_CONVERTUTF
#define IGN_TOOLS_OSGDB_CONVERTUTF


#include <string>

#if defined(__CYGWIN__) || defined(ANDROID)
namespace std
{
typedef basic_string<wchar_t> wstring;
}
#endif

// Sukender: Namespace is intenionnally NOT osgDB, to avoid duplicate symbols.

namespace ign
{

extern std::string convertUTF16toUTF8(const wchar_t* source, unsigned sourceLength);
extern std::wstring convertUTF8toUTF16(const char* source, unsigned sourceLength);

extern std::string convertUTF16toUTF8(const std::wstring& s);
extern std::string convertUTF16toUTF8(const wchar_t* s);

extern std::wstring convertUTF8toUTF16(const std::string& s);
extern std::wstring convertUTF8toUTF16(const char* s);

extern std::string convertStringFromCurrentCodePageToUTF8(const char* source, unsigned sourceLength);
extern std::string convertStringFromUTF8toCurrentCodePage(const char* source, unsigned sourceLength);

extern std::string convertStringFromCurrentCodePageToUTF8(const std::string& s);
extern std::string convertStringFromCurrentCodePageToUTF8(const char* s);

extern std::string convertStringFromUTF8toCurrentCodePage(const std::string& s);
extern std::string convertStringFromUTF8toCurrentCodePage(const char* s);

}

#endif
