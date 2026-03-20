#pragma once

#include <array>
#include <locale>

#include <cstdint>
#include <cstdlib>

class UTF
{
public:

	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	template<typename T>
	static T Next(T _Begin, T _End);

	template<typename T>
	static std::size_t Count(T _Begin, T _End);

	template<typename In, typename Out>
	static Out FromANSI(In _Begin, In _End, Out _Output, std::locale& _Locale = {});

	template<typename In, typename Out>
	static Out FromWIDE(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out FromLATIN1(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToANSI(In _Begin, In _End, Out _Output, char _Replacement = 0, std::locale& _Locale = {});

	template<typename In, typename Out>
	static Out ToWIDE(In _Begin, In _End, Out _Output, wchar_t _Replacement = 0);

	template<typename In, typename Out>
	static Out ToLATIN1(In _Begin, In _End, Out _Output, char _Replacement = 0);

	template<typename In, typename Out>
	static Out ToUTF8(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToUTF16(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToUTF32(In _Begin, In _End, Out _Output);

protected:
	template<typename InIt, typename OutIt>
	static OutIt Copy(InIt _First, InIt _Last, OutIt _Ouput);

};

class UTF8 : public UTF
{
public:

	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	template<typename T>
	static T Encode(char32_t _Input, T _Output, std::uint8_t _Replacement = 0);

	template<typename In, typename Out>
	static Out FromLATIN1(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToLATIN1(In _Begin, In _End, Out _Output, char _Replacement = 0);

	template<typename In, typename Out>
	static Out ToUTF16(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToUTF32(In _Begin, In _End, Out _Output);
};

class UTF16 : public UTF
{
public:

	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	template<typename T>
	static T Encode(char32_t _Input, T _Output, std::uint16_t _Replacement = 0);

	template<typename In, typename Out>
	static Out ToUTF8(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToUTF32(In _Begin, In _End, Out _Output);
};

class UTF32
{
public:

	template<typename T>
	static T Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0);

	template<typename T>
	static T Encode(char32_t _Input, T _Output, std::uint32_t _Replacement = 0);

	template<typename T>
	static T Next(T _Begin, T _End);

	template<typename T>
	static std::size_t Count(T _Begin, T _End);

	template<typename In, typename Out>
	static Out FromANSI(In _Begin, In _End, Out _Output, std::locale& _Locale = {});

	template<typename In, typename Out>
	static Out FromWIDE(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToANSI(In _Begin, In _End, Out _Output, char _Replacement = 0, std::locale& _Locale = {});

	template<typename In, typename Out>
	static Out ToWIDE(In _Begin, In _End, Out _Output, wchar_t _Replacement = 0);

	template<typename In, typename Out>
	static Out ToUTF8(In _Begin, In _End, Out _Output);

	template<typename In, typename Out>
	static Out ToUTF16(In _Begin, In _End, Out _Output);

	template<typename T>
	static char32_t DecodeANSI(T _Input, const std::locale& _Locale = {});

	template<typename T>
	static char32_t DecodeWIDE(T _Input);

	template<typename T>
	static T EncodeANSI(char32_t _Codepoint, T _Output, char _Remplacement = 0, const std::locale& _Locale = {});

	template<typename T>
	static T EncodeWIDE(char32_t _Codepoint, T _Output, wchar_t _Replacement = 0);
};

