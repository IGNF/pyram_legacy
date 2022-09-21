#ifndef _IGN_DATA_FLOAT_H_
#define _IGN_DATA_FLOAT_H_
#include <sstream>

#include <ignConfig.h>

#include <ignDataValue.h>


namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API Float : public Value {
		public:
			/// \brief constructeur par defaut
			Float();
			/// \brief constructeur par recopie
			Float( Float const& other );
			/// \brief operateur d'affectation
			Float& operator = ( Float const& other );
			/// \brief destructeur
			virtual ~Float();

			/// \brief constructeur a partir d'un booleen
			Float( float const& value );
			/// \brief constructeur a partir d'un booleen
			Float& operator = ( float const& value ) ;

			/// \brief implicit cast
			inline operator float () const { return _value; }

			/// \brief convertit le double en chaine de caracteres
			/// \warning ne precise aucun format particulier
			virtual std::string toString() const ;
			/// \brief arrondit a la valeur entiere la plus proche
			virtual int64_t		toLong() const ;
			//--
			virtual double		toDouble() const ;
            
            //-- [ign::data::Value]
            virtual float       toFloat() const ;
            
			//-- [ign::data::Value]
			virtual Value*		clone()   const ;

			//-- [ign::data::Value]
			virtual bool		equals( Value const& other, bool strict = true ) const ;

			/// \brief recupere la valeur encapsulee
			float const& 		value() const ;
			/// \brief recupere la valeur encapsulee
			float & 			value() ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			float _value;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
