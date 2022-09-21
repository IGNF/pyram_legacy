#ifndef __MACRO_TYPES_HPP__
#define __MACRO_TYPES_HPP__

//-----------------------------------------------------
// Types de base.
//-----------------------------------------------------
/** @name Types de base 
 *
 * Les types de base sont redéfinis (sauf {\bf char}), mais le travail de remplacement des types standards par ces types de base et de longue haleine, et donc non terminé.
 * Le noyau redéfinit ses types de bases de la façon suivante :
 */
//@{
#ifdef Bool
#undef Bool
#endif
/** Bool */
typedef bool Bool;
/** Charactere */
typedef char Char;
/** Entier signé de taille 1 octet */
typedef signed char Int1;
/** Entier non signé de taille 1 octet */
typedef unsigned char UInt1;
/** Entier signé de taille 2 octets */
typedef signed short Int2;
/** Entier non signé de taille 2 octets */
typedef unsigned short UInt2;
/** Entier signé de taille 4 octets */
typedef signed int Int4;
/** Entier non signé de taille 4 octets */
typedef unsigned int UInt4;
/** Flottant de taille 4 octets */
typedef float Float4;
/** Flottant de taille 8 octets */
typedef double Float8;
//@}

#endif //__MACRO_TYPES_HPP__

