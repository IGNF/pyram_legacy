#include <iostream>
#include <fstream>

/**
 * Cette macro envoie le message dans la console et le flux de log.
 * Elle s'utilise presque comme un flux habituel (void \URL[Tutorial]{ExempleSORTIEMESSAGE.html}
 */
#define SORTIEMESSAGE(a) \
{ \
  std::ostringstream oss_sortiemessage; \
  oss_sortiemessage << a; \
  std::cout<<oss_sortiemessage.str(); \
  std::clog<<oss_sortiemessage.str() ; \
  std::cout.flush(); std::clog.flush(); \
}

#define SORTIEERREUR(a) SORTIEMESSAGE(a)

/**
* Affichage d'un message d'alerte.
*
* Cette macro fonctionne comme 
* SORTIEMESSAGE 
*, mais n'envoie le message que si les warnings sont actifs.
* Pour activer les warnings, voir 
* Systeme::EnableWarnings
*/
#define SORTIEWARNING(a) { SORTIEMESSAGE ("WARNING : "<< a) ; }

