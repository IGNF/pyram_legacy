#ifndef _IGN_DATA_INTEGER_H_
#define _IGN_DATA_INTEGER_H_

#include <ignConfig.h>
#include <ignDataValue.h>

#include <sstream>

namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API Integer : public Value {
		public:
			/// \brief constructeur par defaut
			Integer();
			/// \brief constructeur par recopie
			Integer( Integer const& other );
			/// \brief operateur d'affectation
			Integer& operator = ( Integer const& other );
			/// \brief destructeur
			virtual ~Integer();

			/// \brief constructeur a partir d'un booleen
			Integer( int const& value );
			/// \brief constructeur a partir d'un booleen
			Integer& operator = ( int const& value ) ;

			/// \brief implicit cast
			inline operator int () const { return _value; }

			//-- [ign::data::Value]
			virtual std::string toString() const ;
			/// \brief convertit en booleen (true si valeur != 0 )
			virtual bool        toBoolean() const ;
			//-- [ign::data::Value]
			virtual int         toInteger() const ;
			//-- [ign::data::Value]
			virtual int64_t	    toLong() const ;
			//-- [ign::data::Value]
			virtual double      toDouble() const ;
            //-- [ign::data::Value]
            virtual float       toFloat() const ;
            
			//-- [ign::data::Value]
			virtual Value*		clone()   const;

			//-- [ign::data::Value]
			virtual bool		equals( Value const& other, bool strict = true ) const ;

			/// \brief recupere la valeur encapsulee
			int const& 		value() const ;
			/// \brief recupere la valeur encapsulee
			int & 			value() ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const&        getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			int _value;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
