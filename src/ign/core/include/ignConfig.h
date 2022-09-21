#ifndef _IGN_CONFIG_H_
#define _IGN_CONFIG_H_


/// \brief placer a vrai pour voir actif les controles d'acces
#define IGN_SAFE_MODE 1

//--- gestion de l'environnement de compilation

// #define IGN_WITH_SSE

#define IGN_WITH_CXX11

#ifdef IGN_WITH_CXX11
#include <memory>
#include <array>
namespace ign
{
	template <class T, class Deleter = std::default_delete<T> >
	using unique_ptr = std::unique_ptr<T, Deleter>;

	template <class T>
	using shared_ptr = std::shared_ptr<T>;

	template <typename ValueType, size_t N>
	using array = std::array<ValueType, N>;

	template< class T, class U >
	shared_ptr<T>  dynamic_pointer_cast( const shared_ptr<U>& r )
	{
		return std::dynamic_pointer_cast<T, U>( r );
		
	}
	template< class T, class U >
	shared_ptr<T>  static_pointer_cast( const shared_ptr<U>& r )
	{
		return std::static_pointer_cast<T, U>( r );
		
	}
}
#else
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
namespace ign
{
	template <class T>
	using unique_ptr = std::auto_ptr<T>;

	template <class T>
	using shared_ptr = boost::shared_ptr<T>;
	 
	template <typename ValueType, size_t N>
	using array = boost::array<ValueType, N>;

	template< class T, class U >
	shared_ptr<T>  dynamic_pointer_cast( const shared_ptr<U>& r )
	{
		return boost::dynamic_pointer_cast<T, U>( r );
		
	}

	template< class T, class U >
	shared_ptr<T>  static_pointer_cast( const shared_ptr<U>& r )
	{
		return boost::static_pointer_cast<T, U>( r );
		
	}
}
#endif

//--- gestion des dependances facultatives

/// \brief indique si la dependance a PROJ est activee
// #cmakedefine IGN_WITH_PROJ

///gestion des lib images

/// \brief indique si la dependance a GDAL est activee
// #cmakedefine IGN_WITH_GDAL


/// \brief [private] on s'autorise une perte de 6 decimales lors des comparaison a 0
/// IGN_NUMERIC_LIMIT_EPSILON_FACTOR*std::numeric_limits< T >::epsilon()
/// (hesitation avec une precision absolue de 1.0e-10, plus robuste, mais plus limitant)
#define IGN_NUMERIC_LIMIT_EPSILON_FACTOR 1.0e6

#include <boost/cstdint.hpp>
/// \todo passer dans plateform et placer dans ign::
typedef boost::int8_t	 	int8_t;
typedef boost::uint8_t		uint8_t;
typedef boost::int16_t	 	int16_t;
typedef boost::uint16_t		uint16_t;
typedef boost::int32_t	 	int32_t;
typedef boost::uint32_t		uint32_t;

#ifndef IGN_ARCH_64
typedef boost::int64_t	 	int64_t;
typedef boost::uint64_t		uint64_t;
#endif


//----- DLL MANAGEMENT ------------------

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
     #define IGN_API_IMPORT __declspec(dllimport)
     #define IGN_API_EXPORT __declspec(dllexport)

     #define IGN_API_TEMPLATE_EXPORT extern
     #define IGN_API_TEMPLATE_IMPORT
#else
     #define IGN_API_IMPORT
     #define IGN_API_EXPORT
     #define IGN_API_TEMPLATE_EXPORT
     #define IGN_API_TEMPLATE_IMPORT
#endif

//-- temporary for windows management (replacement to IGN_COMMON_API, IGN_TVV_API, IGN_TVI_API)
#define IGN_CORE_API
#define IGN_DATA_API
#define IGN_TRANSFORM_API
#define IGN_FILESYSTEM_API
#define IGN_IMAGE_API
#define IGN_MATH_API
#define IGN_NUMERIC_API
#define IGN_SHAPEFILE_API
#define IGN_LOG_API

#endif


