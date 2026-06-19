#pragma once
#include <math.h>


/**
 * 0Représente un vecteur en 3D avec des composantes X, Y, Z.
 * @tparam T Type numérique des composantes (ex: float, double, int).
 */
template<typename T>
struct Vector3
{

	T X = 0;
	T Y = 0;
	T Z = 0;

	/**
	* Constructeur par défaut : initialise un vecteur nul (0, 0, 0).
	*/
	Vector3() {}

	/**
	* Constructeur avec composantes : initialise un vecteur avec les valeurs X, Y, Z données.
	* @param x Composante X.
	* @param y Composante Y.
	* @param z Composante Z.
	*/
	Vector3(T x, T y, T z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	/**
	* Constructeur de copie depuis un autre Vector3 (de type potentiellement différent).
	* @tparam T Type du vecteur source.
	* @param _Vector3 Vecteur source à copier.
	*
	* @note Ce constructeur permet une conversion implicite entre Vector3 de types différents.
	*/
	template<typename T>
	Vector3(Vector3<T>& _Vector3)
	{
		X = _Vector3.X;
		Y = _Vector3.Y;
		Z = _Vector3.Z;
	}

	/**
	* Vérifie si le vecteur courant est colinéaire avec un autre vecteur.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Vecteur à comparer.
	* @return true si les vecteurs sont colinéaires, false sinon.
	*
	* @details
	* Deux vecteurs sont colinéaires si leurs composantes sont proportionnelles :
	* - X * _Other.Y == Y * _Other.X
	* - X * _Other.Z == Z * _Other.X
	* - Y * _Other.Z == Z * _Other.Y
	*
	* @note Cette méthode utilise des produits en croix pour éviter les divisions (et donc les erreurs de précision).
	*/
	template<typename T>
	bool Collinear(const Vector3<T>& _Other)
	{
		if (X * _Other.Y == Y * _Other.X)
		{
			if (X * _Other.Z == Z * _Other.X)
			{
				if (Y * _Other.Z == Z * _Other.Y)
				{
					return true;
				}
			}
		}

		return false;
	}

	/**
	* Vérifie si deux vecteurs sont colinéaires (version statique).
	* @tparam T Type des vecteurs à comparer.
	* @param _VecA Premier vecteur.
	* @param _VecB Deuxième vecteur.
	* @return true si les vecteurs sont colinéaires, false sinon.
	*/
	template<typename T>
	static bool Collinear(const Vector3<T>& _VecA, const Vector3<T>& _VecB)
	{
		if (_VecA.X * _VecB.Y == _VecA.Y * _VecB.X)
		{
			if (_VecA.X * _VecB.Z == _VecA.Z * _VecB.X)
			{
				if (_VecA.Y * _VecB.Z == _VecA.Z * _VecB.Y)
				{
					return true;
				}
			}
		}

		return false;
	}

	/**
	* Calcule le produit scalaire entre le vecteur courant et un autre vecteur.
	* @tparam T Type du vecteur à multiplier.
	* @param _Other Vecteur à multiplier.
	* @return Produit scalaire (float).
	*
	* @details
	* Le produit scalaire est calculé comme : X * _Other.X + Y * _Other.Y + Z * _Other.Z.
	* Il est utilisé pour calculer l'angle entre deux vecteurs ou pour projeter un vecteur sur un autre.
	*/
	template<typename T>
	float DotProduct(const Vector3<T>& _Other)
	{
		return X * _Other.X + Y * _Other.Y + Z * _Other.Z;
	}

	/**
	* Calcule le produit scalaire entre deux vecteurs (version statique).
	* @tparam T Type des vecteurs à multiplier.
	* @param _VecA Premier vecteur.
	* @param _VecB Deuxième vecteur.
	* @return Produit scalaire (float).
	*/
	template<typename T>
	static float DotProduct(const Vector3<T>& _VecA, const Vector3<T>& _VecB)
	{
		return _VecA.X * _VecB.X + _VecA.Y * _VecB.Y + _VecA.Z * _VecB.Z;
	}

	/**
	* Calcule le produit vectoriel entre le vecteur courant et un autre vecteur.
	* @tparam T Type du vecteur à multiplier.
	* @param _Other Pointeur vers le vecteur à multiplier.
	* @return Nouveau vecteur résultat du produit vectoriel.
	*
	* @details
	* Le produit vectoriel de deux vecteurs A et B est calculé comme :
	* - X = A.Y * B.Z - A.Z * B.Y
	* - Y = A.Z * B.X - A.X * B.Z
	* - Z = A.X * B.Y - A.Y * B.X
	*/
	template<typename T>
	Vector3<T> CrossProduct(const Vector3<T>* _Other)
	{
		T x = Y * _Other->Z - Z * _Other->Y;
		T y = Z * _Other->X - X * _Other->Z;
		T z = X * _Other->Y - Y * _Other->X;

		return Vector3<T>(x, y, z);
	}

	/**
	* Calcule la longueur (norme) du vecteur.
	* @return Longueur du vecteur (float).
	*
	* @details
	* La longueur est calculée comme : sqrt(X² + Y² + Z²).
	*/
	template<typename T>
	float Lengh()
	{
		return sqrtf(powf(X, 2) + powf(Y, 2) + powf(Z, 2));
	}

	/**
	* Calcule le carré de la longueur du vecteur (évite le calcul de la racine carrée).
	* @return Carré de la longueur du vecteur (float).
	*
	* @details
	* Utile pour les comparaisons de distances (évite le coût de sqrt).
	*/
	float SqrtLengh()
	{
		return powf(X, 2) + powf(Y, 2) + powf(Z, 2);
	}

	/**
	* Normalise le vecteur (le transforme en vecteur unitaire de longueur 1).
	* @tparam T Type du vecteur à retourner.
	* @return Nouveau vecteur normalisé.
	*
	* @details
	* Divise chaque composante par la longueur du vecteur.
	* Si le vecteur est nul, le comportement est indéfini (division par zéro).
	*/
	template<typename T>
	Vector3<T> Normalise()
	{
		float coef = 1 / Lengh();
		T x = X * coef;
		T y = Y * coef;
		T z = Z * coef;

		return Vector3<T>(x, y, z);
	}

	/**
	* Calcule l'angle (en radians) entre le vecteur courant et un autre vecteur.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Pointeur vers l'autre vecteur.
	* @return Angle en radians (float).
	*
	* @details
	* Utilise la formule : cos(θ) = (A · B) / (|A| * |B|)
	* Puis θ = arccos(cos(θ)).
	*/
	template<typename T>
	float AngleBetween(const Vector3<T>* _Other)
	{
		float dot = DotProduct(_Other);
		float thisLenght = Lengh();
		float otherLenght = _Other->Lengh();

		return acos(dot / (thisLenght * otherLenght));
	}

	/**
	* Calcule l'angle (en radians) entre deux vecteurs (version statique).
	* @tparam T Type des vecteurs à comparer.
	* @param _VecA Premier vecteur.
	* @param _VecB Deuxième vecteur.
	* @return Angle en radians (float).
	*/
	template<typename T>
	static float AngleBetween(const Vector3<T>& _VecA, const Vector3<T>& _VecB)
	{
		float dot = DotProduct(_VecA, _VecB);
		float vecALenght = _VecA.Lengh();
		float vecBLenght = _VecB.Lengh();

		return acos(dot / (vecALenght * vecBLenght));
	}

	/**
	* Opérateur de conversion vers un Vector3 de type différent.
	* @tparam U Type cible du vecteur.
	* @return Nouveau vecteur de type Vector3<U> avec les composantes converties.
	*/
	template<typename U>
	operator Vector3<U>() const
	{
		return Vector3<U>(static_cast<U>(X), static_cast<U>(Y), static_cast<U>(Z));
	}

	/**
	* Additionne deux vecteurs.
	* @tparam T Type du vecteur à ajouter.
	* @param _Other Vecteur à ajouter.
	* @return Nouveau vecteur résultat de l'addition.
	*/
	template<typename T>
	Vector3<T> operator +(const Vector3<T>& _Other)
	{
		return Vector3<T>(X + _Other.X, Y + _Other.Y, Z + _Other.Z);
	}

	/**
	* Soustrait deux vecteurs.
	* @tparam T Type du vecteur à soustraire.
	* @param _Other Vecteur à soustraire.
	* @return Nouveau vecteur résultat de la soustraction.
	*/
	template<typename T>
	Vector3<T> operator -(const Vector3<T>& _Other)
	{
		return Vector3<T>(X - _Other.X, Y - _Other.Y, Z - _Other.Z);
	}

	/**
	* Multiplie le vecteur par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Nouveau vecteur résultat de la multiplication.
	*/
	template<typename T>
	Vector3<T> operator *(T _Value)
	{
		return Vector3<T>(X * _Value, Y * _Value, Z * _Value);
	}

	/**
	* Divise le vecteur par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Nouveau vecteur résultat de la division.
	*/
	template<typename T>
	Vector3<T> operator /(T _Value)
	{
		return Vector3<T>(X / _Value, Y / _Value, Z / _Value);
	}

	/**
	* Ajoute un vecteur au vecteur courant.
	* @tparam T Type du vecteur à ajouter.
	* @param _Other Vecteur à ajouter.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector3<T> operator +=(const Vector3<T>& _Other)
	{
		X += _Other.X;
		Y += _Other.Y;
		Z += _Other.Z;

		return this;
	}

	/**
	* Soustrait un vecteur du vecteur courant.
	* @tparam T Type du vecteur à soustraire.
	* @param _Other Vecteur à soustraire.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector3<T> operator -=(const Vector3<T>& _Other)
	{
		X -= _Other.X;
		Y -= _Other.Y;
		Z -= _Other.Z;

		return this;
	}

	/**
	* Multiplie le vecteur courant par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector3<T> operator *=(T _Value)
	{
		X *= _Value;
		Y *= _Value;
		Z *= _Value;

		return this;
	}

	/**
	* Divise le vecteur courant par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector3<T> operator /=(T _Value)
	{
		X /= _Value;
		Y /= _Value;
		Z /= _Value;

		return this;
	}

	/**
	* Vérifie si deux vecteurs sont égaux.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Vecteur à comparer.
	* @return true si les vecteurs sont égaux, false sinon.
	*/
	template <typename T>
	bool operator ==(const Vector3<T>& _Other)
	{
		if (X == _Other.X)
		{
			if (Y == _Other.Y)
			{
				if (Z == _Other.Z)
				{
					return true;
				}
			}
		}

		return false;
	}

	/**
	* Vérifie si deux vecteurs sont différents.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Vecteur à comparer.
	* @return true si les vecteurs sont différents, false sinon.
	*/
	template <typename T>
	bool operator !=(const Vector3<T>& _Other)
	{
		return !(this == _Other);
	}

};

/**
 * Soustrait deux vecteurs (version externe).
 * @tparam T Type des vecteurs.
 * @param _Left Premier vecteur.
 * @param _Right Deuxième vecteur.
 * @return Nouveau vecteur résultat de la soustraction.
 */
template<typename T>
static Vector3<T> operator -(const Vector3<T>& _Left, const Vector3<T>& _Right)
{
	return Vector3<T>(_Left.X - _Right.X, _Left.Y - _Right.Y, _Left.Z - _Right.Z);
}


/**
 * Représente un vecteur en 2D avec des composantes X, Y.
 * @tparam T Type numérique des composantes (ex: float, double, int).
 */
template<typename T>
struct Vector2
{

	T X = 0;
	T Y = 0;

	/**
	* Constructeur par défaut : initialise un vecteur nul (0, 0).
	*/
	Vector2() {}

	/**
	* Constructeur avec composantes : initialise un vecteur avec les valeurs X, Y données.
	* @param x Composante X.
	* @param y Composante Y.
	*/
	Vector2(T x, T y)
	{
		X = x;
		Y = y;
	}

	/**
	* Constructeur de copie depuis un autre Vector2 (de type potentiellement différent).
	* @tparam T Type du vecteur source.
	* @param _Vector2 Vecteur source à copier.
	*/
	template<typename T>
	Vector2(Vector2<T>& _Vector2)
	{
		X = _Vector2.X;
		Y = _Vector2.Y;
	}

	/**
	* Vérifie si le vecteur courant est colinéaire avec un autre vecteur.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Vecteur à comparer.
	* @return true si les vecteurs sont colinéaires, false sinon.
	*
	* @details
	* Deux vecteurs 2D sont colinéaires si : X * _Other.Y == Y * _Other.X.
	*/
	template<typename T>
	bool Collinear(const Vector2<T>& _Other)
	{
		if (X * _Other.Y == Y * _Other.X)
		{
			return true;
		}

		return false;
	}

	/**
	* Vérifie si deux vecteurs sont colinéaires (version statique).
	* @tparam T Type des vecteurs à comparer.
	* @param _VecA Premier vecteur.
	* @param _VecB Deuxième vecteur.
	* @return true si les vecteurs sont colinéaires, false sinon.
	*/
	template<typename T>
	static bool Collinear(const Vector2<T>& _VecA, const Vector2<T>& _VecB)
	{
		if (_VecA.X * _VecB.Y == _VecA.Y * _VecB.X)
		{
			return true;
		}

		return false;
	}

	/**
	* Calcule le produit scalaire entre le vecteur courant et un autre vecteur.
	* @tparam T Type du vecteur à multiplier.
	* @param _Other Vecteur à multiplier.
	* @return Produit scalaire (float).
	*/
	template<typename T>
	float DotProduct(const Vector2<T>& _Other)
	{
		return X * _Other.X + Y * _Other.Y;
	}

	/**
	* Calcule le produit vectoriel entre le vecteur courant et un autre vecteur 2D.
	* @tparam U Type du vecteur à multiplier.
	* @param _Other Vecteur à multiplier.
	* @return Vecteur 3D résultat du produit vectoriel (Z = 0 pour les vecteurs 2D).
	*
	* @details
	* En 2D, le produit vectoriel de deux vecteurs A et B est un scalaire :
	* A.X * B.Y - A.Y * B.X.
	* Ici, le résultat est retourné sous forme de Vector3 avec Z = 0.
	*/
	template<typename T>
	Vector3<T> CrossProduct(const Vector2<T>& _Other)
	{
		T vec2Z = 0;

		T x = Y * vec2Z - vec2Z * _Other.Y;
		T y = X * vec2Z - vec2Z * _Other.X;
		T z = Y * _Other.X - X * _Other.Y;

		return Vector3<T>(x, y, z);
	}

	/**
	* Calcule la longueur (norme) du vecteur.
	* @return Longueur du vecteur (float).
	*/
	template<typename T>
	float Lengh()
	{
		return sqrtf(powf(X, 2) + powf(Y, 2));
	}

	/**
	* Calcule le carré de la longueur du vecteur.
	* @return Carré de la longueur du vecteur (float).
	*/
	float SqrtLengh()
	{
		return powf(X, 2) + powf(Y, 2);
	}

	/**
	* Normalise le vecteur (le transforme en vecteur unitaire de longueur 1).
	* @tparam T Type du vecteur à retourner.
	* @return Nouveau vecteur normalisé.
	*/
	template<typename T>
	Vector2<T> Normalise()
	{
		float coef = 1 / Lengh();
		T x = X * coef;
		T y = Y * coef;

		return Vector2<T>(x, y);
	}

	/**
	* Calcule l'angle (en radians) entre le vecteur courant et un autre vecteur.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Pointeur vers l'autre vecteur.
	* @return Angle en radians (float).
	*/
	template<typename T>
	float AngleBetween(const Vector2<T>* _Other)
	{
		float dot = DotProduct(_Other);
		float thisLenght = Lengh();
		float otherLenght = _Other->Lengh();

		return acos(dot / (thisLenght * otherLenght));
	}

	/**
	 * Calcule l'angle (en radians) entre deux vecteurs (version statique).
	 * @tparam T Type des vecteurs à comparer.
	 * @param _VecA Premier vecteur.
	 * @param _VecB Deuxième vecteur.
	 * @return Angle en radians (float).
	 */
	template<typename T>
	static float AngleBetween(const Vector2<T>& _VecA, const Vector2<T>& _VecB)
	{
		float dot = DotProduct(_VecA, _VecB);
		float vecALenght = _VecA.Lengh();
		float vecBLenght = _VecB.Lengh();

		return acos(dot / (vecALenght * vecBLenght));
	}

	/**
	* Opérateur de conversion vers un Vector2 de type différent.
	* @tparam U Type cible du vecteur.
	* @return Nouveau vecteur de type Vector2<U> avec les composantes converties.
	*/
	template<typename U>
	operator Vector2<U>() const
	{
		return Vector2<U>(static_cast<U>(X), static_cast<U>(Y));
	}

	/**
	* Additionne deux vecteurs.
	* @tparam T Type du vecteur à ajouter.
	* @param _Other Vecteur à ajouter.
	* @return Nouveau vecteur résultat de l'addition.
	*/
	template<typename T>
	Vector2<T> operator +(const Vector2<T>& _Other)
	{
		return Vector2<T>(X + _Other.X, Y + _Other.Y);
	}

	/**
	* Soustrait deux vecteurs.
	* @tparam T Type du vecteur à soustraire.
	* @param _Other Vecteur à soustraire.
	* @return Nouveau vecteur résultat de la soustraction.
	*/
	template<typename T>
	Vector2<T> operator-(const Vector2<T>& _Other)
	{
		return Vector2<T>(X - _Other.X, Y - _Other.Y);
	}

	/**
	* Multiplie le vecteur par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Nouveau vecteur résultat de la multiplication.
	*/
	template<typename T>
	Vector2<T> operator*(T _Value)
	{
		return Vector2<T>(X * _Value, Y * _Value);
	}

	/**
	* Divise le vecteur par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Nouveau vecteur résultat de la division.
	*/
	template<typename T>
	Vector2<T> operator /(T _Value)
	{
		return Vector2<T>(X / _Value, Y / _Value);
	}

	/**
	* Ajoute un vecteur au vecteur courant.
	* @tparam T Type du vecteur à ajouter.
	* @param _Other Vecteur à ajouter.
	* @return Référence vers le vecteur courant.
	*
	* @note **⚠️ Bug** : Retourne `this` au lieu de `*this`.
	*/
	template<typename T>
	Vector2<T> operator +=(const Vector2<T>& _Other)
	{
		X += _Other.X;
		Y += _Other.Y;

		return this;
	}

	/**
	* Soustrait un vecteur du vecteur courant.
	* @tparam T Type du vecteur à soustraire.
	* @param _Other Vecteur à soustraire.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector2<T> operator -=(const Vector2<T>& _Other)
	{
		X -= _Other.X;
		Y -= _Other.Y;

		return this;
	}

	/**
	* Multiplie le vecteur courant par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector2<T> operator *=(T _Value)
	{
		X *= _Value;
		Y *= _Value;

		return this;
	}

	/**
	* Divise le vecteur courant par un scalaire.
	* @tparam T Type du scalaire.
	* @param _Value Scalaire.
	* @return Référence vers le vecteur courant.
	*/
	template<typename T>
	Vector2<T> operator /=(T _Value)
	{
		X /= _Value;
		Y /= _Value;

		return this;
	}

	/**
	* Vérifie si deux vecteurs sont égaux.
	* @tparam T Type du vecteur à comparer.
	* @param _Other Vecteur à comparer.
	* @return true si les vecteurs sont égaux, false sinon.
	*/
	template <typename T>
	bool operator ==(const Vector2<T>& _Other)
	{
		if (X == _Other.X)
		{
			if (Y == _Other.Y)
			{
				return true;
			}
		}

		return false;
	}


	/**
	 * Vérifie si deux vecteurs sont différents.
	 * @tparam T Type du vecteur à comparer.
	 * @param _Other Vecteur à comparer.
	 * @return true si les vecteurs sont différents, false sinon.
	 */
	template <typename T>
	bool operator !=(const Vector2<T>& _Other)
	{
		return !(this == _Other);
	}
};


// Define the most common types
typedef Vector2<int>          Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<float>        Vector2f;

typedef Vector3<int>          Vector3i;
typedef Vector3<unsigned int> Vector3u;
typedef Vector3<float>        Vector3f;
