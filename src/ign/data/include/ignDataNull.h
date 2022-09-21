#ifndef _IGN_DATA_NULL_H_
#define _IGN_DATA_NULL_H_

#include <ignConfig.h>

#include <ignDataValue.h>


namespace ign {
	namespace data {

		/// \brief represente une valeur nulle
		class IGN_DATA_API Null : public Value {
		public:
			/// \brief constructeur par defaut
			Null();
			/// \brief constructeur par recopie
			Null( Null const& other );
			/// \brief operateur d'affectation
			Null& operator = ( Null const& other );
			/// \brief destructeur
			virtual ~Null();

			//-- [ign::data::Value]
			virtual bool		isNull() const ;

			//-- [ign::data::Value]
			virtual std::string toString() const ;
			//-- [ign::data::Value]
			virtual Value*			clone()   const;

			//-- [ign::data::Value]
			virtual bool			equals( Value const& other, bool strict = true ) const ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;

		protected:
			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
