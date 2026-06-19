#pragma once

#include <iosfwd>
#include <iostream>
#include <streambuf>

#include <cstdio>

std::ostream& Log();

std::ostream& Warning();


/**
 * Fournit un flux de sortie (std::ostream) pour écrire des messages d'erreur.
 *
 * @return Référence vers un flux std::ostream statique, lié au tampon `DefaultErrorStreamBuf`.
 *
 * @details
 * - Utilise un tampon (`DefaultErrorStreamBuf`) et un flux (`std::ostream`) statiques pour éviter les réallocations.
 * - Le flux est initialisé avec l'adresse du tampon, ce qui permet d'écrire des messages d'erreur via ce flux.
 * - Exemple d'utilisation : `Err() << "Erreur : " << message;`
 */
std::ostream& Err();

std::ostream& FatalErr();

class DefaultErrorStreamBuf : public std::streambuf
{
public:

	/**
	* Constructeur : initialise un tampon de taille fixe pour stocker les caractères.
	*
	* - Alloue dynamiquement un tableau de caractères de taille 64.
	* - Configure le tampon de sortie (output buffer) avec `setp` :
	*   - `buffer` : début du tampon.
	*   - `buffer + size` : fin du tampon.
	*   Cela permet à std::ostream d'écrire dans ce tampon avant de le vider.
	*/
	DefaultErrorStreamBuf();

	/**
	* Destructeur : libère les ressources allouées.
	*
	* - Appelle `sync()` pour vider le tampon avant la destruction (écrit les données restantes dans stderr).
	* - Libère la mémoire allouée pour le tampon via `delete[] pbase()`.
	*   `pbase()` retourne le pointeur vers le début du tampon de sortie.
	*/
	~DefaultErrorStreamBuf() override;

private:

	/**
	* Gère le débordement du tampon (quand le tampon est plein ou qu'un caractère EOF est rencontré).
	*
	* @param _Character Caractère à écrire ou EOF (fin de flux).
	* @return EOF en cas d'erreur, sinon le caractère écrit ou un code de succès.
	*
	* @details
	* - Si `_Character` n'est pas EOF et qu'il reste de la place dans le tampon (`pptr() != epptr()`) :
	*   - Écrit le caractère dans le tampon via `sputc` et retourne le résultat.
	* - Si `_Character` est EOF :
	*   - Appelle `sync()` pour vider le tampon, puis rappelle `overflow(_Character)`.
	* - Si le tampon est plein et que `_Character` n'est pas EOF :
	*   - Appelle `sync()` pour vider le tampon et libérer de l'espace.
	*/
	int overflow(int _Character) override;

	/**
	* Vide le tampon en écrivant son contenu dans stderr (flux d'erreur standard).
	*
	* @return 0 en cas de succès, ou un code d'erreur sinon.
	*
	* @details
	* - Si le tampon contient des données (`pbase() != pptr()`) :
	*   - Calcule la taille des données à écrire (`pptr() - pbase()`).
	*   - Écrit les données dans `stderr` via `std::fwrite`.
	*   - Réinitialise le pointeur de position du tampon (`pptr`) au début (`pbase`) pour permettre de nouvelles écritures.
	*/
	int sync() override;
};