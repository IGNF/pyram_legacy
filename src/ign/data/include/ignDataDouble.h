#ifndef _IGN_DATA_DOUBLE_H_
#define _IGN_DATA_DOUBLE_H_
#include <sstream>

#include <ignConfig.h>

#include <ignDataValue.h>


namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API Double : public Value {
		public:
			/// \brief constructeur par defaut
			Double();
			/// \brief constructeur par recopie
			Double( Double const& other );
			/// \brief operateur d'affectation
			Double& operator = ( Double const& other );
			/// \brief destructeur
			virtual ~Double();

			/// \brief constructeur a partir d'un booleen
			Double( double const& value );
			/// \brief constructeur a partir d'un booleen
			Double& operator = ( double const& value ) ;

			/// \brief implicit cast
			inline operator double () const { return _value; }

			/// \brief convertit le double en chaine de caracteres
			/// \warning ne precise aucun format particulier
			virtual std::string toString() const ;
			/// \brief arrondit a la valeur entiere la plus proche
			virtual int64_t		toLong() const ;
			//-- [ign::data::Value]
			virtual double		toDouble() const ;
            //-- [ign::data::Value]
            virtual float       toFloat() const ;
            
			//-- [ign::data::Value]
			virtual Value*		clone()   const ;

			//-- [ign::data::Value]
			virtual bool		equals( Value const& other, bool strict = true ) const ;

			/// \brief recupere la valeur encapsulee
			double const& 		value() const ;
			/// \brief recupere la valeur encapsulee
			double & 			value() ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			double _value;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
