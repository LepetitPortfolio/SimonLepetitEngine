#pragma once
#include <math.h>
#include "Vector.h"

#define PI 3.14159265359f
#define DegToRad PI/360

namespace MyMath
{

	/**
	* Structure représentant un quaternion, utilisé pour les rotations en 3D.
	* @tparam T Type numérique (float, double, etc.) utilisé pour les composantes I, J, K, W.
	*
	* Un quaternion est défini par 4 composantes : I, J, K (partie vectorielle) et W (partie scalaire).
	* Forme générale : q = W + I*i + J*j + K*k.
	*/
	template<typename T>
	struct Quaternion
	{
		T I = 0;
		T J = 0;
		T K = 0;
		T W = 0;

		/**
		* Constructeur par défaut : initialise un quaternion nul (0, 0, 0, 0).
		*/
		Quaternion() {}

		/**
		* Constructeur avec paramètres : initialise un quaternion avec les valeurs données.
		* @param _I Composante i.
		* @param _J Composante j.
		* @param _K Composante k.
		* @param _W Composante scalaire w.
		*/
		Quaternion(T _I, T _J, T _K, T _W)
		{
			I = _I;
			J = _J;
			K = _K;
			W = _W;
		}

		/**
		* Calcule la norme (longueur) d'un quaternion.
		* @tparam T Type du quaternion.
		* @param _Quat Pointeur vers le quaternion dont on veut calculer la norme.
		* @return Norme du quaternion (float).
		*
		* @details
		* La norme est calculée comme : sqrt(I² + J² + K² + W²).
		* Cette fonction est statique et prend un pointeur en paramètre.
		*/
		template<typename T>
		static float Normalise(const Quaternion<T>* _Quat)
		{
			return sqrtf(powf(_Quat->I, 2) + powf(_Quat->J, 2) + powf(_Quat->K, 2) + powf(_Quat->W, 2));
		}

		/**
		* Calcule la norme (longueur) du quaternion courant.
		* @return Norme du quaternion (float).
		*/
		float Lengh()
		{
			return sqrtf(powf(I, 2) + powf(J, 2) + powf(K, 2) + powf(W, 2));
		}

		/**
		* Calcule le produit scalaire entre le quaternion courant et un autre quaternion.
		* @tparam T Type du quaternion.
		* @param _Other Pointeur vers l'autre quaternion.
		* @return Produit scalaire (float).
		*
		* @details
		* Le produit scalaire est calculé comme : I1*I2 + J1*J2 + K1*K2 + W1*W2.
		*/
		template<typename T>
		float DotProduct(const Quaternion<T>* _Other)
		{
			return I * _Other->I + J * _Other->J + K * _Other->K + W * _Other->W;
		}

		/**
		* Calcule le produit scalaire entre deux quaternions (version statique).
		* @tparam T Type des quaternions.
		* @param _QuatA Premier quaternion.
		* @param _QuatB Deuxième quaternion.
		* @return Produit scalaire (float).
		*
		* @note Cette version semble contenir une erreur : `_QuatA->I` devrait être `_QuatA.I` (accès via `.` et non `->`).
		*/
		template<typename T>
		static float DotProduct(const Quaternion<T>& _QuatA, const Quaternion<T>& _QuatB)
		{
			return _QuatA->I * _QuatB->I + _QuatA->J * _QuatB->J + _QuatA->K * _QuatB->K + _QuatA->W * _QuatB->W;
		}

		/**
		* Normalise le quaternion courant.
		* @tparam T Type du quaternion.
		* @return Nouveau quaternion normalisé.
		*
		* @details
		* Divise chaque composante par la norme du quaternion pour obtenir un quaternion de norme 1.
		*/
		template<typename T>
		Quaternion<T> Normalise()
		{
			float coef = 1 / Lengh();
			T i = I * coef;
			T j = J * coef;
			T k = K * coef;
			T w = W * coef;

			return Quaternion<T>(i, j, k, w);
		}

		/**
		* Calcule le quaternion inverse (conjugué) d'un quaternion donné.
		* @tparam T Type du quaternion.
		* @param _Quat Pointeur vers le quaternion à inverser.
		* @return Nouveau quaternion inversé (I, J, K négatifs, W inchangé).
		*/
		template<typename T>
		static Quaternion Reverse(const Quaternion<T>* _Quat)
		{
			return Quaternion<T>(-_Quat->I, -_Quat->J, -_Quat->K, _Quat->W);
		}

		/**
		* Calcule le quaternion inverse (conjugué) du quaternion courant.
		* @tparam T Type du quaternion.
		* @return Nouveau quaternion inversé.
		*/
		template<typename T>
		Quaternion Reverse()
		{
			return Quaternion<T>(-I, -J, -K, W);
		}

		/**
		* Crée un quaternion à partir d'un angle et d'un axe de rotation.
		* @tparam T Type du quaternion.
		* @param _AngleInDeg Angle de rotation en degrés.
		* @param _Axis Axe de rotation (Vector3<T>).
		* @return Quaternion normalisé représentant la rotation.
		*
		* @details
		* Utilise la formule : q = (sin(θ/2) * axe, cos(θ/2)).
		* Convertit l'angle en radians via `DegToRad`.
		* Normalise le quaternion avant de le retourner.
		*
		* @note La fonction semble contenir une erreur : `Normalise(new Quaternion(i, j, k, w))` devrait probablement être `Normalise(Quaternion(i, j, k, w))`.
		*/
		template<typename T>
		static Quaternion<T> AngleAxis(float _AngleInDeg, Vector3<T> _Axis)
		{
			T sinTeta = std::sin((_AngleInDeg * DegToRad) / 2);
			T cosTeta = std::cos((_AngleInDeg * DegToRad) / 2);

			T w = cosTeta;
			T i = _Axis.X * sinTeta;
			T j = _Axis.Y * sinTeta;
			T k = _Axis.Z * sinTeta;

			return Normalise(new Quaternion(i, j, k, w));
		}

		/**
		* Additionne deux quaternions.
		* @tparam T Type des quaternions.
		* @param _Other Deuxième quaternion.
		* @return Nouveau quaternion résultat de l'addition.
		*/
		template<typename T>
		Quaternion<T> operator +(const Quaternion<T>& _Other)
		{
			return Quaternion(I + _Other.I, J + _Other.J, K + _Other.K, W + _Other.W);
		}

		/**
		* Soustrait deux quaternions.
		* @tparam T Type des quaternions.
		* @param _Other Deuxième quaternion.
		* @return Nouveau quaternion résultat de la soustraction.
		*/
		template<typename T>
		Quaternion<T> operator -(const Quaternion<T>& _Other)
		{
			return Quaternion(I - _Other.I, J - _Other.J, K - _Other.K, W - _Other.W);
		}

		/**
		* Multiplie un quaternion par un scalaire.
		* @tparam T Type du quaternion.
		* @param _Value Scalaire.
		* @return Nouveau quaternion résultat de la multiplication.
		*/
		template<typename T>
		Quaternion<T> operator *(T _Value)
		{
			return Quaternion<T>(I * _Value, J * _Value, K * _Value, W * _Value);
		}

		/**
		* Divise un quaternion par un scalaire.
		* @tparam T Type du quaternion.
		* @param _Value Scalaire.
		* @return Nouveau quaternion résultat de la division.
		*/
		template<typename T>
		Quaternion<T> operator /(T _Value)
		{
			return Quaternion<T>(I / _Value, J / _Value, K / _Value, W / _Value);
		}

		/**
		* Additionne un quaternion au quaternion courant.
		* @tparam T Type des quaternions.
		* @param _Other Deuxième quaternion.
		* @return Référence vers le quaternion courant (modifié).
		*
		* @note Cette fonction semble contenir une erreur : elle retourne `this` au lieu de `*this`.
		*/
		template<typename T>
		Quaternion<T> operator +=(const Quaternion<T>& _Other)
		{
			I += _Other.I;
			J += _Other.J;
			K += _Other.K;
			W += _Other.W;

			return this;
		}

		/**
		* Soustrait un quaternion du quaternion courant.
		* @tparam T Type des quaternions.
		* @param _Other Deuxième quaternion.
		* @return Référence vers le quaternion courant (modifié).
		*
		* @note Cette fonction semble contenir une erreur : elle retourne `this` au lieu de `*this`.
		*/
		template<typename T>
		Quaternion<T> operator -=(const Quaternion<T>& _Other)
		{
			I -= _Other.I;
			J -= _Other.J;
			K -= _Other.K;
			W -= _Other.W;

			return this;
		}

		/**
		* Multiplie le quaternion courant par un scalaire.
		* @tparam T Type du quaternion.
		* @param _Value Scalaire.
		* @return Référence vers le quaternion courant (modifié).
		*
		* @note Cette fonction semble contenir une erreur : elle retourne `this` au lieu de `*this`.
		*/
		template<typename T>
		Quaternion<T> operator *=(T _Value)
		{
			I *= _Value;
			J *= _Value;
			K *= _Value;
			W *= _Value;

			return this;
		}

		/**
		* Divise le quaternion courant par un scalaire.
		* @tparam T Type du quaternion.
		* @param _Value Scalaire.
		* @return Référence vers le quaternion courant (modifié).
		*
		* @note Cette fonction semble contenir une erreur : elle retourne `this` au lieu de `*this`.
		*/
		template<typename T>
		Quaternion<T> operator /=(T _Value)
		{
			I /= _Value;
			J /= _Value;
			K /= _Value;
			W /= _Value;

			return this;
		}

		/**
		* Vérifie si deux quaternions sont égaux.
		* @tparam T Type des quaternions.
		* @param _Other Deuxième quaternion.
		* @return true si toutes les composantes sont égales, false sinon.
		*/
		template <typename T>
		bool operator ==(const Quaternion<T>& _Other)
		{
			if (I == _Other.I)
			{
				if (J == _Other.J)
				{
					if (K == _Other.K)
					{
						if (W == _Other.W)
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		/**
		* Vérifie si deux quaternions sont différents.
		* @tparam T Type des quaternions.
		* @param _Other Deuxième quaternion.
		* @return true si au moins une composante est différente, false sinon.
		*/
		template <typename T>
		bool operator !=(const Quaternion<T>& _Other)
		{
			return !(this == _Other);
		}

	};
}

// Define the most common types
typedef MyMath::Quaternion<int>          Quaternioni;
typedef MyMath::Quaternion<unsigned int> Quaternionu;
typedef MyMath::Quaternion<float>        Quaternionf;