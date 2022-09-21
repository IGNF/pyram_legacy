#ifndef _IGN_DATA_HELPER_H_
#define _IGN_DATA_HELPER_H_

#include <string>
#include <sstream>

#include <ignConfig.h>

//#include <ctype.h>
//#include <algorithm>


namespace ign {
	namespace data {
		/// \brief ensemble de fonction utilitaire pour la manipulation du type dynamique
		class IGN_DATA_API Helper {
		public:
			/// \brief ajoute des quotes devant les caracteres speciaux json (excepte \u)
			/// \todo migrer dans tools et prendre different type d'echappement en option
			static std::string quote( std::string const& str ) ;
			/// \brief supprime les quotes devant les caracteres  speciaux json (excepte \u), les caracteres echappe non reconnu
			///			sont ignore.
			/// \todo migrer dans tools et prendre different type d'echappement en option
			static std::string unquote( std::string const& str ) ;

			/// \brief conversion d'une chaine en une valeur (int, float, long, double, bool)
			/// \return vrai si la conversion se realise sans probleme
			/// \example main_data_helper.cpp
			template < typename T >
			static bool StringTo( std::string const& value, T& out )
			{
				out = T();
				std::istringstream iss(value);
				return ( iss >> out ) && iss.eof();
			}


			/// \brief conversion en chaine de caractere d'une valeur (int, float, long, double)
			template < typename T >
			static std::string ToString( T const& value )
			{
				std::ostringstream oss;
				oss << value ;
				return oss.str();
			}

			/// \brief renvoie un message indiquant un downcast impossible
			static std::string GetMessageBadCast( std::string const& from, std::string const& to );
			/// \brief renvoie un message indiquant une mauvaise conversion de type
			static std::string GetMessageBadConvert( std::string const& from, std::string const & to, std::string const& value = "" );
			/// \brief renvoie un message indiquant qu'un attribut n'existe pas
			static std::string GetMessageNoAttribute( std::string const& attributeName );
			/// \brief renvoie un message indiquant que l'attribut existe
			static std::string GetMessageAttributeAlreadyExists( std::string const& attributeName );

		};

		/// \brief specialisation pour les booleens
		template <>
		IGN_DATA_API bool Helper::StringTo( std::string const& value, bool& out );


		template <>
		IGN_DATA_API std::string Helper::ToString( bool const& value );
	}
}

#endif

