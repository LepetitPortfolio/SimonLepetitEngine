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

    /**
    * Assigne la valeur d'un caractère à un autre.
    * @param _Char1 Référence vers le caractère de destination.
    * @param _Char2 Caractère source à assigner.
    */
    static void Assign(CharType& _Char1, CharType _Char2);

    /**
    * Assigne un caractère à une position spécifique dans une chaîne.
    * @param _Str Pointeur vers la chaîne de destination.
    * @param _Index Position dans la chaîne où assigner le caractère.
    * @param _Char Caractère à assigner.
    * @return Pointeur vers la chaîne modifiée.
    *
    * @details
    * Utilise `std::char_traits<char>::assign` pour effectuer l'assignation,
    * avec des conversions de type pour compatibilité.
    */
    static CharType* Assign(CharType* _Str, std::size_t _Index, CharType _Char);

    /**
    * Compare deux caractères pour l'égalité.
    * @param _Char1 Premier caractère.
    * @param _Char2 Deuxième caractère.
    * @return true si les caractères sont égaux, false sinon.
    */
    static bool Equal(CharType _Char1, CharType _Char2);

    /**
    * Compare deux caractères pour vérifier si le premier est strictement inférieur au deuxième.
    * @param _Char1 Premier caractère.
    * @param _Char2 Deuxième caractère.
    * @return true si _Char1 < _Char2, false sinon.
    */
    static bool Lt(CharType _Char1, CharType _Char2);

    /**
    * Déplace un bloc de caractères d'une chaîne source vers une chaîne destination.
    * @param _Str1 Pointeur vers la chaîne de destination.
    * @param _Str2 Pointeur vers la chaîne source.
    * @param _Index Nombre de caractères à déplacer.
    * @return Pointeur vers la chaîne de destination.
    *
    * @details
    * Utilise `std::memmove` pour garantir un déplacement sûr (même si les blocs se chevauchent).
    */
    static CharType* Move(CharType* _Str1, const CharType* _Str2, std::size_t _Index);

    /**
    * Copie un bloc de caractères d'une chaîne source vers une chaîne destination.
    * @param _Str1 Pointeur vers la chaîne de destination.
    * @param _Str2 Pointeur vers la chaîne source.
    * @param _Index Nombre de caractères à copier.
    * @return Pointeur vers la chaîne de destination.
    *
    * @details
    * Utilise `std::memcpy` pour une copie rapide.
    */
    static CharType* Copy(CharType* _Str1, const CharType* _Str2, std::size_t _Index);

    /**
    * Compare deux blocs de caractères.
    * @param _Str1 Pointeur vers la première chaîne.
    * @param _Str2 Pointeur vers la deuxième chaîne.
    * @param _Index Nombre de caractères à comparer.
    * @return Résultat de la comparaison (0 si égaux, <0 si _Str1 < _Str2, >0 sinon).
    */
    static int Compare(const CharType* _Str1, const CharType* _Str2, std::size_t _Index);

    /**
    * Calcule la longueur d'une chaîne de caractères (jusqu'au caractère nul).
    * @param _Str Pointeur vers la chaîne.
    * @return Longueur de la chaîne (sans le caractère nul final).
    */
    static std::size_t Length(const CharType* _Str);

    /**
    * Trouve la première occurrence d'un caractère dans une chaîne.
    * @param _Str Pointeur vers la chaîne à parcourir.
    * @param _Index Nombre de caractères à parcourir.
    * @param _Char Caractère à rechercher.
    * @return Pointeur vers la première occurrence du caractère, ou nullptr si non trouvé.
    */
    static const CharType* Find(const CharType* _Str, std::size_t _Index, const CharType& _Char);

    /**
    * Convertit un entier de type `IntType` en `CharType`.
    * @param _Int Valeur entière à convertir.
    * @return Caractère correspondant.
    */
    static CharType ToCharType(IntType _Int) noexcept;

    /**
    * Convertit un caractère de type `CharType` en `IntType`.
    * @param _Char Caractère à convertir.
    * @return Valeur entière correspondante.
    */
    static IntType ToIntType(CharType _Char) noexcept;

    /**
    * Compare deux valeurs de type `IntType` pour l'égalité.
    * @param _Int1 Première valeur.
    * @param _Int2 Deuxième valeur.
    * @return true si les valeurs sont égales, false sinon.
    */
    static bool EqualIntType(IntType _Int1, IntType _Int2) noexcept;

    /**
    * Retourne la valeur spéciale EOF (fin de fichier).
    * @return Valeur EOF de type `IntType`.
    */
    static IntType Eof() noexcept;

    /**
    * Vérifie si une valeur de type `IntType` n'est pas EOF.
    * @param _Int Valeur à vérifier.
    * @return true si la valeur n'est pas EOF, false sinon.
    */
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
    
    /**
    * Constructeur à partir d'un caractère ANSI.
    * @param _ANSIChar Caractère ANSI à convertir en UTF-32.
    * @param _Locale Locale utilisée pour la conversion.
    */
    String(char _ANSIChar, const std::locale& _Locale = {});

    /**
    * Constructeur à partir d'un caractère large (wchar_t).
    * @param _WideChar Caractère large à convertir en UTF-32.
    */
    String(wchar_t _WideChar);

    /**
    * Constructeur à partir d'un caractère UTF-32.
    * @param _UTF32Char Caractère UTF-32.
    */
    String(char32_t _UTF32Char);

    /**
    * Constructeur à partir d'une chaîne ANSI (C-string).
    * @param _ANSIString Chaîne ANSI à convertir en UTF-32.
    * @param _Locale Locale utilisée pour la conversion.
    */
    String(const char* _ANSIString, const std::locale& _Locale = {});

    /**
    * Constructeur à partir d'une chaîne ANSI (std::string).
    * @param _ANSIString Chaîne ANSI à convertir en UTF-32.
    * @param _Locale Locale utilisée pour la conversion.
    */
    String(const std::string& _ANSIString, const std::locale& _Locale = {});

    /**
    * Constructeur à partir d'une chaîne large (C-string).
    * @param _WideString Chaîne large à convertir en UTF-32.
    */
    String(const wchar_t* _WideString);
    
    /**
    * Constructeur à partir d'une chaîne large (std::wstring).
    * @param _WideString Chaîne large à convertir en UTF-32.
    */
    String(const std::wstring& _WideString);
    
    /**
    * Constructeur à partir d'une chaîne UTF-32 (C-string).
    * @param _UTF32String Chaîne UTF-32.
    */
    String(const char32_t* _UTF32String);

    /**
    * Constructeur à partir d'une chaîne UTF-32 (std::u32string).
    * @param _UTF32String Chaîne UTF-32 à déplacer.
    */
    String(std::u32string _UTF32String);

    /**
    * Convertit une chaîne UTF-8 en une chaîne UTF-32 (représentée par la classe String).
    *
    * @tparam T Type de l'itérateur (ex: `const char*`, `std::string::iterator`, etc.).
    * @param _Begin Itérateur pointant vers le début de la chaîne UTF-8.
    * @param _End Itérateur pointant vers la fin de la chaîne UTF-8.
    * @return String Chaîne convertie en UTF-32.
    *
    * @details
    * - Crée une nouvelle instance de `String` (UTF-32).
    * - Utilise la fonction statique `UTF8::ToUTF32` pour convertir la plage [_Begin, _End) en UTF-32.
    * - Les caractères convertis sont insérés dans `outStr.m_String` via `std::back_inserter`.
    * - Retourne la chaîne résultante.
    *
    * @note
    * `UTF8::ToUTF32` est probablement une fonction utilitaire qui gère la conversion des séquences UTF-8
    * (1 à 4 octets par caractère) vers UTF-32 (4 octets par caractère).
    */
    template <typename T>
    static String FromUTF8(T _Begin, T _End);

    /**
    * Convertit une chaîne UTF-16 en une chaîne UTF-32 (représentée par la classe String).
    *
    * @tparam T Type de l'itérateur (ex: `const char16_t*`, `std::u16string::iterator`, etc.).
    * @param _Begin Itérateur pointant vers le début de la chaîne UTF-16.
    * @param _End Itérateur pointant vers la fin de la chaîne UTF-16.
    * @return String Chaîne convertie en UTF-32.
    *
    * @details
    * - Crée une nouvelle instance de `String` (UTF-32).
    * - Utilise la fonction statique `UTF16::ToUTF32` pour convertir la plage [_Begin, _End) en UTF-32.
    * - Les caractères convertis sont insérés dans `outStr.m_String` via `std::back_inserter`.
    * - Retourne la chaîne résultante.
    *
    * @note
    * `UTF16::ToUTF32` gère la conversion des paires de substitution UTF-16 (pour les caractères hors du plan multilingue de base)
    * vers UTF-32.
    */
    template <typename T>
    static String FromUTF16(T _Begin, T _End);

    /**
    * Crée une chaîne UTF-32 (représentée par la classe String) à partir d'une plage d'itérateurs UTF-32.
    *
    * @tparam T Type de l'itérateur (ex: `const char32_t*`, `std::u32string::iterator`, etc.).
    * @param _Begin Itérateur pointant vers le début de la chaîne UTF-32.
    * @param _End Itérateur pointant vers la fin de la chaîne UTF-32.
    * @return String Chaîne UTF-32 copiée.
    *
    * @details
    * - Crée une nouvelle instance de `String`.
    * - Copie directement la plage [_Begin, _End) dans `outStr.m_String` via `assign`.
    * - Retourne la chaîne résultante.
    *
    * @note
    * Aucune conversion n'est nécessaire ici, car UTF-32 est déjà le format interne de `String`.
    * Cette fonction est une simple copie.
    */
    template <typename T>
    static String FromUTF32(T _Begin, T _End);

    
    /**
    * Conversion implicite vers std::string (ANSI).
    * @return Chaîne ANSI équivalente.
    */
    operator std::string() const;

    /**
    * Conversion implicite vers std::wstring (large).
    * @return Chaîne large équivalente.
    */
    operator std::wstring() const;

    /**
    * Convertit la chaîne UTF-32 en ANSI (std::string).
    * @param _Locale Locale utilisée pour la conversion.
    * @return Chaîne ANSI équivalente.
    */
    std::string ToANSIString(const std::locale& _Locale = {}) const;
    
    /**
    * Convertit la chaîne UTF-32 en large (std::wstring).
    * @return Chaîne large équivalente.
    */
    std::wstring ToWideString() const;

    /**
    * Convertit la chaîne UTF-32 en UTF-8.
    * @return Chaîne UTF-8 équivalente.
    */
    U8String ToUTF8() const;

    /**
    * Convertit la chaîne UTF-32 en UTF-16.
    * @return Chaîne UTF-16 équivalente.
    */
    std::u16string ToUTF16() const;
    
    /**
    * Retourne la chaîne UTF-32 sous-jacente.
    * @return Référence vers m_String.
    */
    std::u32string ToUTF32() const;

    
    /**
    * Ajoute une chaîne à la chaîne courante.
    * @param _Right Chaîne à ajouter.
    * @return Référence vers la chaîne courante.
    */
    String& operator+= (const String& _Right);

    /**
    * Concatène deux chaînes.
    * @param _Right Chaîne à concaténer.
    * @return Nouvelle chaîne résultat de la concaténation.
    */
    String operator+ (const String& _Right);

    
    /**
    * Accès en lecture seule à un caractère de la chaîne.
    * @param _Index Index du caractère.
    * @return Caractère à la position _Index.
    * @throws assert Si l'index est hors limites.
    */
    char32_t operator[](std::size_t _Index) const;

    /**
    * Accès en lecture/écriture à un caractère de la chaîne.
    * @param _Index Index du caractère.
    * @return Référence vers le caractère à la position _Index.
    * @throws assert Si l'index est hors limites.
    */
    char32_t& operator[](std::size_t _Index);

    /**
    * Efface tout le contenu de la chaîne.
    */
    void Clear();

    /**
    * Retourne la taille de la chaîne.
    * @return Nombre de caractères dans la chaîne.
    */
    std::size_t GetSize() const;

    /**
    * Vérifie si la chaîne est vide.
    * @return true si la chaîne est vide, false sinon.
    */
    bool IsEmpty() const;

    /**
    * Supprime une partie de la chaîne.
    * @param _Position Position de départ de la suppression.
    * @param _Count Nombre de caractères à supprimer.
    */
    void Erase(std::size_t _Position, std::size_t _Count = 1);

    /**
    * Insère une chaîne à une position donnée.
    * @param _Position Position d'insertion.
    * @param _Str Chaîne à insérer.
    */
    void Insert(std::size_t _Position, const String& _Str);

    /**
    * Trouve la première occurrence d'une sous-chaîne.
    * @param _Str Sous-chaîne à rechercher.
    * @param _Start Position de départ de la recherche.
    * @return Position de la première occurrence, ou `InvalidPos` si non trouvée.
    */
    std::size_t Find(const String& _Str, std::size_t _Start = 0) const;

    /**
    * Remplace une partie de la chaîne par une autre chaîne.
    * @param _Position Position de départ du remplacement.
    * @param _Length Nombre de caractères à remplacer.
    * @param _ReplaceWith Chaîne de remplacement.
    */
    void Replace(std::size_t _Position, std::size_t _Length, const String& _ReplaceWith);
    
    /**
    * Remplace toutes les occurrences d'une sous-chaîne par une autre.
    * @param _SearchFor Sous-chaîne à rechercher.
    * @param _ReplaceWith Chaîne de remplacement.
    */
    void Replace(const String& _SearchFor, const String& _ReplaceWith);

    /**
    * Extrait une sous-chaîne.
    * @param _Position Position de départ.
    * @param _Length Longueur de la sous-chaîne.
    * @return Nouvelle chaîne contenant la sous-chaîne extraite.
    */
    String SubString(std::size_t _Position, std::size_t _Legth = InvalidPos) const;

    /**
    * Divise une chaîne en sous-chaînes en utilisant un délimiteur.
    * @param _Str Chaîne à diviser.
    * @param _Delimiter Délimiteur.
    * @return Vecteur de sous-chaînes.
    */
    static std::vector<String> Split(const String& _Str, const String& _Delimiter);

    /**
    * Divise la chaîne courante en sous-chaînes en utilisant un délimiteur.
    * @param _Delimiter Délimiteur.
    * @return Vecteur de sous-chaînes.
    */
    std::vector<String> Split(const String& _Delimiter);
    
    /**
    * Retourne un pointeur vers les données de la chaîne.
    * @return Pointeur vers le premier caractère de la chaîne.
    */
    const char32_t* GetData() const;

    /**
    * Retourne un itérateur vers le début de la chaîne.
    * @return Itérateur vers le premier caractère.
    */
    Iterator Begin();

    /**
    * Retourne un itérateur constant vers le début de la chaîne.
    * @return Itérateur constant vers le premier caractère.
    */
    ConstIterator Begin() const;

    /**
    * Retourne un itérateur vers la fin de la chaîne.
    * @return Itérateur vers la position après le dernier caractère.
    */
    Iterator End();

    /**
    * Retourne un itérateur constant vers la fin de la chaîne.
    * @return Itérateur constant vers la position après le dernier caractère.
    */
    ConstIterator End() const;

private:

    std::u32string m_String;

    /**
    * Compare deux chaînes pour l'égalité.
    * @param _Left Première chaîne.
    * @param _Right Deuxième chaîne.
    * @return true si les chaînes sont égales, false sinon.
    */
    friend bool operator==(const String& _Left, const String& _Right);
    
    /**
    * Compare deux chaînes pour vérifier si la première est strictement inférieure à la deuxième.
    * @param _Left Première chaîne.
    * @param _Right Deuxième chaîne.
    * @return true si _Left < _Right, false sinon.
    */
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
