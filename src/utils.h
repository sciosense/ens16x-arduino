#ifndef SCIOSENSE_UTILS_H
#define SCIOSENSE_UTILS_H

#include <stdint.h>

namespace ScioSense::Utils
{
    // This class implements std::endian <bit> (C++20)
    // https://en.cppreference.com/w/cpp/types/endian
    //
    // This is intended to support C compilers below C++20 as the Arduino IDE defaults to C++11
    //
    class endian
    {
    public:
    #ifndef __BYTE_ORDER__
        static constexpr    uint32_t little  = 0;
        static constexpr    uint32_t big     = 1;
        static inline const uint32_t native() { return (((union { unsigned x; unsigned char c; }){1}).c == 1) ? little : big; }
    #else
        static constexpr          uint32_t little  = __ORDER_LITTLE_ENDIAN__;
        static constexpr          uint32_t big     = __ORDER_BIG_ENDIAN__;
        inline static constexpr   uint32_t native() { return __BYTE_ORDER__; }
    #endif

    public:
        template<class T> static inline T littleTo(const uint8_t* data, const uint8_t len = sizeof(T))
        {
            if (native() == little && len >= sizeof(T))
            {
                return *reinterpret_cast<const T*>(data);
            }

            return reverse<T>(data, len);
        }

        template<class T> static inline T bigTo(const uint8_t* data, const uint8_t len = sizeof(T))
        {
            if (native() == big && len >= sizeof(T))
            {
                return *reinterpret_cast<const T*>(data);
            }

            return reverse<T>(data, len);
        }

    private:
        template<class T> static inline T reverse(const uint8_t* data, const uint8_t len)
        {
            T ret = (T)data[sizeof(T) - 1];

            if (sizeof(T) > 1 && len > 1) ret |= ((T)data[0]) << ((sizeof(T) - 1) - 0) * 8;
            if (sizeof(T) > 2 && len > 2) ret |= ((T)data[1]) << ((sizeof(T) - 1) - 1) * 8;
            if (sizeof(T) > 3 && len > 3) ret |= ((T)data[2]) << ((sizeof(T) - 1) - 2) * 8;
            if (sizeof(T) > 4 && len > 4) ret |= ((T)data[3]) << ((sizeof(T) - 1) - 3) * 8;
            if (sizeof(T) > 5 && len > 5) ret |= ((T)data[4]) << ((sizeof(T) - 1) - 4) * 8;
            if (sizeof(T) > 6 && len > 6) ret |= ((T)data[5]) << ((sizeof(T) - 1) - 5) * 8;
            if (sizeof(T) > 7 && len > 7) ret |= ((T)data[6]) << ((sizeof(T) - 1) - 6) * 8;

            return ret;
        }
    };

    template<class ADDRESS, class RESULT> class IoInterface
    {
        public:
            virtual RESULT read(const ADDRESS& address, uint8_t* data, const size_t& size)  = 0;
            virtual RESULT write(const ADDRESS& address, uint8_t* data, const size_t& size) = 0;
    };
}

#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE, UNDERLYINGTYPE)                                                                                            \
                                                                                                                                                        \
    inline ENUMTYPE operator |  (ENUMTYPE a, ENUMTYPE b)    { return ENUMTYPE(((UNDERLYINGTYPE)a)|((UNDERLYINGTYPE)b)); }                               \
    inline ENUMTYPE operator |= (ENUMTYPE& a, ENUMTYPE b)   { return (ENUMTYPE &)(((UNDERLYINGTYPE &)a) |= ((UNDERLYINGTYPE)b)); }                      \
    inline ENUMTYPE operator &  (ENUMTYPE a, ENUMTYPE b)    { return ENUMTYPE(((UNDERLYINGTYPE)a)&((UNDERLYINGTYPE)b)); }                               \
    inline ENUMTYPE operator &= (ENUMTYPE& a, ENUMTYPE b)   { return (ENUMTYPE &)(((UNDERLYINGTYPE &)a) &= ((UNDERLYINGTYPE)b)); }                      \
    inline ENUMTYPE operator ~  (ENUMTYPE a)                { return (ENUMTYPE)(~((UNDERLYINGTYPE)a)); }                                                \
    inline ENUMTYPE operator ^  (ENUMTYPE a, ENUMTYPE b)    { return ENUMTYPE(((UNDERLYINGTYPE)a)^((UNDERLYINGTYPE)b)); }                               \
    inline ENUMTYPE operator ^= (ENUMTYPE& a, ENUMTYPE b)   { return (ENUMTYPE &)(((UNDERLYINGTYPE &)a) ^= ((UNDERLYINGTYPE)b)); }                      \
    inline bool     hasAllFlags (const ENUMTYPE& a, const ENUMTYPE& b)      {return ((UNDERLYINGTYPE &)a & (UNDERLYINGTYPE &)b) == (UNDERLYINGTYPE &)b; }    \
    inline bool     hasAnyFlag  (const ENUMTYPE& a, const ENUMTYPE& b)      {return ((UNDERLYINGTYPE &)a & (UNDERLYINGTYPE &)b) != 0; }                      \
    inline bool     hasFlag     (const ENUMTYPE& e, const ENUMTYPE& flag)   {return hasAllFlags(e, flag); }

#endif //SCIOSENSE_UTILS_H