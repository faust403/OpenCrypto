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
            std::uint64_t __NS   = 10;

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

            constexpr std::string_view hex(void) noexcept;
            constexpr std::string_view bin(void) noexcept;
            constexpr std::string_view dec(void) noexcept;

            template <typename Type>
            constexpr Byte & transform(const Type NS) noexcept
            {
                  static_assert(std::is_integral_v<Type>,
                                "Method Byte::transform(const Type) have got non-integral Type as argument. Integral Required");
                  return *this;
            }

            constexpr unsigned char ascii(void) noexcept;

            constexpr unsigned char get(void) noexcept;
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
            constexpr bool operator== (Byte & Other) noexcept { return this->__Byte == *Other; }
            constexpr bool operator!= (Byte & Other) noexcept { return this->__Byte != *Other; }
            Byte           operator|| (Byte & Other) noexcept { return Byte{static_cast<unsigned char>(this->__Byte || *Other)}; }
            Byte           operator&& (Byte & Other) noexcept { return Byte{static_cast<unsigned char>(this->__Byte && *Other)}; }
            Byte           operator^ (Byte & Other) noexcept { return Byte{static_cast<unsigned char>(this->__Byte ^ *Other)}; }
            constexpr Byte & operator= (Byte & Other) noexcept
            {
                  this->__Byte = *Other;
                  return *this;
            }
            constexpr Byte & operator&= (Byte & Other) noexcept
            {
                  this->__Byte &= *Other;
                  return *this;
            }
            constexpr Byte & operator|= (Byte & Other) noexcept
            {
                  this->__Byte |= *Other;
                  return *this;
            }
            constexpr Byte & operator^= (Byte & Other) noexcept
            {
                  this->__Byte ^= *Other;
                  return *this;
            }
            template <typename Type>
            constexpr Byte operator>> (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator >> have got non-integral operand. Integral required");
                  return Byte{this->__Byte >> Shift};
            }
            template <typename Type>
            constexpr Byte & operator>>= (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator >>= have got non-integral operand. Integral required");
                  this->__Byte >>= Shift;
                  return *this;
            }
            template <typename Type>
            constexpr Byte operator<< (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator << have got non-integral operand. Integral required");
                  return Byte{this->__Byte << Shift};
            }
            template <typename Type>
            constexpr Byte & operator<<= (const Type Shift) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "operator <<= have got non-integral operand. Integral required");
                  this->__Byte <<= Shift;
                  return *this;
            }
            // Unary operators
            constexpr unsigned char & operator* (void) noexcept { return this->__Byte; }
            Byte                      operator!(void) noexcept { return Byte{static_cast<unsigned char>(!this->__Byte)}; }
};

#endif
