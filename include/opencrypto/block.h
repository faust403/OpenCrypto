#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

template <std::uint64_t Size = 0>
class Block
{
      private:
            unsigned char * Bytes  = NULL;
            std::uint64_t   Length = Size;

      public:
            template <std::uint64_t NewSize>
            constexpr explicit Block(Block<NewSize> & Other) noexcept :
                Length(Other.size()), Bytes(new unsigned char[Other.size()])
            {
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  std::memcpy(this->Bytes, Other.data(), this->Length);
            }
            template <std::uint64_t NewSize>
            constexpr explicit Block(Block<NewSize> && Other) noexcept :
                Length(Other.size()), Bytes(new unsigned char[Other.size()])
            {
                  std::memcpy(this->Bytes, std::move(Other.data()), this->Length);
                  Other.clear();
            }
            template <typename Type>
            constexpr explicit Block(Type * NewBytes) noexcept : Bytes(new unsigned char[Size])
            {
                  static_assert(std::is_same_v<std::remove_const_t<Type>, unsigned char>,
                                "The constructor Block:Block(Type * NewBytes) have got Type != unsigned char");
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  std::memcpy(this->Bytes, NewBytes, Size);
            }
            template <typename... Args>
            constexpr explicit Block(Args... args) noexcept : Length(sizeof...(args))
            {
                  this->Bytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  this->fill(args...);
            }
            constexpr explicit Block(void) noexcept : Bytes(new unsigned char[Size])
            {
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
            }
            virtual ~Block(void) noexcept { delete this->Bytes; }

            template <typename... Args>
            constexpr auto & fill(Args... args) const
            {
                  if(sizeof...(args) > Length) [[unlikely]]
                        throw std::out_of_range("Args.size() > Length");

                  if(this->Bytes == NULL) [[unlikely]]
                  {
                        throw std::out_of_range("Block is empty(non-valid)");
                  } else
                  {
                        std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                        typename decltype(CurrentBytes)::iterator  IteratorArgs = CurrentBytes.begin();

                        for(std::uint64_t IteratorBytes = 0; IteratorBytes < Length and IteratorArgs != CurrentBytes.end();)
                        {
                              this->Bytes[IteratorBytes] = *IteratorArgs;
                              ++IteratorArgs;
                              ++IteratorBytes;
                        }
                  }
                  return *this;
            }
            constexpr Block & clear(void) noexcept
            {
                  delete this->Bytes;
                  this->Bytes  = NULL;
                  this->Length = 0;
                  return *this;
            }
            std::uint64_t size(void) const noexcept { return this->Length; }
            template <typename Type>
            constexpr Block & resize(const Type NewSize) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "Type is not an integral, however should be");

                  if(NewSize == Length)
                        return *this;
                  if(NewSize <= 0)
                  {
                        this->clear();
                        return *this;
                  }

                  unsigned char * NewBytes = new unsigned char[NewSize];
                  assert((void("Bad allocation in Block::resize()"), NewBytes != NULL));

                  std::uint64_t Iterator = 0;
                  if(NewSize > Length)
                  {
                        for(; Iterator < Length; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                        for(; Iterator <= NewSize; ++Iterator)
                              NewBytes[Iterator] = NULL;
                  } else if(NewSize < Length)
                  {
                        for(; Iterator <= NewSize; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                  }
                  this->Length = NewSize;

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            std::string string(std::uint64_t Period = 0, const char * Separator = "", const std::uint64_t NS = 10,
                               int (*Case)(int) noexcept = ::toupper) const
            {
                  if(this->Bytes == NULL or this->Length == 0)
                        return "";
                  std::ostringstream Result;
                  std::string        StringResult;

                  std::uint64_t Iterator = 0;
                  switch(NS)
                  {
                        case 2:
                              if(Period)
                              {
                                    for(; Iterator < this->Length; ++Iterator)
                                    {
                                          if(Iterator % Period == 0)
                                                Result << Separator;
                                          Result << std::bitset<8>{static_cast<unsigned int>(this->Bytes[Iterator])}.to_string();
                                    }
                                    StringResult = Result.str();

                                    StringResult.erase(StringResult.begin());
                              } else
                              {
                                    for(; Iterator < this->Length; ++Iterator)
                                          Result << std::bitset<8>{static_cast<unsigned int>(this->Bytes[Iterator])}.to_string();
                                    StringResult = Result.str();
                              }
                              std::transform(StringResult.begin(), StringResult.end(), StringResult.begin(), Case);
                              return StringResult;
                        case 8:
                              Result.setf(std::ios_base::oct, std::ios_base::basefield);
                              break;
                        case 16:
                              Result.setf(std::ios_base::hex, std::ios_base::basefield);
                              break;
                        default:
                              Result.setf(std::ios_base::dec, std::ios_base::basefield);
                              break;
                  }

                  if(Period)
                  {
                        for(; Iterator < this->Length; ++Iterator)
                        {
                              if(Iterator % Period == 0)
                                    Result << Separator;
                              Result << static_cast<unsigned int>(this->Bytes[Iterator]);
                        }
                        StringResult = Result.str();

                        StringResult.erase(StringResult.begin());
                  } else
                  {
                        for(; Iterator < this->Length; ++Iterator)
                              Result << static_cast<unsigned int>(this->Bytes[Iterator]);
                        StringResult = Result.str();
                  }

                  std::transform(StringResult.begin(), StringResult.end(), StringResult.begin(), Case);
                  return StringResult;
            }
            // TODO
            std::string bignum(void) const noexcept
            {
                  std::string Result = "";

                  return Result;
            }
            template <typename... Args>
            constexpr Block & push_front(Args... args)
            {
                  this->Length += sizeof...(args);
                  unsigned char * NewBytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in Block::push_front(Type Arg)"), NewBytes != NULL));

                  std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                  typename decltype(CurrentBytes)::iterator  IteratorArgs = CurrentBytes.begin();

                  std::uint64_t           Iterator                        = 0;
                  constexpr std::uint64_t ArraySize                       = CurrentBytes.size();
                  for(; Iterator < ArraySize; ++Iterator)
                        NewBytes[Iterator] = CurrentBytes[Iterator];
                  for(; Iterator < this->Length; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator - ArraySize];

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            template <typename... Args>
            constexpr Block & push_back(Args... args)
            {
                  const std::uint64_t PreviousSize = this->Length;
                  this->Length += sizeof...(args);
                  unsigned char * NewBytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in Block::push_front(Type Arg)"), NewBytes != NULL));

                  std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                  typename decltype(CurrentBytes)::iterator  IteratorArgs = CurrentBytes.begin();

                  std::uint64_t Iterator                                  = 0;
                  for(; Iterator < PreviousSize; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator];
                  for(; Iterator < this->Length; ++Iterator)
                        NewBytes[Iterator] = CurrentBytes[Iterator - PreviousSize];

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Use this field read-only!
            const unsigned char * data(void) noexcept { return this->Bytes; }

            Block & operator+= (const Block & Other) noexcept;
            Block & operator+= (const Block && Other) noexcept;
            Block & operator+= (const unsigned char * Other) noexcept;
            Block & operator-= (const Block & Other) noexcept;
            Block & operator-= (const Block && Other) noexcept;
            Block & operator-= (const unsigned char * Other) noexcept;
            Block & operator= (const Block & Other) noexcept;
            Block & operator= (const Block && Other) noexcept;
            Block & operator= (const unsigned char * Other) noexcept;
            Block & operator+ (const Block & Other) noexcept;
            Block & operator+ (const Block && Other) noexcept;
            Block & operator+ (const unsigned char * Other) noexcept;
            Block & operator- (const Block & Other) noexcept;
            Block & operator- (const Block && Other) noexcept;
            Block & operator- (const unsigned char * Other) noexcept;

            bool operator== (const Block & Other) noexcept;
            bool operator== (const Block && Other) noexcept;
            bool operator== (const unsigned char * Other) noexcept;
            bool operator!= (const Block & Other) noexcept;
            bool operator!= (const Block && Other) noexcept;
            bool operator!= (const unsigned char * Other) noexcept;
            bool operator> (const Block & Other) noexcept;
            bool operator> (const Block && Other) noexcept;
            bool operator> (const unsigned char * Other) noexcept;
            bool operator>= (const Block & Other) noexcept;
            bool operator>= (const Block && Other) noexcept;
            bool operator>= (const unsigned char * Other) noexcept;
            bool operator<(const Block & Other) noexcept;
            bool operator<(const Block && Other) noexcept;
            bool operator<(const unsigned char * Other) noexcept;
            bool operator<= (const Block & Other) noexcept;
            bool operator<= (const Block && Other) noexcept;
            bool operator<= (const unsigned char * Other) noexcept;
            bool operator&& (const Block & Other) noexcept;
            bool operator&& (const Block && Other) noexcept;
            bool operator&& (const unsigned char * Other) noexcept;
            bool operator|| (const Block & Other) noexcept;
            bool operator|| (const Block && Other) noexcept;
            bool operator|| (const unsigned char * Other) noexcept;

            Block & operator& (const Block & Other) noexcept;
            Block & operator& (const Block && Other) noexcept;
            Block & operator& (const unsigned char * Other) noexcept;
            Block & operator&= (const Block & Other) noexcept;
            Block & operator&= (const Block && Other) noexcept;
            Block & operator&= (const unsigned char * Other) noexcept;
            Block & operator| (const Block & Other) noexcept;
            Block & operator| (const Block && Other) noexcept;
            Block & operator| (const unsigned char * Other) noexcept;
            Block & operator|= (const Block & Other) noexcept;
            Block & operator|= (const Block && Other) noexcept;
            Block & operator|= (const unsigned char * Other) noexcept;
            Block & operator^ (const Block & Other) noexcept;
            Block & operator^ (const Block && Other) noexcept;
            Block & operator^ (const unsigned char * Other) noexcept;
            Block & operator^= (const Block & Other) noexcept;
            Block & operator^= (const Block && Other) noexcept;
            Block & operator^= (const unsigned char * Other) noexcept;

            Block & operator>> (const std::uint64_t Shift) noexcept;
            Block & operator>>= (const std::uint64_t Shift) noexcept;
            Block & operator<< (const std::uint64_t Shift) noexcept;
            Block & operator<<= (const std::uint64_t Shift) noexcept;

            Block &         operator!(void) noexcept;
            Block &         operator~(void) noexcept;
            unsigned char * operator* (void) noexcept;
            unsigned char & operator[] (const std::uint64_t Index) noexcept;
};
template <std::uint64_t Size = 0>
class Informative_Block : public Block<Size>
{
      public:
            constexpr explicit Informative_Block(void) noexcept = default;
            ~Informative_Block(void) noexcept                   = default;
};
template <std::uint64_t Size = 0>
class VBlock
{
      private:
            unsigned char * Bytes;
            std::uint64_t   Length = Size;

      public:
            constexpr explicit VBlock(void) noexcept = default;
            virtual ~VBlock(void) noexcept { delete this->Bytes; }
};
template <std::uint64_t Size = 0>
class Informative_VBlock : public VBlock<Size>
{
      public:
            constexpr explicit Informative_VBlock(void) noexcept = default;
            ~Informative_VBlock(void) noexcept                   = default;
};
#endif
