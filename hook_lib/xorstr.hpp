#pragma once
#include <string>
#include <array>
#include <cstdarg>

#define BEGIN_NAMESPACE( x ) namespace x {
#define END_NAMESPACE }

BEGIN_NAMESPACE(XorCompileTime)

constexpr auto time = __TIME__;
constexpr auto seed = static_cast<int>(time[7]) + static_cast<int>(time[6]) * 10 + static_cast<int>(time[4]) * 60 + static_cast<int>(time[3]) * 600 + static_cast<int>(time[1]) * 3600 + static_cast<int>(time[0]) * 36000;

// 1988, Stephen Park and Keith Miller
// "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
// Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
// with 32-bit math and without division

template < int N >
struct RandomGenerator
{
private:
    static constexpr unsigned a = 16807; // 7^5
    static constexpr unsigned m = 2147483647; // 2^31 - 1

    static constexpr unsigned s = RandomGenerator< N - 1 >::value;
    static constexpr unsigned lo = a * (s & 0xFFFF); // Multiply lower 16 bits by 16807
    static constexpr unsigned hi = a * (s >> 16); // Multiply higher 16 bits by 16807
    static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16); // Combine lower 15 bits of hi with lo's upper bits
    static constexpr unsigned hi2 = hi >> 15; // Discard lower 15 bits of hi
    static constexpr unsigned lo3 = lo2 + hi;

public:
    static constexpr unsigned max = m;
    static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template <>
struct RandomGenerator< 0 >
{
    static constexpr unsigned value = seed;
};

template < int N, int M >
struct RandomInt
{
    static constexpr auto value = RandomGenerator< N + 1 >::value % M;
};

template < int N >
struct RandomChar
{
    static const char value = static_cast<char>(1 + RandomInt< N, 0x7F - 1 >::value);
};

template < size_t N, int K, typename Char >
struct XorString
{
private:
    const char _key;
    std::array< Char, N + 1 > _encrypted;

    constexpr Char enc(Char c) const
    {
        return c ^ _key;
    }

    Char dec(Char c) const
    {
        return c ^ _key;
    }

public:
    template < size_t... Is >
    constexpr __forceinline XorString(const Char* str, std::index_sequence< Is... >) : _key(RandomChar< K >::value), _encrypted{ enc(str[Is])... }
    {
    }

    __forceinline decltype(auto) decrypt(void)
    {
        for (size_t i = 0; i < N; ++i) {
            _encrypted[i] = dec(_encrypted[i]);
        }
        _encrypted[N] = '\0';
        return _encrypted.data();
    }
};

//--------------------------------------------------------------------------------
//-- Note: XorStr will __NOT__ work directly with functions like printf.
//         To work with them you need a wrapper function that takes a const char*
//         as parameter and passes it to printf and alike.
//
//         The Microsoft Compiler/Linker is not working correctly with variadic 
//         templates!
//  
//         Use the functions below or use std::cout (and similar)!
//--------------------------------------------------------------------------------

static auto w_printf = [](const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf_s(fmt, args);
    va_end(args);
};

static auto w_printf_s = [](const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf_s(fmt, args);
    va_end(args);
};

static auto w_sprintf = [](char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
};

static auto w_sprintf_ret = [](char* buf, const char* fmt, ...) {
    int ret;
    va_list args;
    va_start(args, fmt);
    ret = vsprintf(buf, fmt, args);
    va_end(args);
    return ret;
};

static auto w_sprintf_s = [](char* buf, size_t buf_size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf_s(buf, buf_size, fmt, args);
    va_end(args);
};

static auto w_sprintf_s_ret = [](char* buf, size_t buf_size, const char* fmt, ...) {
    int ret;
    va_list args;
    va_start(args, fmt);
    ret = vsprintf_s(buf, buf_size, fmt, args);
    va_end(args);
    return ret;
};

//Old functions before I found out about wrapper functions.
//#define XorStr( s ) ( XorCompileTime::XorString< sizeof(s)/sizeof(char) - 1, __COUNTER__, char >( s, std::make_index_sequence< sizeof(s)/sizeof(char) - 1>() ).decrypt() )
//#define XorStrW( s ) ( XorCompileTime::XorString< sizeof(s)/sizeof(wchar_t) - 1, __COUNTER__, wchar_t >( s, std::make_index_sequence< sizeof(s)/sizeof(wchar_t) - 1>() ).decrypt() )

//Wrapper functions to work in all functions below
#define XorStr( s ) []{ constexpr XorCompileTime::XorString< sizeof(s)/sizeof(char) - 1, __COUNTER__, char > expr( s, std::make_index_sequence< sizeof(s)/sizeof(char) - 1>() ); return expr; }().decrypt()
#define XorStrW( s ) []{ constexpr XorCompileTime::XorString< sizeof(s)/sizeof(wchar_t) - 1, __COUNTER__, wchar_t > expr( s, std::make_index_sequence< sizeof(s)/sizeof(wchar_t) - 1>() ); return expr; }().decrypt()

END_NAMESPACE


/*#ifndef JM_XORSTR_HPP
#define JM_XORSTR_HPP

#if defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM) || defined(__arm__)
#include <arm_neon.h>
#elif defined(_M_X64) || defined(__amd64__) || defined(_M_IX86) || defined(__i386__)
#include <immintrin.h>
#else
#error Unsupported platform
#endif

#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>

#define xorstr(str) ::jm::xor_string([]() { return str; }, std::integral_constant<std::size_t, sizeof(str) / sizeof(*str)>{}, std::make_index_sequence<::jm::detail::_buffer_size<sizeof(str)>()>{})
#define Xors(str) xorstr(str).crypt_get()

#ifdef _MSC_VER
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline)) inline
#endif

namespace jm {

    namespace detail {

        template<std::size_t Size>
        XORSTR_FORCEINLINE constexpr std::size_t _buffer_size()
        {
            return ((Size / 16) + (Size % 16 != 0)) * 2;
        }

        template<std::uint32_t Seed>
        XORSTR_FORCEINLINE constexpr std::uint32_t key4() noexcept
        {
            std::uint32_t value = Seed;
            for (char c : __TIME__)
                value = static_cast<std::uint32_t>((value ^ c) * 16777619ull);
            return value;
        }

        template<std::size_t S>
        XORSTR_FORCEINLINE constexpr std::uint64_t key8()
        {
            constexpr auto first_part = key4<2166136261 + S>();
            constexpr auto second_part = key4<first_part>();
            return (static_cast<std::uint64_t>(first_part) << 32) | second_part;
        }

        // loads up to 8 characters of string into uint64 and xors it with the key
        template<std::size_t N, class CharT>
        XORSTR_FORCEINLINE constexpr std::uint64_t
            load_xored_str8(std::uint64_t key, std::size_t idx, const CharT* str) noexcept
        {
            using cast_type = typename std::make_unsigned<CharT>::type;
            constexpr auto value_size = sizeof(CharT);
            constexpr auto idx_offset = 8 / value_size;

            std::uint64_t value = key;
            for (std::size_t i = 0; i < idx_offset && i + idx * idx_offset < N; ++i)
                value ^=
                (std::uint64_t{ static_cast<cast_type>(str[i + idx * idx_offset]) }
            << ((i % idx_offset) * 8 * value_size));

            return value;
        }

        // forces compiler to use registers instead of stuffing constants in rdata
        XORSTR_FORCEINLINE std::uint64_t load_from_reg(std::uint64_t value) noexcept
        {
#if defined(__clang__) || defined(__GNUC__)
            asm("" : "=r"(value) : "0"(value) : );
            return value;
#else
            volatile std::uint64_t reg = value;
            return reg;
#endif
        }

    } // namespace detail

    template<class CharT, std::size_t Size, class Keys, class Indices>
    class xor_string;

    template<class CharT, std::size_t Size, std::uint64_t... Keys, std::size_t... Indices>
    class xor_string<CharT, Size, std::integer_sequence<std::uint64_t, Keys...>, std::index_sequence<Indices...>> {
#ifndef JM_XORSTR_DISABLE_AVX_INTRINSICS
        constexpr static inline std::uint64_t alignment = ((Size > 16) ? 32 : 16);
#else
        constexpr static inline std::uint64_t alignment = 16;
#endif

        alignas(alignment) std::uint64_t _storage[sizeof...(Keys)];

    public:
        using value_type = CharT;
        using size_type = std::size_t;
        using pointer = CharT*;
        using const_pointer = const CharT*;

        template<class L>
        XORSTR_FORCEINLINE xor_string(L l, std::integral_constant<std::size_t, Size>, std::index_sequence<Indices...>) noexcept
            : _storage{ ::jm::detail::load_from_reg((std::integral_constant<std::uint64_t, detail::load_xored_str8<Size>(Keys, Indices, l())>::value))... }
        {}

        XORSTR_FORCEINLINE constexpr size_type size() const noexcept
        {
            return Size - 1;
        }

        XORSTR_FORCEINLINE void crypt() noexcept
        {
            // everything is inlined by hand because a certain compiler with a certain linker is _very_ slow
#if defined(__clang__)
            alignas(alignment)
                std::uint64_t arr[]{ ::jm::detail::load_from_reg(Keys)... };
            std::uint64_t* keys =
                (std::uint64_t*)::jm::detail::load_from_reg((std::uint64_t)arr);
#else
            alignas(alignment) std::uint64_t keys[]{ ::jm::detail::load_from_reg(Keys)... };
#endif

#if defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM) || defined(__arm__)
#if defined(__clang__)
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : __builtin_neon_vst1q_v(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(__builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2, 51),
                    __builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(keys) + Indices * 2, 51)),
                51)), ...);
#else // GCC, MSVC
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : vst1q_u64(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(vld1q_u64(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2),
                    vld1q_u64(reinterpret_cast<const uint64_t*>(keys) + Indices * 2)))), ...);
#endif
#elif !defined(JM_XORSTR_DISABLE_AVX_INTRINSICS)
            ((Indices >= sizeof(_storage) / 32 ? static_cast<void>(0) : _mm256_store_si256(
                reinterpret_cast<__m256i*>(_storage) + Indices,
                _mm256_xor_si256(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(_storage) + Indices),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(keys) + Indices)))), ...);

            if constexpr (sizeof(_storage) % 32 != 0)
                _mm_store_si128(
                    reinterpret_cast<__m128i*>(_storage + sizeof...(Keys) - 2),
                    _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage + sizeof...(Keys) - 2)),
                        _mm_load_si128(reinterpret_cast<const __m128i*>(keys + sizeof...(Keys) - 2))));
#else
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : _mm_store_si128(
                reinterpret_cast<__m128i*>(_storage) + Indices,
                _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage) + Indices),
                    _mm_load_si128(reinterpret_cast<const __m128i*>(keys) + Indices)))), ...);
#endif
        }

        XORSTR_FORCEINLINE const_pointer get() const noexcept
        {
            return reinterpret_cast<const_pointer>(_storage);
        }

        XORSTR_FORCEINLINE pointer get() noexcept
        {
            return reinterpret_cast<pointer>(_storage);
        }

        XORSTR_FORCEINLINE pointer crypt_get() noexcept
        {
            // crypt() is inlined by hand because a certain compiler with a certain linker is _very_ slow
#if defined(__clang__)
            alignas(alignment)
                std::uint64_t arr[]{ ::jm::detail::load_from_reg(Keys)... };
            std::uint64_t* keys =
                (std::uint64_t*)::jm::detail::load_from_reg((std::uint64_t)arr);
#else
            alignas(alignment) std::uint64_t keys[]{ ::jm::detail::load_from_reg(Keys)... };
#endif

#if defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM) || defined(__arm__)
#if defined(__clang__)
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : __builtin_neon_vst1q_v(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(__builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2, 51),
                    __builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(keys) + Indices * 2, 51)),
                51)), ...);
#else // GCC, MSVC
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : vst1q_u64(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(vld1q_u64(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2),
                    vld1q_u64(reinterpret_cast<const uint64_t*>(keys) + Indices * 2)))), ...);
#endif
#elif !defined(JM_XORSTR_DISABLE_AVX_INTRINSICS)
            ((Indices >= sizeof(_storage) / 32 ? static_cast<void>(0) : _mm256_store_si256(
                reinterpret_cast<__m256i*>(_storage) + Indices,
                _mm256_xor_si256(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(_storage) + Indices),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(keys) + Indices)))), ...);

            if constexpr (sizeof(_storage) % 32 != 0)
                _mm_store_si128(
                    reinterpret_cast<__m128i*>(_storage + sizeof...(Keys) - 2),
                    _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage + sizeof...(Keys) - 2)),
                        _mm_load_si128(reinterpret_cast<const __m128i*>(keys + sizeof...(Keys) - 2))));
#else
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : _mm_store_si128(
                reinterpret_cast<__m128i*>(_storage) + Indices,
                _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage) + Indices),
                    _mm_load_si128(reinterpret_cast<const __m128i*>(keys) + Indices)))), ...);
#endif

            return (pointer)(_storage);
        }
    };

    template<class L, std::size_t Size, std::size_t... Indices>
    xor_string(L l, std::integral_constant<std::size_t, Size>, std::index_sequence<Indices...>)->xor_string<
        std::remove_const_t<std::remove_reference_t<decltype(l()[0])>>,
        Size,
        std::integer_sequence<std::uint64_t, detail::key8<Indices>()...>,
        std::index_sequence<Indices...>>;

} // namespace jm

#endif // include guard
*/