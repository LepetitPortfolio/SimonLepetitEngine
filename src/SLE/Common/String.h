#pragma once
#include "UTF.h"

#include <locale>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>

struct  U8StringCharTraits
{
    using CharType = std::uint8_t;
    using IntType = std::char_traits<char>::int_type;
    using OffType = std::char_traits<char>::off_type;
    using PosType = std::char_traits<char>::pos_type;
    using StateType = std::char_traits<char>::state_type;

    static void Assign(CharType& _Char1, CharType _Char2);
    static CharType* Assign(CharType* _Str, std::size_t _Index, CharType _Char);
    static bool Equal(CharType _Char1, CharType _Char2);
    static bool Lt(CharType _Char1, CharType _Char2);
    static CharType* Move(CharType* _Str1, const CharType* _Str2, std::size_t _Index);
    static CharType* Copy(CharType* _Str1, const CharType* _Str2, std::size_t _Index);
    static int Compare(const CharType* _Str1, const CharType* _Str2, std::size_t _Index);
    static std::size_t Length(const CharType* _Str);
    static const CharType* Find(const CharType* _Str, std::size_t _Index, const CharType& _Char);
    static CharType ToCharType(IntType _Int) noexcept;
    static IntType ToIntType(CharType _Char) noexcept;
    static bool EqualIntType(IntType _Int1, IntType _Int2) noexcept;
    static IntType Eof() noexcept;
    static IntType NotEof(IntType _Int) noexcept;
};

using U8String = std::basic_string<std::uint8_t>;
//using U8String = std::basic_string<std::uint8_t, U8StringCharTraits>;


class String
{
public:

    using Iterator = std::u32string::iterator;
    using ConstIterator = std::u32string::const_iterator;

    static inline const std::size_t InvalidPos{ std::u32string::npos };

	String() = default;
    String(std::nullptr_t, const std::locale & = {}) = delete;
    String(char _ANSIChar, const std::locale& _Locale = {});
    String(wchar_t _WideChar);
    String(char32_t _UTF32Char);
    String(const char* _ANSIString, const std::locale& _Locale = {});
    String(const std::string& _ANSIString, const std::locale& _Locale = {});
    String(const wchar_t* _WideString);
    String(const std::wstring& _WideString);
    String(const char32_t* _UTF32String);
    String(std::u32string _UTF32String);

    template <typename T>
    static String FromUTF8(T _Begin, T _End);

    template <typename T>
    static String FromUTF16(T _Begin, T _End);

    template <typename T>
    static String FromUTF32(T _Begin, T _End);

    operator std::string() const;
    operator std::wstring() const;

    std::string ToANSIString(const std::locale& _Locale = {}) const;
    
    std::wstring ToWideString() const;

    U8String ToUTF8() const;

    std::u16string ToUTF16() const;
    
    std::u32string ToUTF32() const;

    String& operator+= (const String& _Right);
    String operator+ (const String& _Right);

    char32_t operator[](std::size_t _Index) const;
    char32_t& operator[](std::size_t _Index);

    void Clear();

    std::size_t GetSize() const;

    bool IsEmpty() const;

    void Erase(std::size_t _Position, std::size_t _Count = 1);

    void Insert(std::size_t _Position, const String& _Str);

    std::size_t Find(const String& _Str, std::size_t _Start = 0) const;

    void Replace(std::size_t _Position, std::size_t _Length, const String& _ReplaceWith);
    void Replace(const String& _SearchFor, const String& _ReplaceWith);

    String SubString(std::size_t _Position, std::size_t _Legth = InvalidPos) const;

    static std::vector<String> Split(const String& _Str, const String& _Delimiter);
    std::vector<String> Split(const String& _Delimiter);
    

    const char32_t* GetData() const;

    Iterator Begin();

    ConstIterator Begin() const;

    Iterator End();

    ConstIterator End() const;

private:

    std::u32string m_String;

    friend bool operator==(const String& _Left, const String& _Right);
    friend bool operator<(const String& _Left, const String& _Right);
};

template<typename T>
inline String String::FromUTF8(T _Begin, T _End)
{
    String outStr;
    UTF8::ToUTF32(_Begin, _End, std::back_inserter(outStr.m_String));
    return outStr;
}

template<typename T>
inline String String::FromUTF16(T _Begin, T _End)
{
    String outStr;
    UTF16::ToUTF32(_Begin, _End, std::back_inserter(outStr.m_String));
    return outStr;
}

template<typename T>
inline String String::FromUTF32(T _Begin, T _End)
{
    String outStr;
    outStr.m_String.assign(_Begin, _End);
    return outStr;
}
