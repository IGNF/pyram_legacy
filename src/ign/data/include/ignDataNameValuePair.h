#ifndef _IGN_DATA_NAMEVALUEPAIR_H_
#define _IGN_DATA_NAMEVALUEPAIR_H_

#include <ignConfig.h>
#include <ignDataVariant.h>

namespace ign {
	namespace data {

		/// \brief Conteneur pour un attribut d'objet. Cette classe doit etre utilisee seulement dans un objet
		/// \warning le constructeur par recopie provoque une copie profonde de la valeur, dans un std::vector, Value*
		///				changera d'adresse apres l'ajout.
		class IGN_DATA_API NameValuePair {
		public:
			/// \brief constructeur par defaut
			NameValuePair();
			/// \brief constructeur a l'aide d'un nom et de la valeur
			NameValuePair( std::string const& name, Value const& value );
			/// \brief constructeur a l'aide d'un nom prenant la responsabilite de la valeur
			NameValuePair( std::string const& name, Value* value );
			/// \brief constructeur par recopie
			NameValuePair( NameValuePair const & other );
			/// \brief operateur d'affectation
			NameValuePair& operator = ( NameValuePair const& other );
			/// \brief destructeur
			~NameValuePair();

			/// \brief indique si la valeur est nulle
			bool				isNull() const ;
			/// \brief rend la valeur nulle
			void				setToNull() ;

			/// \brief renvoie le nom de l'attribut
			std::string const& 	name() const ;
			/// \brief renvoie le nom de l'attribut
			std::string & 		name() ;

			/// \brief renvoie la valeur de l'attribut
			/// \throw exception si isNull()
			Variant const& 		value() const ;
			/// \brief renvoie la valeur de l'attribut
			/// \throw exception si isNull()
			Variant & 			value() ;
			/// \brief definit la valeur de l'attribut, en clonant le parametre
			void				setValue( Value const& value ) ;
			/// \brief definit la valeur de l'attribut, prend la responsabilite du pointeur
			void				setValue( Value* value ) ;

		private:
			/// \brief le nom de l'attribut
			std::string 			_name;
			/// \brief la valeur de l'attribut
			Variant					_value;
		};
	}
}

#endif

