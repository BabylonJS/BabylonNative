#pragma once

#if defined(__ANDROID__) && defined(_LIBCPP_VERSION)

// libc++ removed the non-standard std::char_traits<unsigned short> specialization in
// C++20. Some transitive Chromium-based dependencies still instantiate
// std::basic_string<uint16_t>, so provide a shim that forwards to the supported
// char16_t implementation when we compile against Android's libc++.
#include <cstdint>
#include <cstddef>
#include <string>
#include <type_traits>

static_assert(sizeof(char16_t) == sizeof(std::uint16_t),
    "uint16_t char traits shim assumes char16_t is 16 bits");

namespace std
{
    template<>
    struct char_traits<std::uint16_t>
    {
        using char_type = std::uint16_t;
        using int_type = char_traits<char16_t>::int_type;
        using off_type = char_traits<char16_t>::off_type;
        using pos_type = char_traits<char16_t>::pos_type;
        using state_type = char_traits<char16_t>::state_type;

        static void assign(char_type& c1, const char_type& c2) noexcept
        {
            c1 = c2;
        }

        static constexpr bool eq(char_type c1, char_type c2) noexcept
        {
            return c1 == c2;
        }

        static constexpr bool lt(char_type c1, char_type c2) noexcept
        {
            return c1 < c2;
        }

        static void assign(char_type* s, std::size_t n, char_type c)
        {
            char_traits<char16_t>::assign(reinterpret_cast<char16_t*>(s), n, static_cast<char16_t>(c));
        }

        static char_type* move(char_type* dst, const char_type* src, std::size_t n)
        {
            return reinterpret_cast<char_type*>(char_traits<char16_t>::move(
                reinterpret_cast<char16_t*>(dst),
                reinterpret_cast<const char16_t*>(src),
                n));
        }

        static char_type* copy(char_type* dst, const char_type* src, std::size_t n)
        {
            return reinterpret_cast<char_type*>(char_traits<char16_t>::copy(
                reinterpret_cast<char16_t*>(dst),
                reinterpret_cast<const char16_t*>(src),
                n));
        }

        static int compare(const char_type* s1, const char_type* s2, std::size_t n)
        {
            return char_traits<char16_t>::compare(
                reinterpret_cast<const char16_t*>(s1),
                reinterpret_cast<const char16_t*>(s2),
                n);
        }

        static std::size_t length(const char_type* s)
        {
            return char_traits<char16_t>::length(reinterpret_cast<const char16_t*>(s));
        }

        static const char_type* find(const char_type* s, std::size_t n, const char_type& a)
        {
            return reinterpret_cast<const char_type*>(char_traits<char16_t>::find(
                reinterpret_cast<const char16_t*>(s),
                n,
                static_cast<char16_t>(a)));
        }

        static constexpr int_type eof() noexcept
        {
            return char_traits<char16_t>::eof();
        }

        static constexpr int_type to_int_type(char_type c) noexcept
        {
            return char_traits<char16_t>::to_int_type(static_cast<char16_t>(c));
        }

        static constexpr char_type to_char_type(int_type c) noexcept
        {
            return static_cast<char_type>(char_traits<char16_t>::to_char_type(c));
        }

        static constexpr int_type not_eof(int_type c) noexcept
        {
            return char_traits<char16_t>::not_eof(c);
        }
    };
}

#endif // defined(__ANDROID__) && defined(_LIBCPP_VERSION)
