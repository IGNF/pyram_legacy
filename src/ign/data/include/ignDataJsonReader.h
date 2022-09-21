#ifndef _IGN_DATA_JSON_JSONREADER_H_
#define _IGN_DATA_JSON_JSONREADER_H_

#include <ignConfig.h>

#include <ignDataVariant.h>
#include <ignDataJsonLexer.h>

namespace ign {
	namespace data {
		namespace json {

			/// \brief [private]classe de lecture des chaines de caracteres JSON
			class IGN_DATA_API JsonReader {
			public:
				/// \brief classe de lecture d'une chaine json
				JsonReader();
				/// \brief destructeur
				~JsonReader();

				/// \brief lecture d'une chaine json
				void	read( std::string const& json, Variant& value );

			private:
				JsonLexer	_lexer;

				/// \brief lecture d'un element json
				void	readValue( ValuePtr& value );
				/// \brief lit un nom json dans le lexer
				void	decodeNumber( std::string const& token, ValuePtr& value );
				/// \brief lit un tableau json dans le lexer
				void	readArray( ValuePtr & value );
				/// \brief lit un objet json dans le lexer
				void	readObject( ValuePtr & value );
			};

			/// \brief lit une valeur json dans une valeur
			/// \throw ign::Exception en cas de probleme lors de la lecture
			IGN_DATA_API void		ReadJson( std::string const & json, Variant& value );
			/// \brief lit une valeur json et renvoie le variant correspondant
			IGN_DATA_API Variant	ReadJson( std::string const & json );
			/// \brief lit un fichier json
			IGN_DATA_API void 	ReadJsonFile( std::string const& filename, Variant & value );
		}
	}
}


#endif
