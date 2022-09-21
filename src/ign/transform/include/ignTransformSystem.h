#ifndef _IGN_TRANSFORM_SYSTEM_H_
#define _IGN_TRANSFORM_SYSTEM_H_

#include <string>
#include <map>

#include <ignConfig.h>

#include <ignCommon.h>
#include <ignTransformVector.h>

namespace ign {
	/// \brief regroupe les transformations applicable sur les geometries
	namespace transform {
		/// \brief represente un systeme
		class IGN_DATA_API System
		{
		private:
			
			/// \brief l'identifiant unique du systeme
			std::string										_uuid;	
			
			/// \brief unites canoniques du systeme
			/// empty: unit undefined
			/// size = 1: all values are in unit _units[0]
			/// size = _data.size: each value has its own unit
			/// other : ASSERT(FALSE)
			ign::numeric::unit::SysUnitPtr					_sysUnit;
			
			/// \brief constructeur par defaut private
			System();
		protected:
			/// \brief interdit le constructeur de copie
			System(const System&);
			
			/// \brief constructeur avec uuid a la responsabilite de l'utilisateur
			/// @warning: constructeur de la classe mere laisse public pour des usages un peu detournes
			///le bon usage est de typer les systemes et d'appeler SystemRegistry::Create<ConcreteSystem>(), donc a utiliser avec precaution. 
			System(const std::string&  uuid, const numeric::unit::SysUnitPtr unit);

		public:

			static ign::shared_ptr<System>					NullPtr;

			
			/// \brief destructeur
			virtual											~System();
			
			/// \brief renvoie une chaine de description du systeme
			virtual std::string								toString() const;

			/// \brief renvoie l'identifiant unique du systeme
			const std::string&								uuid() const;
			/// \brief renvoie le systeme d'unites canoniques du systeme
			numeric::unit::SysUnitPtr						sysUnit() ;
			/// \brief renvoie le systeme d'unites canoniques du systeme (CONST)
			const ign::numeric::unit::SysUnitPtr			sysUnit() const;
			
			/// \brief verification du type d'une classe  derivee (const)
			template < typename T >
			bool											is() const 
			{	
				return (typeid(*this)==typeid(T));
			}
			
			/// \brief down cast sur une classe  derivee (const)
			template < typename T >
			T const &										as() const 
			{	
				T const * cast = dynamic_cast< T const * >( this );
				if ( cast == NULL ) {IGN_THROW_EXCEPTION("[System::as] bad cast");	}
				return *cast;
			}
			/// \brief down cast sur une classe  derivee
			template < typename T >
			T  &											as()  
			{	
				T  * cast = dynamic_cast< T * >( this );
				if ( cast == NULL ) {IGN_THROW_EXCEPTION("[System::as] bad cast");	}
				return *cast;
			}				
			///\ brief teste l'egalite entre deux systemes, par defaut compare toString()
			virtual bool									equals(const System& other) const;
		};
		
		typedef ign::shared_ptr<System>			SystemPtr;
		
		///
		///
		/// \brief systeme unknown
		class IGN_TRANSFORM_API
		SystemUnknown: public System
		{
		public:

			/// \brief destructeur
			virtual									~SystemUnknown();
			
			///\brief renvoie l'instance unique de SystemUnknown
			static	SystemPtr						Instance();
			
			/// \brief renvoie une chaine unique correspondant au systeme
			std::string								toString() const;

			static	std::string						UUID();

		private:
			/// \brief constructeur par defaut prive
			SystemUnknown();
			///\brief constructeur de copie prive
			SystemUnknown (const SystemUnknown&);
			
			///\brief instance unique 
			//// c'est un singleton qui se desalloue tout seul a la fin de l'execution
			static ign::shared_ptr<SystemUnknown>	_systemUnknown;
			
			
			static	std::string						_UUID;
		};
		
		
		
		///
		///
		/// \brief gestionnaire de systemes
		///
		/// permet d'enregistrer les systemes dans une map statique qui s'autodetruit a la fin de l'execution 
		/// L'utilisateur alloue le pointeur et laisse le soin de la desallocation au registre
		class SystemRegistry;
		typedef ign::shared_ptr<SystemRegistry>	SystemRegistryPtr;
		
		class IGN_TRANSFORM_API SystemRegistry {
		private:
			typedef  std::map<std::string, SystemPtr>::const_iterator	system_const_iterator;
			typedef  std::map<std::string, SystemPtr >::iterator		system_iterator;
			std::map<std::string, SystemPtr >							_systemMap;

			/// \brief renvoie l'iterateur de debut et de fin
			inline system_iterator							systemBegin()		{ return _systemMap.begin(); }
			inline system_iterator							systemEnd()			{ return _systemMap.end(); }
			inline system_const_iterator					systemBegin()const	{ return _systemMap.begin(); }
			inline system_const_iterator					systemEnd()	 const	{ return _systemMap.end(); }
			
			///\brief instance unique du registre des systemes
			//// c'est un singleton qui se desalloue tout seul a la fin de l'execution
			static SystemRegistryPtr						_systemRegistry;
			
			///\brief constructeur par defaut prive
			SystemRegistry ();
			///\brief constructeur de copie prive
			SystemRegistry (const SystemRegistry&);

		public:
            ///\brief destructeur
            virtual  ~SystemRegistry();
            
			/// \brief renvoie le systeme a partir de la chaine s'il existe, renvoie une exception dans le cas contraire
			const SystemPtr									getSystemById(const std::string& uuid) const;

			/// \brief ajoute sys a la table des systemes existants. 
			/// renvoie une exception si son identifiant (System::toString) existe deja dans la map
			/// prend la responsabilite du pointeur
			void											registerSystem(transform::SystemPtr const& sys);
			
			///\brief renvoie l'instance unique du registre des systemes
			static	SystemRegistry&							Instance();
			
			///\brief teste l'existence d'un systeme
			bool											hasSystem(const std::string &uuid) const;
			
			///\brief methode template instance fille de Systeme.
			///@warning: necessite l'existence d'un constructeur prenant en parametre une chaine de caracteres
			template <class ConcreteSystem>
			static	ign::shared_ptr<ConcreteSystem>		Create(const std::string& uuid, numeric::unit::SysUnitPtr unites)
			{				
				SystemPtr system;
				if (!SystemRegistry::Instance().hasSystem(uuid))
				{
					system.reset(new ConcreteSystem(uuid, unites));
					SystemRegistry::Instance().registerSystem(system);
				}
				else 
				{
					system = SystemRegistry::Instance().getSystemById(uuid);
					if (!system->is<ConcreteSystem>())
					{
						IGN_ASSERT(system->is<ConcreteSystem>());
					}
				}
				return ign::static_pointer_cast<ConcreteSystem>(system);
			}
		};
	}
}
	
#endif
