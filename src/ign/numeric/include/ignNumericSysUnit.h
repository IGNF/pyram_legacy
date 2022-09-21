/*
 *  unit.h
 *  ign-all
 *
 *  Created by Nicolas Bellaiche on 29/12/10.
 *  Copyright 2010 Institut Geographique National - France. All rights reserved.
 *
 */

#ifndef _IGN_NUMERIC_UNIT_SYSUNIT_H_
#define _IGN_NUMERIC_UNIT_SYSUNIT_H_

#include <map>

#include <ignConfig.h>
#include <ignUblas.h>
#include <ignNumericUnit.h>


namespace ign
{
	namespace numeric
	{
		namespace unit
		{
			
			/// \brief enumere decrivant le nom de l'unite de mesure
			enum eUnit
			{
				eUndefined		= 0,
				ePixel			= 1,
				eMeter			= 2,
				eCentimeter		= 3,
				eMillimeter		= 4,
				eKilometer		= 5,
				eInch			= 6,
				eFoot			= 7,
				eMile			= 8,
				eDegree			= 9,
				eRadian			= 10,
				eGrade			= 11,
				eDMS			= 12,
				eLengthUserDefined = 100, 
				eAngleUserDefined = 200
				
			} ;
			
			
			const char	kUndefined[] = "UNDEFINED";
			const char	kPixel[] = "PIXEL";
			const char	kMeter[] = "METER";
			const char	kCentimeter[] = "CENTIMETER";
			const char	kMillimeter[] = "MILLIMETER";
			const char	kKilometer[] = "KILOMETER";
			const char	kInch[] = "INCH";
			const char	kFoot[] = "FOOT";
			const char	kMile[] = "MILE";
			const char	kDegree[] = "DEGREE";
			const char	kRadian[] = "RADIAN";
			const char	kGrade[] = "GRADE";
			const char	kDMS[] = "DMS";
			const char	kLengthUserDefined[] = "USER_LENGTH_UNIT";
			const char	kAngleUserDefined[] = "USER_ANGLE_UNIT";
			
			///\brief definition d'un vecteur d'unites
			typedef ign::ublas::vector<numeric::unit::eUnit> unit_vector;
			
			///\brief identifie une chaine de caracteres a partir des abreviations classiques de l'unite et renvoie l'enumere correspondant
			eUnit		String2Unit(std::string const& unite);
			
			///\brief convertit l'unite dans la chaine correspondante
			std::string Unit2String(const eUnit& unite);

			
			/// \brief classe permettant de gerer les conversions d'unite
			/// \todo creer header, inclure ici
			class SysUnit;
			typedef ign::shared_ptr<SysUnit> SysUnitPtr;

			
			class IGN_NUMERIC_API SysUnit
			{
				friend class SysUnitRegistry;
			protected:
				//constructeur protected: passer par SysUnitRegistry::Create
				/// \brief operateur d'affectation private
				SysUnit&				operator = ( SysUnit const& other );
				/// \brief constructeur
				SysUnit(eUnit unite);
				/// \brief constructeur
				SysUnit(std::string const& uuid, eUnit unite);
				/// \brief constructeur par identification d'une chaine de caractere
				SysUnit(std::string const& uuid, const std::string& unite);
				/// \brief constructeur avec une liste d'unites par coord
				SysUnit(std::string const& uuid, const unit_vector& unites);
			public:			
				///\brief permet la conversion vers un systeme d'unite de longueur defini par l'utilisateur
				///donne le rapport entre unite de l'utilisateur et metre (ex km -> 1000.)
				static double			_LengthUserDefined;
				///\brief permet la conversion vers un systeme d'unite angulaire defini par l'utilisateur
				///donne le rapport entre unite de l'utilisateur et radian (ex dd -> 180/PI)
				static double			_AngleUserDefined;

				
				/// \brief destructeur
				virtual					~SysUnit();
				
				///\brief renvoie l'identifiant unique
				const std::string&		uuid()const;

				/// \brief convertit dans l'unite de reference (u -> _unit)
				///renvoie dans l'unite canonique de rang n du SysUnit la valeur @value exprimes en unites 'u' 
				double					toCanonic(const double& value, eUnit u, size_t n = 0) const;
				/// \brief convertit dans l'unite u (_unit -> u)
				///renvoie en unites u @value exprimes en unites canoniques de rang n du SysUnit
				double					fromCanonic(const double& value, eUnit u, size_t n = 0) const;
				
				/// \brief convertit dans l'unite de reference (u -> _unit)
				void					toCanonic(ign::ublas::vector<double>& values, const SysUnitPtr u) const;
				/// \brief convertit dans l'unite u (_unit -> u)
				void					fromCanonic(ign::ublas::vector<double>& values, const SysUnitPtr u) const;
				
				/// \brief renvoie une chaine de caractere correspondant a l'unite N
				std::string				asText(size_t n) const;
				
				/// \brief renvoie les unites
				unit_vector&			units() ;
				/// \brief renvoie les unites (CONST)
				const unit_vector&		units() const;
				/// \brief reference l'unite de la coordonnee i. 
				void					unit(size_t n, eUnit u);				
				/// \brief renvoie l'unite de la coordonnee i. 
				eUnit					unit(size_t n) const;
				
				/// \brief regarde si on doit convertir les coordonnees du point pour le rendre coherent avec celui du systeme
				/// renvoie true si au moins une coordonnee est differente et qu'elle est 'convertissable' ie unite_system !undefined ET unite_pt !undefined
				bool					needConversion(const ign::ublas::vector<double>& values, const SysUnitPtr& u) const;
				
				static	SysUnitPtr		Null;
			protected:
				///\brief unique identifier
				std::string				_uuid;

				/// \brief les unites canoniques du systeme
				/// empty:  unit undefined
				/// size =  1: all values are in unit _units[0]
				/// size >= data.size: each value has its own unit
				/// other : ASSERT(FALSE)
				unit_vector				_units;
				
				
			};
			
			
			///
			///
			/// \brief gestionnaire de systemes d'unite
			///
			/// permet d'enregistrer les systemes d'unites dans une map statique qui s'autodetruit a la fin de l'execution 
			/// L'utilisateur alloue le pointeur et laisse le soin de la desallocation au registre
			
			class SysUnitRegistry;
			typedef ign::shared_ptr<SysUnitRegistry> SysUnitRegistryPtr;
			class IGN_NUMERIC_API SysUnitRegistry {
			private:
				typedef  std::map<std::string, SysUnitPtr >::const_iterator		system_const_iterator;
				typedef  std::map<std::string, SysUnitPtr >::iterator			system_iterator;
				std::map<std::string, SysUnitPtr >								_systemMap;
				
				/// \brief renvoie l'iterateur de debut et de fin
				inline system_iterator							systemBegin()			{ return _systemMap.begin(); }
				inline system_iterator							systemEnd()				{ return _systemMap.end(); }
				inline system_const_iterator					systemBegin()	const	{ return _systemMap.begin(); }
				inline system_const_iterator					systemEnd()		const	{ return _systemMap.end(); }
				
				///\brief instance unique du registre des systemes
				//// c'est un singleton qui se desalloue tout seul a la fin de l'execution
				static SysUnitRegistryPtr						_systemRegistry;
				
				///\brief constructeur par defaut prive
				SysUnitRegistry ();
				///\brief constructeur de copie prive
				SysUnitRegistry (const SysUnitRegistry&);
			public:
				/// \brief renvoie le systeme a partir de la chaine s'il existe, NULL sinon
				const SysUnitPtr								getSystemById(const std::string& systemName) const;
								
				/// \brief ajoute sys a la table des systemes existants. 
				/// renvoie une exception si son identifiant (System::toString) existe deja dans la map
				/// prend la responsabilite du pointeur
				void											registerSystem(SysUnitPtr sys);
				
				///\brief renvoie l'instance unique du registre des systemes
				static	SysUnitRegistry&						Instance();
				
				///\brief teste l'existence d'un systeme
				bool											hasSystem(const std::string& uuid) const;
				
				///\brief methode permettant de generer le systeme. 
				static	SysUnitPtr								Create(const std::string& uuid, const numeric::unit::unit_vector& unites);
			};
			

		}
	}
}
#endif

