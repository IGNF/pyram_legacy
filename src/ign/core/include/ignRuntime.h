#ifndef _IGN_RUNTIME_H_
#define _IGN_RUNTIME_H_

#include <ignConfig.h>

#include <string>
#include <boost/optional.hpp>
#include <boost/uuid/random_generator.hpp>

namespace ign {
	/// \brief represente le contexte d'execution
	class IGN_CORE_API Runtime {
	public:
		/// \brief renvoie l'unique instance de
		static Runtime & 						Instance() ;
		
		/// \brief recupere une variable d'environnement
		boost::optional< std::string >			getEnv( std::string const& name ) const ;
		
		/// \brief met a jour une variable d'environnement, return false si echec
		bool									setEnv( std::string const& key, std::string const& name ) const ;

		/// \brief recupere le repertoire des donnees (IGN_DATA)
		std::string      						getDataDirectory() const ;
		/// \brief definit le repertoire des donnees (utile si macro non definie)
		void									setDataDirectory( std::string const& dataDirectory );
		
		/// \brief recupere le repertoire des donnees test de non regression(IGN_TEST_DATA)
		std::string const& 						getTestDataDirectory() const ;
		/// \brief definit le repertoire des donnees test de non regression (utile si macro non definie)
		void									setTestDataDirectory( std::string const& dataDirectory );
		
		/// \brief renvoie le repertoire contenant les fichiers temporaires
		std::string 							getTempDirectory() const ;
		/// \brief renvoie le chemin pour un fichier contenu %IGN_DATA%/tmp/[filename]
		std::string 							getTempPath( std::string const& relativePath ) const ;

		/// \brief renvoie un chemin pour creer un fichier temporaire a base de UUID (identifiant ignoble unique)
		std::string								createTempPath( std::string const& prefix = "" ) const;

		/// \brief renvoie un identifiant unique
		std::string								uuid() const;
		
		/// \brief renvoie un identifiant unique avec prefixe et
		std::string								uuidWithPrefix(const std::string& prefix) const;

		/// \brief renvoie un identifiant unique avec suffixe
		std::string								uuidWithPostfix(const std::string& postfix) const;
		
		/// \brief renvoie un identifiant unique avec prefixe et suffixe
		std::string								uuidWithPreAndPostfix(const std::string& prefix, const std::string& postfix) const;
		
		
		///\brief indique si la machine tourne en 32 ou en 64b
		bool									is64b()const;
		
	private:
		/// \brief generateur d'identifiant aleatoire
		mutable boost::uuids::random_generator 	_uuidGenerator;
		/// \brief le repertoire des donnees de l'application
		std::string								_dataDirectory;
		/// \brief le repertoire des donnees test de non regression
		std::string								_testDataDirectory;

		/// \brief constructeur par defaut
		Runtime();
		
		/// \brief interdiction de copie
		Runtime( Runtime const& other );

		/// \brief envoie une exception indiquant que le repertoire des donnees n'est pas defini
		void									_throwExceptionDataDirectoryNotSet() const ;
	};

	/// \brief recupere le contexte
	IGN_CORE_API Runtime &					runtime();

}


#endif
