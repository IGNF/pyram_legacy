#ifndef _IGN_DATA_LONG_H_
#define _IGN_DATA_LONG_H_

#include <ignConfig.h>
#include <ignDataValue.h>


#include <sstream>

namespace ign {
	namespace data {

		/// \brief encapsule un booleen en temps que valeur
		class IGN_DATA_API Long : public Value {
		public:
			/// \brief constructeur par defaut
			Long();
			/// \brief constructeur par recopie
			Long( Long const& other );
			/// \brief operateur d'affectation
			Long& operator = ( Long const& other );
			/// \brief destructeur
			virtual ~Long();

			/// \brief constructeur a partir d'un booleen
			Long( int64_t const& value );
			/// \brief constructeur a partir d'un booleen
			Long& operator = ( int64_t const& value ) ;

			/// \brief implicit cast
			inline operator int64_t () const { return _value; }

			//--
			virtual std::string     toString() const ;
			/// \brief convertit en booleen (true si valeur != 0 )
			virtual bool		    toBoolean() const ;
			//-- [ign::data::Long]
			virtual int   		    toInteger() const ;
			//-- [ign::data::Value]
            virtual int64_t		    toLong() const ;
            //-- [ign::data::Value]
            virtual double          toDouble() const ;
            //-- [ign::data::Value]
            virtual float           toFloat() const ;

			//-- [ign::data::Value]
			virtual Value*		clone()   const;

			//-- [ign::data::Value]
			virtual bool		equals( Value const& other, bool strict = true ) const ;

			/// \brief recupere la valeur encapsulee
			int64_t const& 		value() const ;
			/// \brief recupere la valeur encapsulee
			int64_t & 			value() ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		protected:
			int64_t _value;

			//-- [ign::data::Value]
			virtual void _asJson( std::ostream & s ) const ;
		};

	}
}

#endif
