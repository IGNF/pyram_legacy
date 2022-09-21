#ifndef _IGN_DATA_VARIANT_H_
#define _IGN_DATA_VARIANT_H_

#include <ignConfig.h>

#include <ignDataValue.h>

namespace ign {
	/// \brief Systeme de typage dynamique permettant de representer les attributs des objets geographiques.
	namespace data {
		/// \brief Represente une valeur de type variant.
		/// note : Cette classe doit etre consideree comme une sorte de pointeur intelligent, adaptee a differents types
		class IGN_DATA_API Variant {
		public:
			/// \brief constructeur par defaut, construit une valeur nulle
			Variant();
			/// \brief constructeur par recopie, clone la valeur encapsulee
			Variant( Variant const& variant );
			/// \brief constructeur par recopie, clone la valeur encapsulee
			Variant& operator = ( Variant const& variant );
			/// \brief constructeur par recopie
			~Variant();

			/// \brief constructeur a partir d'une valeur (copie)
			Variant( Value const& value );
			/// \brief constructeur a partir d'une valeur (responsable destruction)
			Variant( Value* value );
			/// \brief affectation d'une valeur
			Variant& operator = ( Value const& value );

			/// \brief indique si la valeur est nulle
			bool	        isNull() const;
			/// \brief rend la valeur encapsulee nulle
			void	        setToNull() ;

			/// \brief definition de la valeur encapsulee, effectuant une copie
			void			setValue( Value const& value );
			/// \brief definition de la valeur encapsulee en prenant la responsabilite
			/// \todo choisir entre celle ci et reset
			void			setValue( Value* value );

			/// \brief recupere la valeur encapsulee, non modifiable
			Value const &   value() const ;
			/// \brief recupere la valeur encapsulee, modifiable
			Value & 		value() ;

			/// \brief dereferencement operator
			Value* 			operator -> () ;
			/// \brief dereferencement operator
			Value const* 	operator -> () const ;

			/// \brief dereferencement operator
			Value& 			operator * () ;
			/// \brief dereferencement operator
			Value const& 	operator * () const ;

			/// \brief renvoie le type de la valeur encapsulee
			/// \deprecated use variant->getValueType()
			inline Value::ValueType const& 	getValueType() const { return _value->getValueType(); }
			/// \brief renvoie le nom du type associe
			/// \deprecated use variant->getValueTypeName()
			inline std::string const& 		getTypeName() const  { return _value->getValueTypeName(); }


			/// \brief renvoie une chaine de caracteres
			/// \throw ign::Exception si la conversion est impossible
			/// \deprecated use v->toString() instead of v.toString()
			inline std::string	toString() const { return _value->toString(); }
			/// \brief renvoie une valeur booleenne
			/// \throw ign::Exception si la valeur est nulle ou la conversion est impossible
			/// \deprecated use v->toBoolean() instead of v.toBoolean()
			inline bool			toBoolean() const { return _value->toBoolean(); }
			/// \brief renvoie une valeur entiere
			/// \throw ign::Exception si la valeur est nulle ou la conversion est impossible
			/// \deprecated use v->toInteger() instead of v.toInteger()
			inline int			toInteger() const { return _value->toInteger(); }
			/// \brief renvoie une representation flotante
			/// \throw ign::Exception si la valeur est nulle ou la conversion est impossible
			/// \deprecated use v->toLong() instead of v.toLong()
            inline int64_t		toLong() const { return _value->toLong(); }
            /// \brief renvoie une representation double
            /// \throw ign::Exception si la valeur est nulle ou la conversion est impossible
            /// \deprecated use v->toDouble() instead of v.toDouble()
            inline double        toDouble() const { return _value->toDouble(); }
            /// \brief renvoie une representation double
            /// \throw ign::Exception si la valeur est nulle ou la conversion est impossible
            /// \deprecated use v->toFloat() instead of v.toFloat()
            inline float         toFloat() const { return _value->toFloat(); }


			/// \brief test le type du variant
			/// \deprecated use v->is< data::Derived >()
			template < class T >
			inline bool is() const {
				IGN_SAFE_MODE_ASSERT( _value != 0 );
				return _value->is< T >();
			}

			/// \brief conversion vers une classe derivee de Value
			/// \deprecated use v->as< data::Derived >()
			template < class T >
			inline T const&  as() const {
				IGN_SAFE_MODE_ASSERT( _value != 0 );
				return _value->as< T >();
			}

			/// \brief conversion vers une classe derivee de Value
			/// \deprecated use v->as< data::Derived >()
			template < class T >
			inline T&  as() {
				IGN_SAFE_MODE_ASSERT( _value != 0 );
				return _value->as< T >();
			}


			/// \brief renvoie une representation json de la valeur (symetrique fromJson)
			std::string     toJson( int const& numDecimals = -1 ) const;
			/// \brief renvoie une representation json de la valeur dans un flux
			void            toJson( std::ostream & s ) const ;

			/// \brief construit le variant a partir d'une valeur json (symetrique toJson)
			/// \warning si un grand entier depasse la taille maximale, alors, il est lu comme double
			void			fromJson( std::string const& json );


			/// \brief [private] detruit le pointeur courant et prend la responsabilite de value (eq ign::unique_ptr)
			void 			reset( Value* value = 0 );
			/// \brief [private] relache le pointeur courant et perd la responsabilite de la valeur pointee (eq ign::unique_ptr)
			Value*			release();
			/// \brief [private] renvoie le pointeur encapsule, volontaire supprime suite a l'ajout de data::Null
			/// \warning ne pas utiliser pour tester si la valeur est nulle, voir isNull()
			Value*			get();
			/// \brief [private] renvoie le pointeur encapsule, volontaire supprime suite a l'ajout de data::Null
			/// \warning ne pas utiliser pour tester si la valeur est nulle, voir isNull()
			Value const*	get() const ;
		private:
			/// \brief pointeur vers la valeur encapsulee
			Value* 			_value;
		};
	}
}

#endif
