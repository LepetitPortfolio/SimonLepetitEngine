#include "UTF.h"

////////////////////////////////////////////////////// UTF

template<typename InIt, typename OutIt>
OutIt UTF::Copy(InIt _First, InIt _Last, OutIt _Ouput)
{
    while (_First != _Last)
    {
        *_Ouput++ = static_cast<typename OutIt::container_type::value_type>(*_First++);
    }

    return _Ouput;
}

template<typename T>
T UTF::Next(T _Begin, T _End)
{
    char32_t codepoint = 0;
    return Decode(_Begin, _End, codepoint);
}

template<typename T>
std::size_t UTF::Count(T _Begin, T _End)
{
    std::size_t length = 0;

    while (_Begin != _End)
    {
        _Begin = Next(_Begin, _End);
        ++length;
    }

    return length;
}

template<typename In, typename Out>
Out UTF::FromANSI(In _Begin, In _End, Out _Output, std::locale& _Locale)
{
    while (_Begin != _End)
    {
        const char32_t codepoint = UTF32::DecodeANSI(*_Bengin++, _Locale);
        _Output = Encode(codepoint, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF::FromWIDE(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        char32_t codepoint = UTF32::DecodeWIDE(*_Begin++);
        _Output = Encode(codepoint, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF::FromLATIN1(In _Begin, In _End, Out _Output)
{
    return Copy(_Begin, _End, _Output);
}

template<typename In, typename Out>
Out UTF::ToANSI(In _Begin, In _End, Out _Output, char _Replacement, std::locale& _Locale)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        _Output = UTF32::EncodeANSI(codepoint, _Output, _Replacement, _Locale);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF::ToWIDE(In _Begin, In _End, Out _Output, wchar_t _Replacement)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        _Output = UTF32::EncodeWIDE(codepoint, _Output, _Replacement);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF::ToLATIN1(In _Begin, In _End, Out _Output, char _Replacement)
{
    while (_Begin != _End)
    {
        *_Output++ = *_Begin < 256 ? static_cast<char>(*_Begin) : _Replacement;
        ++_Begin;
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF::ToUTF8(In _Begin, In _End, Out _Output)
{
    return Copy(_Begin, _End, _Output);
}

template<typename In, typename Out>
Out UTF::ToUTF32(In _Begin, In _End, Out _Output)
{
    return Copy(_Begin, _End, _Output);
}

template<typename In, typename Out>
Out UTF::ToUTF32(In _Begin, In _End, Out _Output)
{
    return Copy(_Begin, _End, _Output);
}

////////////////////////////////////////////////////// UTF8


template<typename T>
T UTF8::Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement)
{
    static constexpr std::array<std::uint8_t, 256> trailing =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
    };

    static constexpr std::array<std::uint32_t, 6> offsets = { 0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080 };

    const auto trailingBytes = trailing[static_cast<std::uint8_t>(*_Begin)];

    if (trailingBytes < std::distance(_Begin, _End))
    {
        _Ouput = 0;

        switch (trailing)
        {
        case 5: 
            _Output += static_cast<std::uint8_t>(*_Begin++);
            _Output <<= 6;
            [[fallthrough]];
            
        case 4:
            _Output += static_cast<std::uint8_t>(*_Begin++);
            _Output <<= 6;
            [[fallthrough]];

        case 3:
            _Output += static_cast<std::uint8_t>(*_Begin++);
            _Output <<= 6;
            [[fallthrough]];

        case 2:
            _Output += static_cast<std::uint8_t>(*_Begin++);
            _Output <<= 6;
            [[fallthrough]];

        case 1:
            _Output += static_cast<std::uint8_t>(*_Begin++);
            _Output <<= 6;
            [[fallthrough]];

        case 0:
            _Output += static_cast<std::uint8_t>(*_Begin++);
            _Output <<= 6;
        }

        _Output -= offsets[trailingBytes];
    }
    else
    {
        _Begin = _End;
        _Output = _Replacement;
    }

	return _Begin;
}

template<typename T>
T UTF8::Encode(char32_t _Input, T _Output, std::uint8_t _Replacement)
{
    static constexpr std::array<std::uint8_t, 7> firstBytes = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if ((_Input > 0x0010FFFF) || ((_Input >= 0xD800) && (_Input <= 0xDBFF)))
    {
        if (_Replacement)
        {
            *_Output++ = static_cast<typename T::container_type::value_type>(_Replacement);
        }
    }
    else
    {
        std::size_t bytesToWrite = 1;
        if (_Input < 0x80)
        {
            bytesToWrite = 1;
        }
        else if (_Input < 0x800)
        {
            bytesToWrite = 2;
        }
        else if (_Input < 0x10000)
        {
            bytesToWrite = 3;
        }
        else if (_Input <= 0x0010FFFF)
        {
            bytesToWrite = 4;
        }

        std::array<std::byte, 4> bytes{};

        switch (bytesToWrite)
        {
        case 4 :
            bytes[3] = static_cast<std::byte>((_Input | 0x80) & 0xBF);
            _Input >>= 6;
            [[fallthrough]];

        case 3:
            bytes[2] = static_cast<std::byte>((_Input | 0x80) & 0xBF);
            _Input >>= 6;
            [[fallthrough]];

        case 2:
            bytes[1] = static_cast<std::byte>((_Input | 0x80) & 0xBF);
            _Input >>= 6;
            [[fallthrough]];

        case 1:
            bytes[0] = static_cast<std::byte>(_Input | firstBytes[bytesToWrite]);
            
        }

        _Output = Copy(bytes.data(), bytes.data() + bytesToWrite, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF8::FromLATIN1(In _Begin, In _End, Out _Output)
{
    while (_Bengin != _End)
    {
        _Output = Encode(*_Bengin++, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF8::ToLATIN1(In _Begin, In _End, Out _Output, char _Replacement)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        *_Output++ = codepoint < 256 ? static_cast<char>(codepoint) : _Replacement;
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF8::ToUTF16(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        _Output = UTF16::Encode(codepoint, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF8::ToUTF32(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        *_Output++ = codepoint;
    }

    return _Output;
}

////////////////////////////////////////////////////// UTF16

template<typename T>
T UTF16::Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement = 0)
{
    const char16_t first = *_Begin++;

    if ((first >= 0xD800) && (first <= 0xDBFF))
    {
        if (_Begin != _End)
        {
            const std::uint16_t szcond = *_Begin++;

            if ((second >= 0xDC00) && (second <= 0xDFFF))
            {
                _Output = ((first - 0xD800u) << 10) + (second - 0xDC00) + 0x0010000;
            }
            else
            {
                ouput = _Replacement;
            }
        }
        else
        {
            _Begin = _End;
            _Output = _Replacement;
        }
    }
    else
    {
        _Output = static_cast<char32_t>(first);
    }

    return _Begin;
}

template<typename T>
T UTF16::Encode(char32_t _Input, T _Output, std::uint16_t _Replacement)
{
    if (_Input <= 0xFFFF)
    {
        if ((_Input >= 0xD800) && (_Input <= 0xDFFF))
        {
            if (_Replacement)
            {
                *_Output++ = _Replacement;
            }
        }
        else
        {
            *_Output++ = static_cast<char16_t>(_Input);
        }
    }
    else if (_Input > 0x0010FFFF)
    {
        if (_Replacement)
        {
            *_Output++ = _Replacement;
        }
    }
    else
    {
        _Input -= 0x0010000;
        *_Output++ = static_cast<char16_t>((_Input >> 10) + 0xD800);
        *_Output++ = static_cast<char16_t>((_Input & 0x3FFUL) + 0xDC00);

    }
    return _Output;
}


template<typename In, typename Out>
Out UTF16::ToUTF8(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        _Output = UTF8::Encode(codepoint, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF16::ToUTF32(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        char32_t codepoint = 0;
        _Begin = Decode(_Begin, _End, codepoint);
        *_Output++ = codepoint;
    }

    return _Output;
}

////////////////////////////////////////////////////// UTF32

template<typename T>
T UTF32::Decode(T _Begin, T _End, char32_t& _Output, char32_t _Replacement)
{
    _Output = *_Begin++;
    return _Output;
}

template<typename T>
T UTF32::Encode(char32_t _Input, T _Output, std::uint32_t _Replacement)
{
    *_Output++ = _Input;
    return _Output;
}

template<typename T>
T UTF32::Next(T _Begin, T _End)
{
    return ++_Begin;
}

template<typename T>
std::size_t UTF32::Count(T _Begin, T _End)
{
    return static_cast<std::size_t>(_End - _Begin);
}

template<typename In, typename Out>
Out UTF32::FromANSI(In _Begin, In _End, Out _Output, std::locale& _Locale)
{
    while (_Begin != _End)
    {
        *_Output++ = DecodeANSI(*_Begin++, _Locale);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF32::FromWIDE(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        *_Output++ = DecodeWIDE(*_Begin++);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF32::ToANSI(In _Begin, In _End, Out _Output, char _Replacement = 0, std::locale& _Locale = {})
{
    while (_Begin != _End)
    {
        _Output = EncodeANSI(*_Begin++, _Output, _Locale);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF32::ToWIDE(In _Begin, In _End, Out _Output, wchar_t _Replacement = 0)
{
    while (_Begin != _End)
    {
        _Output = EncodeWIDE(*_Begin++, _Output, _Replacement);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF32::ToUTF8(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        _Output = UTF8::Encode(*_Begin++, _Output);
    }

    return _Output;
}

template<typename In, typename Out>
Out UTF32::ToUTF16(In _Begin, In _End, Out _Output)
{
    while (_Begin != _End)
    {
        _Output = UTF16::Encode(*_Begin++, _Output);
    }

    return _Output;
}

template<typename T>
char32_t UTF32::DecodeANSI(T _Input, const std::locale& _Locale)
{
    const auto& facet = std::use_facet<std::ctype<wchar_t>>(_Locale);
    
    return static_cast<char32_t>(facet.widen(_Input));
}

template<typename T>
char32_t UTF32::DecodeWIDE(T _Input)
{
    return static_cast<char32_t>(_Input);
}

template<typename T>
T UTF32::EncodeANSI(char32_t _Codepoint, T _Output, char _Remplacement, const std::locale& _Locale)
{
    const auto& facet = std::use_facet<std::ctype<wchar_t>>(_Locale);

    *_Output++ = facet.narrow(static_cast<wchar_t>(_Codepoint), _Remplacement);

    return _Output;
}

template<typename T>
T UTF32::EncodeWIDE(char32_t _Codepoint, T _Output, wchar_t _Replacement)
{
    if (sizeof(wchar_t) == 4)
    {
        *_Output++ = static_cast<wchar_t>(_Codepoint);
    }
    else 
    {
        if ((_Codepoint <= 0xFFFF) && ((_Codepoint < 0xD800) || (_Codepoint > 0xDFFF)))
        {
            *_Output++ = static_cast<wchar_t>(_Codepoint);
        }
        else if (_Replacement)
        {
            *_Output++ = _Replacement;
        }
    }
    return _Output;
}
