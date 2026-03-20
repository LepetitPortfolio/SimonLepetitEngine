#pragma once
#include <math.h>


template<typename T>
struct Vector3
{

	T X = 0;
	T Y = 0;
	T Z = 0;

	Vector3() {}

	Vector3(T x, T y, T z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	template<typename T>
	Vector3(Vector3<T>& _Vector3)
	{
		X = _Vector3.X;
		Y = _Vector3.Y;
		Z = _Vector3.Z;
	}

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

	/*
	* Dot Product = X1 * X2 + Y1 * Y2 + Z1 * Z2
	*/
	template<typename T>
	float DotProduct(const Vector3<T>& _Other)
	{
		return X * _Other.X + Y * _Other.Y + Z * _Other.Z;
	}

	/*
	* Dot Product = X1 * X2 + Y1 * Y2 + Z1 * Z2
	*/
	template<typename T>
	static float DotProduct(const Vector3<T>& _VecA, const Vector3<T>& _VecB)
	{
		return _VecA.X * _VecB.X + _VecA.Y * _VecB.Y + _VecA.Z * _VecB.Z;
	}

	/*
	* X = Y1 * Z2 - Z1 * Y2
	* Y = X1 * Z2 - Z1 * X2
	* Z = Y1 * X2 - X1 * Y2
	*/
	template<typename T>
	Vector3<T> CrossProduct(const Vector3<T>* _Other)
	{
		T x = Y * _Other->z - Z * _Other->Y;
		T y = X * _Other->Z - Z * _Other->X;
		T z = Y * _Other->X - X * _Other->Y;

		return Vector3<T>(x, y, z);
	}

	/*
	* \  ____________
	*  \/x² + y² + z²
	*/
	template<typename T>
	float Lengh()
	{
		return sqrtf(powf(X, 2) + powf(Y, 2) + powf(Z, 2));
	}

	float SqrtLengh()
	{
		return powf(X, 2) + powf(Y, 2) + powf(Z, 2);
	}

	template<typename T>
	Vector3<T> Normalise()
	{
		float coef = 1 / Lengh();
		T x = X * coef;
		T y = Y * coef;
		T z = Z * coef;

		return Vector3<T>(x, y, z);
	}

	/*
	*           V1 . V2
	* cos O = --------------
	*          |V1| . |V2|
	*/
	template<typename T>
	float AngleBetween(const Vector3<T>* _Other)
	{
		float dot = DotProduct(_Other);
		float thisLenght = Lengh();
		float otherLenght = _Other->Lengh();

		return acos(dot / (thisLenght * otherLenght));
	}

	/*
	*           V1 . V2
	* cos O = --------------
	*          |V1| . |V2|
	*/
	template<typename T>
	static float AngleBetween(const Vector3<T>& _VecA, const Vector3<T>& _VecB)
	{
		float dot = DotProduct(_VecA, _VecB);
		float vecALenght = _VecA->Lengh();
		float vecBLenght = _VecB->Lengh();

		return acos(dot / (vecALenght * vecBLenght));
	}

	template<typename T>
	Vector3<T> operator +(const Vector3<T>& _Other)
	{
		return Vector3<T>(X + _Other.X, Y + _Other.Y, Z + _Other.Z);
	}

	template<typename T>
	Vector3<T> operator -(const Vector3<T>& _Other)
	{
		return Vector3<T>(X - _Other.X, Y - _Other.Y, Z - _Other.Z);
	}



	template<typename T>
	Vector3<T> operator *(T _Value)
	{
		return Vector3<T>(X * _Value, Y * _Value, Z * _Value);
	}

	template<typename T>
	Vector3<T> operator /(T _Value)
	{
		return Vector3<T>(X / _Value, Y / _Value, Z / _Value);
	}

	template<typename T>
	Vector3<T> operator +=(const Vector3<T>& _Other)
	{
		X += _Other.X;
		Y += _Other.Y;
		Z += _Other.Z;

		return this;
	}

	template<typename T>
	Vector3<T> operator -=(const Vector3<T>& _Other)
	{
		X -= _Other.X;
		Y -= _Other.Y;
		Z -= _Other.Z;

		return this;
	}

	template<typename T>
	Vector3<T> operator *=(T _Value)
	{
		X *= _Value;
		Y *= _Value;
		Z *= _Value;

		return this;
	}

	template<typename T>
	Vector3<T> operator /=(T _Value)
	{
		X /= _Value;
		Y /= _Value;
		Z /= _Value;

		return this;
	}

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

	template <typename T>
	bool operator !=(const Vector3<T>& _Other)
	{
		return !(this == _Other);
	}

};


template<typename T>
static Vector3<T> operator -(const Vector3<T>& _Left, const Vector3<T>& _Right)
{
	return Vector3<T>(_Left.X - _Right.X, _Left.Y - _Right.Y, _Left.Z - _Right.Z);
}

template<typename T>
struct Vector2
{

	T X = 0;
	T Y = 0;

	Vector2() {}

	Vector2(T x, T y)
	{
		X = x;
		Y = y;
	}

	template<typename T>
	Vector2(Vector2<T>& _Vector2)
	{
		X = _Vector2.X;
		Y = _Vector2.Y;
	}

	template<typename T>
	bool Collinear(const Vector2<T>& _Other)
	{
		if (X * _Other.Y == Y * _Other.X)
		{
			return true;
		}

		return false;
	}


	template<typename T>
	static bool Collinear(const Vector2<T>& _VecA, const Vector2<T>& _VecB)
	{
		if (_VecA.X * _VecB.Y == _VecA.Y * _VecB.X)
		{
			return true;
		}

		return false;
	}

	/*
	* Dot Product = X1 * X2 + Y1 * Y2
	*/
	template<typename T>
	float DotProduct(const Vector2<T>& _Other)
	{
		return X * _Other.X + Y * _Other.Y;
	}

	/*
	* X = Y1 * 0 - 0 * Y2
	* Y = X1 * 0 - 0 * X2
	* Z = Y1 * X2 - X1 * Y2
	*/
	template<typename T>
	Vector3<T> CrossProduct(const Vector2<T>& _Other)
	{
		T vec2Z = 0;

		T x = Y * vec2Z - vec2Z * _Other->Y;
		T y = X * vec2Z - vec2Z * _Other->X;
		T z = Y * _Other->X - X * _Other->Y;

		return Vector3<T>(x, y, z);
	}

	/*
	* \  ________
	*  \/x² + y²
	*/
	template<typename T>
	float Lengh()
	{
		return sqrtf(powf(X, 2) + powf(Y, 2));
	}

	float SqrtLengh()
	{
		return powf(X, 2) + powf(Y, 2);
	}

	template<typename T>
	Vector2<T> Normalise()
	{
		float coef = 1 / Lengh();
		T x = X * coef;
		T y = Y * coef;

		return Vector2<T>(x, y);
	}

	/*
	*           V1 . V2
	* cos O = --------------
	*          |V1| . |V2|
	*/
	template<typename T>
	float AngleBetween(const Vector2<T>* _Other)
	{
		float dot = DotProduct(_Other);
		float thisLenght = Lengh();
		float otherLenght = _Other->Lengh();

		return acos(dot / (thisLenght * otherLenght));
	}

	/*
	*           V1 . V2
	* cos O = --------------
	*          |V1| . |V2|
	*/
	template<typename T>
	static float AngleBetween(const Vector2<T>& _VecA, const Vector2<T>& _VecB)
	{
		float dot = DotProduct(_VecA, _VecB);
		float vecALenght = _VecA->Lengh();
		float vecBLenght = _VecB->Lengh();

		return acos(dot / (vecALenght * vecBLenght));
	}


	template<typename T>
	Vector2<T> operator +(const Vector2<T>& _Other)
	{
		return Vector2<T>(X + _Other.X, Y + _Other.Y);
	}

	template<typename T>
	Vector2<T> operator-(const Vector2<T>& _Other)
	{
		return Vector2<T>(X - _Other.X, Y - _Other.Y);
	}

	template<typename T>
	Vector2<T> operator*(T _Value)
	{
		return Vector2<T>(X * _Value, Y * _Value);
	}

	template<typename T>
	Vector2<T> operator /(T _Value)
	{
		return Vector2<T>(X / _Value, Y / _Value);
	}

	template<typename T>
	Vector2<T> operator +=(const Vector2<T>& _Other)
	{
		X += _Other.X;
		Y += _Other.Y;

		return this;
	}

	template<typename T>
	Vector2<T> operator -=(const Vector2<T>& _Other)
	{
		X -= _Other.X;
		Y -= _Other.Y;

		return this;
	}

	template<typename T>
	Vector2<T> operator *=(T _Value)
	{
		X *= _Value;
		Y *= _Value;

		return this;
	}

	template<typename T>
	Vector2<T> operator /=(T _Value)
	{
		X /= _Value;
		Y /= _Value;

		return this;
	}

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
