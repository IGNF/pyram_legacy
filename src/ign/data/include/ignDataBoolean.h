#ifndef _IGN_DATA_BOOLEAN_H_
#define _IGN_DATA_BOOLEAN_H_

#include <ignConfig.h>

#include <ignDataValue.h>

namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API Boolean : public Value {
		public:
			/// \brief constructeur par defaut (valeur = false)
			Boolean();
			/// \brief constructeur par recopie
			Boolean( Boolean const& other );
			/// \brief operateur d'affectation
			Boolean& operator = ( Boolean const& other );
			/// \brief destructeur
			virtual ~Boolean();

			/// \brief constructeur a partir d'un booleen
			Boolean( bool const& value );
			/// \brief constructeur a partir d'un booleen
			Boolean& operator = ( bool const& value ) ;

			/// \brief implicit cast
			inline operator bool () const { return _value; }

			//-- [ign::data::Value]
			virtual std::string toString() const ;
			//-- [ign::data::Value]
			virtual bool		toBoolean() const ;
			//-- [ign::data::Value]
			virtual int64_t		toLong() const ;

			//-- [ign::data::Value]
			virtual Value*		clone() const;

			//-- [ign::data::Value]
			virtual bool	equals( Value const& other, bool strict = true ) const ;

			/// \brief recupere la valeur encapsulee
			bool const& 		value() const ;
			/// \brief recupere la valeur encapsulee
			bool & 				value() ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			bool _value;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}



#endif
