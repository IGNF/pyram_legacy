#ifndef _IGN_DATA_JSON_JSONLEXER_H_
#define _IGN_DATA_JSON_JSONLEXER_H_


#include <string>
#include <ignConfig.h>

namespace ign {
	namespace data {
		/// \brief Ce package regroupe les outils permettant de manipuler du JSON (format d'encodage similaire a XML, plus leger)
		/// \see http://www.json.org
		namespace json {

			/// \brief [private]classe de lecture des chaines de caracteres JSON
			class IGN_DATA_API JsonLexer {
			public:
				/// \brief les types d'elements du format JSON
				typedef enum {
					JNull,			//null
					JTrue,			//true
					JFalse,			//false
					JLeftBrace,		//{
					JRightBrace,	//}
					JLeftBracket,	//[
					JRightBracket,	//]
					JColon,			//:
					JComma,			//,
					JString,		//"toto"
					JNumber			//5.0e5
				} TokenType ;

				/// \brief constructeur par defaut
				JsonLexer();
				/// \brief destructeur
				~JsonLexer();


				/// \brief definit la chaine JSON a lire
				void	setString( std::string const& jsonString );

				/// \brief lit le prochain token dans la chaine json
				bool	nextToken( std::string& token, TokenType & tokenType );

				/// \brief renvoie la position en lecture
				size_t	getPos() const;
				/// \brief definit la position en lecture
				void	setPos( size_t const& pos ) ;

				/// \brief renvoie la longueur de la chaine a lire
				size_t	getLength() const;
			private:
				/// \brief la chaine a parser
				std::string	 										_json;
				/// \brief la longueur
				size_t		 										_length;
				/// \brief la position dans la chaine json
				size_t	     										_pos;

				/// \brief lit un nombre
				void  	readNumber( std::string& number );

				/// \brief lit une chaine en interpretant les caracteres json (_pos doit etre sur ")
				bool   	readString( std::string& str );

				/// \brief verifie que l'on est bien positionne sur la chaine passee en parametre
				bool   	checkIsOnString( std::string const& str );
				bool    _isblank( char c );
			};
		}
	}
}

#endif
