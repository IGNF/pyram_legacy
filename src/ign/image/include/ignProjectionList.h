#ifndef _IGN_GEODESY_PROJECTIONLIST_H_
#define _IGN_GEODESY_PROJECTIONLIST_H_

#include <map>
#include <string>
#include <ignUniqueVector.h>
#include <ignConfig.h>


namespace ign 
{
	namespace geodesy 
	{
		
		/// \brief
		class IGN_IMAGE_API ProjectionList {
		public:
			
			///\brief: nom de la projection locale (indeterminee) par convention "LOCAL"
			static std::string											kStrLocal;
			
		public:
			/// \brief constructeur de ProjectionList
			ProjectionList(){;}
			/// \brief renvoie la chaine de caractere SD associee a la projection eproj qui est fournie au format eformat
			static std::string								FindExternalCodeProj(const std::string& eproj, const std::string & eformat);
			
			/// \brief renvoie la chaine de caractere correspondant a la chaine SD en parametre pour le format de sortie eformat
			static std::string								FindInternalCodeProj(const std::string& chaine, const std::string& eformat);
			
            /// \brief renvoie le format eformat de la projection eproj
            static std::string                              GetFormatProjection(const std::string& eproj);
            
			///\passage de l'enumere interne a un identifiant en string et inverse
			static	int										String2InternalCodeProj(const std::string& chaine);
			static	std::string								InternalCodeProj2String(int proj);
			
			static bool										WriteCodeList(const std::string& nom);
			static void										ReadCodeList(const std::string& nom);
			
			static void										SetCode(const std::string& chaineSD, const std::string& dico, const std::string& code);
			static std::string								GetCode(int internalId);
			static size_t									GetNumCode();
			
		private:
			static void                                                             _InitCodeProjList();
			
			static		ign::tools::UniqueVector<std::string>						_mInternalCodeProjList;
			///\pour chaque nomenclature de systeme, la liste des equivalents au code interne
			static		std::map<std::string, std::map<int, std::string>  >			_mExternalCodeProjList;
		};
	}
}

#endif
