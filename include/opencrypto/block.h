#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <opencrypto/constants.h>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

class Byte
{
      protected:
            unsigned char    __Byte   = 0;
            std::uint64_t    __NS     = 10;
            std::string_view __Format = "";

      public:
            constexpr explicit Byte(void) noexcept = default;
            template <typename Type>
            constexpr explicit Byte(const Type Data) noexcept : __Byte(Data)
            {
                  static_assert(std::is_integral_v<Type>,
                                "Constructor Byte(const Type) have got non-integral Type as argument. Integral required");
                  static_assert(std::is_same_v<unsigned char, Type>,
                                "Constructor Byte(const Type) have got non-uchar Type as argument. Unsigned char required");
            }
            virtual ~Byte(void) noexcept = default;

            constexpr Byte & hex(void) noexcept { return this->transform(16); }
            constexpr Byte & bin(void) noexcept { return this->transform(2); }
            constexpr Byte & dec(void) { return this->transform(10); }
            std::string_view ascii(void) noexcept
            {
                  return std::string{const_cast<const char *>(reinterpret_cast<char *>(&this->__Byte)), 1} +
                         std::string{this->__Format.data(), this->__Format.size()};
            }

            template <typename Type>
            constexpr Byte & transform(const Type NS) noexcept
            {
                  static_assert(std::is_integral_v<Type>,
                                "Method Byte::transform(const Type) have got non-integral Type as argument. Integral required");
                  this->__NS = NS;
                  return *this;
            }
            constexpr Byte & format(const std::string_view NewFormat) noexcept
            {
                  this->__Format = NewFormat;
                  return *this;
            }

            std::string get(void) noexcept
            {
                  const int Copy = static_cast<int>(this->__Byte);
                  switch(this->__NS)
                  {
                        case 10:
                              return (std::ostringstream{} << Copy << std::string{this->__Format}).str();
                        case 8:
                              return (std::ostringstream{} << std::uppercase << std::oct << Copy << std::string{this->__Format})
                                  .str();
                        case 16:
                              return (std::ostringstream{} << std::uppercase << std::hex << Copy << std::string{this->__Format})
                                  .str();
                        case 2:
                              return std::bitset<8>{Copy}.to_string() + std::string{this->__Format};
                  }
                  // Other sutiations
            }
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
