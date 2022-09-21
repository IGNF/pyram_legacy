#ifndef _IGN_DATA_VALUE_H_
#define _IGN_DATA_VALUE_H_

#include <typeinfo>
#include <string>
#include <boost/units/detail/utility.hpp>

#include <ignConfig.h>
#include <ignInstanceCounter.h>
#include <ignNumeric.h>

#include <ignDataHelper.h>


namespace ign {
	namespace data {
		class Value ;
		typedef ign::unique_ptr< Value > ValuePtr ;
		class Variant;
	}
}




namespace ign {
	namespace data {
		/// \brief classe mere representant une valeur generique.
 		class IGN_DATA_API Value {
		public:
			/// \brief liste des types de base : 1 seul niveau de type
			typedef enum {
                VtNull=0,		  //correspond a Value* = 0
                VtBoolean=1,      //correspond a un booleen
                VtInteger=2,	  //correspond a un entier (int)
                VtLong=3,		  //correspond a un entier int64_t
                VtFloat=4,		  //correspond a un float
                VtDouble=5,		  //correspond a un double
                VtString=6,		  //correspond a un std::string
                VtArray=7,		  //correspond a un tableau de valeur
                VtObject=8,		  //correspond a une map clef/valeur
                VtDate=9,		  //correspond a une date YYYYMMDD (boost::gregorian::date)
                VtDateTime=10,	  //correspond a une date et une heure (boost::posix_time::ptime)
                VtGeometry=11,	  //correspond a une geometrie
                VtPath=12,        //correspond a un filesystem::path -- directory
                VtFilePath=13     //correspond a un filesystem::path -- file
			} ValueType ;

			/// \brief destructeur virtuel
			virtual						~Value();

			/// \brief test si la valeur est nulle (i.e. VtNull)
			virtual bool				isNull() const ;

			/// \brief convertit en chaine de caracteres
			virtual std::string			toString() const = 0 ;

			/// \brief convertit en booleen
			/// \throw ign::Exception en cas d'erreur
			virtual bool				toBoolean() const ;
			/// \brief convertit en int
			/// \throw ign::Exception en cas d'erreur
			virtual int					toInteger() const ;
			/// \brief convertit en long
			/// \throw ign::Exception en cas d'erreur
            virtual int64_t				toLong() const ;
            /// \brief convertit en double
            /// \throw ign::Exception en cas d'erreur
            virtual double              toDouble() const ;
            /// \brief convertit en float
            /// \throw ign::Exception en cas d'erreur
            virtual float               toFloat() const ;
            
			/// \brief renvoie une conversion en JSON
			virtual std::string			asJson( int const& numDecimals = -1 ) const ;
			/// \brief renvoie une conversion en JSON dans un flux
			virtual void				asJson( std::ostream & s ) const ;

			/// \brief renvoie une copie (profonde) de la valeur
			virtual Value*			 	clone()   const 	= 0;
			/// \brief renvoie le type associe a la valeur (implemente par IGN_DATA_DECLARE_TYPE)
			/// \warning renommage de Value::getType()
			virtual ValueType const& 	getValueType() const 	= 0 ;
			/// \brief renvoie le nom du type associe a la valeur (a des fins de debug et message)
			///  (implemente par IGN_DATA_DECLARE_TYPE)
			/// \warning renommage de Value::getTypeName()
			virtual std::string const& 	getValueTypeName() const = 0 ;

			/// \brief renvoie le nom du type
			/// \todo templater et utiliser ValueType::TypeName()
			static std::string 			GetTypeName( ValueType const& valueType );
			
			static ValueType 			StringToType( const std::string& str);


			/// \brief test si la valeur est du type indique
			/// \todo optimiser en ne reposant pas sur dynamic cast
			template < typename T >
			bool      is() const {
				return dynamic_cast< T const * >( this ) != NULL ;
			}


			/// \brief permet la conversion securisee vers une classe derivee
			/// \throw ign::Exception en cas d'erreur
			template < class T >
			T const&  as() const {
				T const * derived = dynamic_cast< T const * >( this );
				if ( derived == NULL ){
					IGN_THROW_EXCEPTION(
						Helper::GetMessageBadCast(
							getValueTypeName(),
							boost::units::detail::demangle( typeid(T).name() )
						)
					);
				}
				return *derived;
			}

			/// \brief permet la conversion securisee vers une classe derivee
			template < class T >
			T&  as() {
				T * derived = dynamic_cast< T * >( this );
				if ( derived == NULL ){
					IGN_THROW_EXCEPTION(
						Helper::GetMessageBadCast(
							getValueTypeName(),
							boost::units::detail::demangle( typeid(T).name() )
						)
					);
				}
				return *derived;
			}

			/// -- comparators

			/// \brief compare value to another value
			virtual bool				equals( Value const& other, bool strict = true ) const /*= 0*/ ;
		protected:
			Value() ;

			/// \brief convert value to JSON
			virtual void				_asJson( std::ostream & s ) const = 0 ;
		};
	}
}



#endif

