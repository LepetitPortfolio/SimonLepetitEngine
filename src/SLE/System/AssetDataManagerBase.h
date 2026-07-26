#pragma once

#include <unordered_map>
#include <cstdint>

class AssetData;

class AssetDataManagerBase
{
public:

	/**
	* Constructeur par défaut de DataManagerBase.
	* @tparam AssetData* Type des données gérées.
	*
	* @details
	* Initialise un objet DataManagerBase vide. Aucune allocation ou initialisation supplémentaire n'est nécessaire.
	*/
	AssetDataManagerBase();

	/**
	* Destructeur de DataManagerBase.
	* @tparam AssetData* Type des données gérées.
	*
	* @details
	* - Vérifie si la map `m_DataList` n'est pas vide.
	* - Si des données sont présentes, appelle `ClearAllData()` pour libérer la mémoire allouée pour chaque élément.
	* - Cela garantit qu'aucune fuite mémoire ne se produit lorsque l'objet DataManagerBase est détruit.
	*/
	~AssetDataManagerBase();

	/**
	* Ajoute une donnée à la collection.
	* @tparam AssetData Type de la donnée.
	* @param _Data Donnée à ajouter (doit être un pointeur, car on vérifie `nullptr`).
	*
	* @details
	* - **Vérification de nullité** : Si `_Data` est `nullptr`, la méthode retourne immédiatement sans rien faire.
	* - **Génération de l'ID** : Utilise `reinterpret_cast<uint64_t>(_Data)` pour convertir l'adresse mémoire de `_Data` en un `uint64_t`. Cela permet d'utiliser l'adresse comme clé unique dans la map.
	* - **Ajout à la map** : Vérifie si la clé (ID) n'existe pas déjà dans `m_DataList` avec `m_DataList.count(id) == 0`. Si c'est le cas, ajoute `_Data` à la map avec cette clé.
	*/
	void AddData(AssetData* _Data);

	/**
	* Supprime une donnée de la collection et libère sa mémoire.
	* @tparam AssetData* Type de la donnée.
	* @param _Data Donnée à supprimer (doit être un pointeur).
	*
	* @details
	* - **Vérification de nullité** : Si `_Data` est `nullptr`, la méthode retourne immédiatement.
	* - **Génération de l'ID** : Convertit l'adresse mémoire de `_Data` en `uint64_t` pour obtenir la clé.
	* - **Recherche dans la map** : Vérifie si la clé existe dans `m_DataList` avec `m_DataList.count(id) != 0`.
	* - **Suppression de la mémoire** :
	*   - Récupère le pointeur stocké dans la map (`m_DataList[id]`).
	*   - Supprime la donnée avec `delete data`.
	*   - Supprime l'entrée de la map avec `m_DataList.erase(id)`.
	*/
	void RemoveData(AssetData* _Data);

	/**
	* Supprime toutes les données de la collection et libère leur mémoire.
	* @tparam AssetData* Type des données.
	*
	* @details
	* - **Parcours de la map** : Itère sur toutes les entrées de `m_DataList`.
	* - **Suppression de la mémoire** : Pour chaque entrée, supprime la donnée avec `delete data.second`.
	* - **Nettoyage de la map** : Appelle `m_DataList.clear()` pour vider la map.
	*/
	void ClearAllData();

	uint32_t GetDataCount() const { return static_cast<uint32_t>(m_DataList.size()); }
	std::unordered_map<uint64_t, AssetData*>*  GetData() { return &m_DataList; }

protected:

	std::unordered_map<uint64_t, AssetData*> m_DataList;
};
