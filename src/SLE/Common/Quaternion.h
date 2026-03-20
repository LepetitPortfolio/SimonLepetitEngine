#pragma once
#include <math.h>
#include "Vector.h"

#define PI 3.14159265359f
#define DegToRad PI/360

namespace MyMath
{

	template<typename T>
	struct Quaternion
	{
		T I = 0;
		T J = 0;
		T K = 0;
		T W = 0;

		Quaternion() {}
		Quaternion(T _I, T _J, T _K, T _W)
		{
			I = _I;
			J = _J;
			K = _K;
			W = _W;
		}

		template<typename T>
		static float Normalise(const Quaternion<T>* _Quat)
		{
			return sqrtf(powf(_Quat->I, 2) + powf(_Quat->J, 2) + powf(_Quat->K, 2) + powf(_Quat->W, 2));
		}

		float Lengh()
		{
			return sqrtf(powf(I, 2) + powf(J, 2) + powf(K, 2) + powf(W, 2));
		}

		/*
		* Dot Product = I1 * I2 + J1 * J2 + K1 * K2 + W1 * W2
		*/
		template<typename T>
		float DotProduct(const Quaternion<T>* _Other)
		{
			return I * _Other->I + J * _Other->J + K * _Other->K + W * _Other->W;
		}

		/*
		* Dot Product = I1 * I2 + J1 * J2 + K1 * K2 + W1 * W2
		*/
		template<typename T>
		static float DotProduct(const Quaternion<T>& _QuatA, const Quaternion<T>& _QuatB)
		{
			return _QuatA->I * _QuatB->I + _QuatA->J * _QuatB->J + _QuatA->K * _QuatB->K + _QuatA->W * _QuatB->W;
		}

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

		template<typename T>
		static Quaternion Reverse(const Quaternion<T>* _Quat)
		{
			return Quaternion<T>(-_Quat->I, -_Quat->J, -_Quat->K, _Quat->W);
		}

		template<typename T>
		Quaternion Reverse()
		{
			return Quaternion<T>(-I, -J, -K, W);
		}

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

		template<typename T>
		Quaternion<T> operator +(const Quaternion<T>& _Other)
		{
			return Quaternion(I + _Other.I, J + _Other.J, K + _Other.K, W + _Other.W);
		}

		template<typename T>
		Quaternion<T> operator -(const Quaternion<T>& _Other)
		{
			return Quaternion(I - _Other.I, J - _Other.J, K - _Other.K, W - _Other.W);
		}

		template<typename T>
		Quaternion<T> operator *(T _Value)
		{
			return Quaternion<T>(I * _Value, J * _Value, K * _Value, W * _Value);
		}

		template<typename T>
		Quaternion<T> operator /(T _Value)
		{
			return Quaternion<T>(I / _Value, J / _Value, K / _Value, W / _Value);
		}

		template<typename T>
		Quaternion<T> operator +=(const Quaternion<T>& _Other)
		{
			I += _Other.I;
			J += _Other.J;
			K += _Other.K;
			W += _Other.W;

			return this;
		}

		template<typename T>
		Quaternion<T> operator -=(const Quaternion<T>& _Other)
		{
			I -= _Other.I;
			J -= _Other.J;
			K -= _Other.K;
			W -= _Other.W;

			return this;
		}

		template<typename T>
		Quaternion<T> operator *=(T _Value)
		{
			I *= _Value;
			J *= _Value;
			K *= _Value;
			W *= _Value;

			return this;
		}

		template<typename T>
		Quaternion<T> operator /=(T _Value)
		{
			I /= _Value;
			J /= _Value;
			K /= _Value;
			W /= _Value;

			return this;
		}

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