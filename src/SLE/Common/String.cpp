#include "String.h"

#include "UTF.h"

#include <iterator>
#include <utility>

#include <cassert>
#include <cstring>
#include <cwchar>

void U8StringCharTraits::Assign(CharType& _Char1, CharType _Char2)
{
	_Char1 = _Char2;
}

U8StringCharTraits::CharType* U8StringCharTraits::Assign(CharType* _Str, std::size_t _Index, CharType _Char)
{
	return reinterpret_cast<CharType*>(std::char_traits<char>::assign(reinterpret_cast<char*>(_Str), _Index, static_cast<char>(_Char)));
}

bool U8StringCharTraits::Equal(CharType _Char1, CharType _Char2)
{
	return _Char1 == _Char2;
}

bool U8StringCharTraits::Lt(CharType _Char1, CharType _Char2)
{
	return _Char1 < _Char2;
}

U8StringCharTraits::CharType* U8StringCharTraits::Move(CharType* _Str1, const CharType* _Str2, std::size_t _Index)
{
	std::memmove(_Str1, _Str2, _Index);
	return _Str1;
}

U8StringCharTraits::CharType* U8StringCharTraits::Copy(CharType* _Str1, const CharType* _Str2, std::size_t _Index)
{
	std::memcpy(_Str1, _Str2, _Index);
	return _Str1;
}

int U8StringCharTraits::Compare(const CharType* _Str1, const CharType* _Str2, std::size_t _Index)
{
	return std::memcmp(_Str1, _Str2, _Index);
}

std::size_t U8StringCharTraits::Length(const CharType* _Str)
{
	return std::strlen(reinterpret_cast<const char*>(_Str));
}

const U8StringCharTraits::CharType* U8StringCharTraits::Find(const CharType* _Str, std::size_t _Index, const CharType& _Char)
{
	return reinterpret_cast<const CharType*>(std::char_traits<char>::find(reinterpret_cast<const char*>(_Str), _Index, static_cast<char>(_Char)));
}

U8StringCharTraits::CharType U8StringCharTraits::ToCharType(IntType _Int) noexcept
{
	return static_cast<U8StringCharTraits::CharType>(std::char_traits<char>::to_char_type(_Int));
}

U8StringCharTraits::IntType U8StringCharTraits::ToIntType(CharType _Char) noexcept
{
	return std::char_traits<char>::to_int_type(static_cast<char>(_Char));
}

bool U8StringCharTraits::EqualIntType(IntType _Int1, IntType _Int2) noexcept
{
	return _Int1 == _Int2;
}

U8StringCharTraits::IntType U8StringCharTraits::Eof() noexcept
{
	return std::char_traits<char>::eof();
}

U8StringCharTraits::IntType U8StringCharTraits::NotEof(IntType _Int) noexcept
{
	return std::char_traits<char>::not_eof(_Int);
}

String::String(char _ANSIChar, const std::locale& _Locale) : m_String(1, UTF32::DecodeANSI(_ANSIChar, _Locale))
{
}

String::String(wchar_t _WideChar) : m_String(1, UTF32::DecodeWIDE(_WideChar))
{
}

String::String(char32_t _UTF32Char) : m_String(1, _UTF32Char)
{
}

String::String(const char* _ANSIString, const std::locale& _Locale)
{
	if (_ANSIString)
	{
		const std::size_t length = std::strlen(_ANSIString);
		if (length > 0)
		{
			m_String.reserve(length + 1);
			UTF32::FromANSI(_ANSIString, _ANSIString + length, std::back_inserter(m_String), _Locale);
		}
	}
}

String::String(const std::string& _ANSIString, const std::locale& _Locale)
{
	m_String.reserve(_ANSIString.length() + 1);
	UTF32::FromANSI(_ANSIString.begin(), _ANSIString.end(), std::back_inserter(m_String), _Locale);
}

String::String(const wchar_t* _WideString)
{
	if (_WideString)
	{
		const std::size_t length = std::wcslen(_WideString);

		if (length > 0)
		{
			m_String.reserve(length + 1);
			UTF32::FromWIDE(_WideString, _WideString + length, std::back_inserter(m_String));
		}
	}
}

String::String(const std::wstring& _WideString)
{
	m_String.reserve(_WideString.length() + 1);
	UTF32::FromWIDE(_WideString.begin(), _WideString.end(), std::back_inserter(m_String));
}

String::String(const char32_t* _UTF32String) : m_String(_UTF32String ? _UTF32String : U"")
{
}

String::String(std::u32string _UTF32String) : m_String(std::move(_UTF32String))
{
}

String::operator std::string() const
{
	return ToANSIString();
}


String::operator std::wstring() const
{
	return ToWideString();
}

std::string String::ToANSIString(const std::locale& _Locale) const
{
	std::string outStr;
	outStr.reserve(m_String.length() + 1);

	UTF32::ToANSI(m_String.begin(), m_String.end(), std::back_inserter(outStr), 0, _Locale);

	return std::string();
}

std::wstring String::ToWideString() const
{
	std::wstring outStr;
	outStr.reserve(m_String.length() + 1);

	UTF32::ToWIDE(m_String.begin(), m_String.end(), std::back_inserter(outStr), 0);
	
	return outStr;
}

U8String String::ToUTF8() const
{
	U8String outStr;
	outStr.reserve(m_String.length());

	UTF32::ToUTF8(m_String.begin(), m_String.end(), std::back_inserter(outStr));
	
	return outStr;
}

std::u16string String::ToUTF16() const
{
	std::u16string outStr;
	outStr.reserve(m_String.length());

	UTF16::ToUTF16(m_String.begin(), m_String.end(), std::back_inserter(outStr));

	return outStr;
}

std::u32string String::ToUTF32() const
{
	return m_String;
}

String& String::operator+=(const String& _Right)
{
	m_String += _Right.m_String;

	return *this;
}

char32_t String::operator[](std::size_t _Index) const
{
	assert(_Index < m_String.size() && "Index is out of bounds");
	return m_String[_Index];
}


////////////////////////////////////////////////////////////
char32_t& String::operator[](std::size_t _Index)
{
	assert(_Index < m_String.size() && "Index is out of bounds");
	return m_String[_Index];
}

void String::Clear()
{
	m_String.clear();
}

std::size_t String::GetSize() const
{
	return m_String.size();
}

bool String::IsEmpty() const
{
	return m_String.empty();
}

void String::Erase(std::size_t _Position, std::size_t _Count)
{
	m_String.erase(_Position, _Count);
}

void String::Insert(std::size_t _Position, const String& _Str)
{
	m_String.insert(_Position, _Str.m_String);
}

std::size_t String::Find(const String& _Str, std::size_t _Start) const
{
	return m_String.find(_Str.m_String, _Start);
}

void String::Replace(std::size_t _Position, std::size_t _Length, const String& _ReplaceWith)
{
	m_String.replace(_Position, _Length, _ReplaceWith.m_String);
}

void String::Replace(const String& _SearchFor, const String& _ReplaceWith)
{
	const std::size_t step = _ReplaceWith.GetSize();
	const std::size_t len = _SearchFor.GetSize();
	std::size_t pos = Find(_SearchFor);

	while (pos != InvalidPos)
	{
		Replace(pos, len, _ReplaceWith);
		pos = Find(_SearchFor, pos + step);
	}
}

String String::SubString(std::size_t _Position, std::size_t _Legth) const
{
	return m_String.substr(_Position, _Legth);
}

const char32_t* String::GetData() const
{
	return m_String.c_str();
}

String::Iterator String::Begin()
{
	return m_String.begin();
}

String::ConstIterator String::Begin() const
{
	return m_String.begin();
}

String::Iterator String::End()
{
	return m_String.end();
}

String::ConstIterator String::End() const
{
	return m_String.end();
}

bool operator==(const String& _Left, const String& _Right)
{
	return _Left.m_String == _Right.m_String;
}

bool operator<(const String& _Left, const String& _Right)
{
	return _Left.m_String < _Right.m_String;
}
