#ifndef _IGN_DATA_OBJECT_H_
#define _IGN_DATA_OBJECT_H_
#include <vector>
#include <map>

#include <ignConfig.h>
#include <ignDataVariant.h>



namespace ign {
	namespace data {

		class NameValuePair;

		/// \brief Represente un objet simple, c'est a dire une liste de pair "attribut" | "valeur" sans repetition d'un nom
		///	d'attribut.
		class IGN_DATA_API Object : public Value {
		public:
			/// \brief Constructeur d'un objet vide
			Object();
			/// \brief Constructeur par recopie
			Object( Object const& other );
			/// \brief Operateur d'affectation
			Object&                             operator = ( Object const& other );
			/// \brief Destructeur
			virtual                            ~Object();

			/// \brief ajoute les metas d'un objet a l'objet courant
			void                               append(Object const& other);

			/// \brief test l'existence d'un attribut
			bool				               hasAttribute( std::string const& name ) const;
			/// \brief renvoie le nom d'un attribut a partir de son identifiant
			std::string	const&	               getAttributeName( size_t const& i ) const;
			/// \brief renvoie la position d'un attribut par son nom, -1 si non trouve
			int 				               getAttributeNumber( std::string const& name ) const ;
			/// \brief test si un attribut est nul. renvoie true s'il n'existe pas
			bool				               isNullAttribute( std::string const& name ) const ;
			/// \brief test si un attribut est nul. renvoie true s'il n'existe pas
			bool				               isNullAttribute( size_t const& i ) const ;

			/// \brief renvoie le type d'un attribut, null si attribut de valeur nulle
			Value::ValueType	               getAttributeType( size_t const& i ) const;
			/// \brief renvoie le type d'un attribut, null si attribut de valeur nulle, ou attribut non defini
			Value::ValueType	               getAttributeType( std::string const& name ) const;

			/// \brief renvoie le nombre d'attribut
			size_t 				               numAttributes() const;
			/// \brief indique si l'objet contient aucun attribut
			bool				               isEmpty() const ;

			/// \brief definit la valeur d'un attribut ou cree l'attribut s'il n'existe pas
			void				               setAttribute( std::string const& name, Value* value );
			/// \brief definit la valeur d'un attribut ou cree l'attribut s'il n'existe pas
			void				               setAttribute( std::string const& name, Value const& value );

			/// \brief recupere la valeur d'un attribut par son nom
			/// \throw envoie une exception s'il n'existe pas
			Variant const& 		               attribute( std::string const& name ) const ;
			/// \brief recupere la valeur d'un attribut par son nom
			/// \throw envoie une exception s'il n'existe pas
			Variant & 			               attribute( std::string const& name ) ;

			/// \brief renvoie un attribut par son identifiant
			Variant const& 		               attribute( size_t const& i ) const;
			/// \brief renvoie un attribut par son identifiant
			Variant & 			               attribute( size_t const& i ) ;

			Variant const & 	               operator[] ( size_t const& i ) const ;
			Variant & 			               operator[] ( size_t const& i ) ;

			Variant const & 	               operator[] ( std::string const& name ) const ;
			Variant & 			               operator[] ( std::string const& name ) ;

			/// \brief supprime un attribut
			void				                removeAttribute( size_t const& i );
			/// \brief supprime un attribut par son nom
			void				                removeAttribute( std::string const& name, bool throwIfNotExists = false );

			/// \brief supprime tous les attributs de l'objet
			void				                clear();

			/// \see ign::data::Object::asJson
			virtual std::string 		        toString() const ;
			//--
			virtual Value*			            clone()   const ;

			//-- [ign::data::Value]
			virtual bool			            equals( Value const& other, bool strict = true ) const ;

			//-- [ign::data::Value]
			virtual Value::ValueType const& 	getValueType() const ;
			//-- [ign::data::Value]
			virtual std::string const& 			getValueTypeName() const ;

			/// \brief return type
			static Value::ValueType const&		Type() ;
			/// \brief return type name
			static std::string const&			TypeName() ;
		private:
			/// \brief represente les attributs de l'objet
			std::vector< NameValuePair >		_attributes;
			/// \brief index pour la recherche des attributs par leurs noms
			std::map< std::string, int >		_index;

			/// \brief calcul l'index sur les noms
			void	                            _computeIndex();

		protected:
			//-- [ign::data::Value]
			virtual void                        _asJson( std::ostream & s ) const ;
            
            static int                          _IndentJson;
		};
	}
}

#endif

