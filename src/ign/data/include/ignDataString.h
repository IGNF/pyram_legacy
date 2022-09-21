#ifndef _IGN_DATA_STRING_H_
#define _IGN_DATA_STRING_H_

#include <ignConfig.h>

#include <ignDataValue.h>


namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API String : public Value {
		public:
			/// \brief constructeur par defaut
			String();
			/// \brief constructeur par recopie
			String( String const& other );
			/// \brief operateur d'affectation
			String& operator = ( String const& other );
			/// \brief destructeur
			virtual ~String();

			/// \brief constructeur a partir d'une chaine C
			String( const char* str );
			/// \brief constructeur a partir d'un booleen
			String( std::string const& value );
			/// \brief constructeur a partir d'un booleen
			String& operator = ( std::string const& value ) ;
			/// \brief affectation d'une chaine C
			String& operator = ( const char* str );

			/// \brief implicit cast
			inline operator std::string () const { return _value; }

			//-- [ign::data::Value]
			virtual std::string toString() const ;
			/// \brief convertit la chaine de caracteres en booleen
			///
			/// Supporte les differents formats suivant ( "t"|"f", "0"|"1", "true"|"false" )
			/// Insensible a la casse
			virtual bool		    toBoolean() const ;
			//-- [ign::data::Value]
			virtual int			    toInteger() const ;
			//-- [ign::data::Value]
            virtual int64_t		    toLong() const ;
            //-- [ign::data::Value]
            virtual double          toDouble() const ;
            //-- [ign::data::Value]
            virtual float           toFloat() const ;

			//-- [ign::data::Value]
			virtual Value*			clone()   const;

			//-- [ign::data::Value]
			virtual bool			equals( Value const& other, bool strict = true ) const ;

			/// \brief recupere la valeur encapsulee
			std::string const& 		value() const ;
			/// \brief recupere la valeur encapsulee
			std::string & 			value() ;

			//-- [ign::data::Value]
			virtual Value::ValueType const&   getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const&        getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			std::string _value;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
