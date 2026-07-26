#pragma once
#include "Error.h"
#include "String.h"

#include <fstream>
#include <vector>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

class FileReader
{
public:

	/**
	* Lit le contenu d'un fichier binaire et le retourne sous forme de tableau de caractères.
	*
	* @param _Filename Chemin du fichier à lire.
	* @return std::vector<char> Contenu binaire du fichier.
	*
	* @details
	* - Ouvre le fichier en mode binaire (`std::ios::binary`) et se positionne à la fin (`std::ios::ate`) pour obtenir sa taille.
	* - Si le fichier ne s'ouvre pas, affiche une erreur via `Err()`.
	* - Récupère la taille du fichier avec `tellg()` (position actuelle dans le fichier).
	* - Alloue un `std::vector<char>` de la taille du fichier.
	* - Se repositionne au début du fichier avec `seekg(0)`.
	* - Lit le contenu du fichier dans le `buffer` avec `file.read()`.
	* - Ferme le fichier et retourne le buffer.
	*/
	static std::vector<char> ReadBinaryFile(const std::string& _Ffilename);

	/**
	* Récupère le dossier racine de l'application.
	*
	* @return String Chemin du dossier racine.
	*
	* @details
	* - Si `m_RootFolder` est vide, calcule le chemin du dossier racine :
	*   - Récupère le chemin du répertoire courant avec `_getcwd(NULL, 0)`.
	*   - Découpe le chemin en sous-dossiers avec `Split('\\')` (séparateur Windows).
	*   - Reconstruit le chemin en ignorant les deux derniers dossiers (probablement pour remonter à la racine du projet).
	*   - Stocke le résultat dans `m_RootFolder` pour les appels futurs.
	* - Retourne `m_RootFolder`.
	*/
	static String GetRootFolder();

private:

	/**
	* Variable statique de la classe FileReader.
	* Stocke le chemin du dossier racine de l'application.
	* Initialisée à vide, elle est remplie à la première appel de GetRootFolder().
	*/
	static String m_RootFolder;
	
};