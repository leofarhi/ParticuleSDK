#ifndef PE_BASIC_HPP
#define PE_BASIC_HPP

#if defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define CONST_STR_CMP(str, literal) (__builtin_constant_p(str) && __builtin_strcmp(str, literal) == 0)
#else
    #include <string_view>
    #define CONST_STR_CMP(str, literal) (std::string_view(str) == std::string_view(literal))
#endif

#define DECLARE_BUILTIN_ASSET(id, type, ...) \
    static const type __builtin_asset_##id = type(__VA_ARGS__)

namespace Particule::Core
{
    constexpr double MY_PI = 3.14159265358979323846;
    constexpr double MY_PI_2 = 1.57079632679489661923;

    static inline int true_mod(int a, int b) {return (a % b + b) % b;}
    
}


#endif // PE_BASIC_HPP