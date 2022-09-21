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
 \brief lGl Strings and Filenames (today wxString/wxFilename)
 **/
#ifndef __lglString_h__
#define __lglString_h__
//===========================================================
// namespace	: lgl
//===========================================================
// author		: Laurent Guigues
// class		: String
// history		: 
//	26/07/04		creation
//===========================================================

//=============================================================
// lgl::Filename is based on wxFileName
// lgl::String is based on wxString
#if defined(__APPLE__)|| defined(__MACH__) || defined(__MAC__)
//linking problem on MacOS / clang / cxx11 with wxString
//#define LGL_WITH_BOOST
//resolu avec l'ajout d'une defintion de preprocession: _WCHAR_H_CPLUSPLUS_98_CONFORMANCE_
#endif

#include <memory>
#include <string>
#include <stdexcept>
#include <boost/filesystem.hpp>
//=============================================================

namespace lgl {
	typedef boost::filesystem::path Filename;
	typedef std::string String;

	template<typename ... Args>
	std::string string_format( const std::string& format, Args ... args )
	{
		int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
		if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
		auto size = static_cast<size_t>( size_s );
		std::unique_ptr<char[]> buf( new char[ size ] );
		std::snprintf( buf.get(), size, format.c_str(), args ... );
		return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
	}
}

//===========================================================
// EO namespace lgl
//===========================================================

//===========================================================
// EOF
//===========================================================
#endif
