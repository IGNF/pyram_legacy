#ifndef _IGN_DATA_ARRAY_H_
#define _IGN_DATA_ARRAY_H_

#include <vector>
#include <sstream>

#include <ignConfig.h>

#include <ignDataVariant.h>


namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API Array : public Value {
		public:
			/// \brief constructeur par defaut
			Array();
			/// \brief constructeur par recopie
			Array( Array const& other );
			/// \brief operateur d'affectation
			Array& operator = ( Array const& other );
			/// \brief destructeur
			virtual ~Array();

			/// \brief renvoie la taille
			size_t 				numItems() const ;

			/// \brief renvoie la valeur encapsulee
			Variant const& 		item( size_t const& i ) const ;
			/// \brief renvoie la valeur encapsulee
			Variant & 			item( size_t const& i ) ;

			/// \brief definit la ieme valeur
			void				setItem( size_t const& i, Value const& item );
			/// \brief ajoute un element
			void				addItem( Value const& value ) ;
			/// \brief ajoute un element en prenant la responsabilite
			void				addItem( Value* value );
			/// \brief supprime un element
			void				removeItem( size_t const& i );
			/// \brief vide le vecteur
			void				clear() ;

			/// \brief renvoie le i-ieme element
			Variant &	 	 	operator [] ( size_t const& i ) ;
			/// \brief renvoie le i-ieme element
			Variant const &	 	operator [] ( size_t const& i ) const ;


			//-- [ign::data::Value]
			virtual std::string  	toString() const ;
			//-- [ign::data::Value]
			virtual Value*			clone()   const;

			//-- [ign::data::Value]
			virtual bool	equals( Value const& other, bool strict = true ) const ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			/// \brief les elements du tableau
			std::vector< Variant >	_values;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
