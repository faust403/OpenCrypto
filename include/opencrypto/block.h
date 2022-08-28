#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <type_traits>

class Byte
{
      protected:
            unsigned char __Byte = 0;

      public:
            constexpr explicit Byte(void) noexcept = default;
            template <typename Type>
            constexpr explicit Byte(const Type Data) noexcept : __Byte(Data)
            {
                  static_assert(std::is_integral_v<Type>,
                                "Constructor Byte(const Type) have got non-integral Type as argument. Integral Required");
                  static_assert(std::is_same_v<unsigned char, Type>,
                                "Constructor Byte(const Type) have got non-uchar Type as argument. Unsigned char required");
            }
            virtual ~Byte(void) noexcept = default;

            constexpr Byte &           hex(void) noexcept;
            constexpr Byte &           bin(void) noexcept;
            constexpr Byte &           transform(const short) noexcept;
            constexpr Byte &           format(void) noexcept;
            constexpr unsigned char    integral(void) noexcept;
            constexpr std::string_view str(void) noexcept;
            constexpr unsigned char &  get(void) noexcept;
            template <typename Type>
            constexpr void set(const Type Data) noexcept
            {
                  static_assert(std::is_integral_v<Type>,
                                "Method Byte::set(const Type) have got non-integral Type. Integral required");
                  static_assert(std::is_same_v<unsigned char, Type>,
                                "Method Byte::set(const Type) have got non-uchar Type. Unsigned char required");
                  this->__Byte = Data;
            }

            // Binary operators
            constexpr Byte & operator= (const Byte & Other) noexcept;
            constexpr Byte & operator== (const Byte & Other) noexcept;
            constexpr Byte & operator!= (const Byte & Other) noexcept;
            constexpr Byte & operator|| (const Byte & Other) noexcept;
            constexpr Byte & operator|= (const Byte & Other) noexcept;
            constexpr Byte & operator&& (const Byte & Other) noexcept;
            constexpr Byte & operator&= (const Byte & Other) noexcept;
            constexpr Byte & operator^ (const Byte & Other) noexcept;
            constexpr Byte & operator^= (const Byte & Other) noexcept;
            template <typename Type>
            constexpr Byte & operator>> (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator >> have got non-integral operand. Integral required");
                  return *this;
            }
            template <typename Type>
            constexpr Byte & operator>>= (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator >>= have got non-integral operand. Integral required");
                  return *this;
            }
            template <typename Type>
            constexpr Byte & operator<< (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator << have got non-integral operand. Integral required");
                  return *this;
            }
            template <typename Type>
            constexpr Byte & operator<<= (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator <<= have got non-integral operand. Integral required");
                  return *this;
            }
            // Unary operators
            constexpr Byte & operator!(void) noexcept;
            constexpr Byte & operator* (const Byte & Other) noexcept;
};

#endif
