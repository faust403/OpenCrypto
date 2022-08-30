#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

// Class Block representing a sequential byte set with easy-to-use methods without vectorisation
// Block<Starter Size>
template <std::uint64_t Size = 0>
class Block
{
      private:
            unsigned char * Bytes  = NULL;
            std::uint64_t   Length = Size;

      public:
            // Copy the Other Block to this. If you wrote Block<N> block(OtherBlock); | OtherBlock.size() = N+k, then
            // block.size() = N+k | N, k ∈ ℕ, k >= N
            template <std::uint64_t NewSize>
            constexpr explicit Block(Block<NewSize> & Other) noexcept :
                Length(Other.size()), Bytes(new unsigned char[Other.size()])
            {
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  std::memcpy(this->Bytes, Other.data(), this->Length);
            }
            // Move the Other Block to this. If you wrote Block<N> block(move(OtherBlock)); | OtherBlock.size() = N+k, then
            // block.size() = N+k | N, k ∈ ℕ, k >= N
            template <std::uint64_t NewSize>
            constexpr explicit Block(Block<NewSize> && Other) noexcept :
                Length(Other.size()), Bytes(new unsigned char[Other.size()])
            {
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  std::memcpy(this->Bytes, std::move(Other.data()), this->Length);
                  Other.clear();
            }
            // Copy the dynamic array to this Block. If you wrote Block<N> block(ptr); | #ptr >= N, then
            // block.size() = N. Otherwise it will considering memory rubbish | N ∈ ℕ
            template <typename Type>
            constexpr explicit Block(Type * NewBytes) noexcept : Bytes(new unsigned char[Size])
            {
                  static_assert(std::is_same_v<std::remove_const_t<Type>, unsigned char>,
                                "The constructor Block:Block(Type * NewBytes) have got Type != unsigned char");
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  std::memcpy(this->Bytes, NewBytes, Size);
            }
            // Copy the parameter pack to this Block. If you wrote Block<N> block(a1, a2, a3, ..., aN+k); | then
            // block.size() = N+k | N, k ∈ ℕ, k >= N
            template <typename... Args>
            constexpr explicit Block(Args... args) noexcept : Length(sizeof...(args))
            {
                  this->Bytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));
                  this->fill(args...);
            }
            // Simple constructor. If you wrote Block<N> block(); | then
            // block.size() = N | N ∈ ℕ
            constexpr explicit Block(void) noexcept : Bytes(new unsigned char[Size])
            {
                  assert((void("Bad allocation in constructor"), this->Bytes != NULL));

                  for(std::uint64_t Iterator = 0; Iterator < this->Length; ++Iterator)
                        this->Bytes[Iterator] = 0;
            }
            // Simple distructor with deleting the Bytes field
            virtual ~Block(void) noexcept { delete this->Bytes; }
            // Method, which is wrapping your byte sequence without copying
            //
            // If yoy will delete a pointer to Array, then Block->Bytes will be deleted also
            //
            // If ArraySize < 0, then it will throw an exception std::invalid_argument
            //
            // If ArraySize = 0, then Block->Bytes = NULL and Block->ArraySize = 0
            //
            // If #Array > ArraySize > 0, then NewBytes will be cutted
            // If #Array < ArraySize < 0, then NewBytes will considering zeros or rubbish
            // If #Array = ArraySize, then well done
            template <typename Type>
            constexpr Block & wrap(unsigned char * NewBytes, const Type ArraySize)
            {
                  static_assert(std::is_integral_v<Type>,
                                "In method Block::wrap(unsigned char * Bytes, const Type ArraySize) Type is not an "
                                "integral, however should be");

                  if(ArraySize < 0)
                  {
                        throw std::invalid_argument("ArraySize is less than zero");
                  } else if(ArraySize == 0)
                  {
                        this->Bytes  = NULL;
                        this->Length = ArraySize;
                  } else
                  {
                        this->Length = ArraySize;
                        this->Bytes  = NewBytes;
                  }
            }

            // Method, which is replacing elements of Block->Bytes from beginning by parameter pack
            // Before: block[a1, a2, a3, a4, a5, ..., aN] | N ∈ ℕ
            // block.fill(b1, b2, b3, ..., bM) | M ∈ ℕ, M <= N
            // After: Block[b1, b2, b3, ..., bM, aM+1, aM+2, ..., aN] | block.size() = N
            // If #Args > block.size() or block.size() == 0 or block.data() == NULL,
            // then you will get a std::out_of_range exception, otherwise all will work
            template <typename... Args>
            constexpr auto & fill(Args... args)
            {
                  if(sizeof...(args) > Length) [[unlikely]]
                        throw std::out_of_range("Args.size() > Length");

                  if(this->Bytes == NULL or this->Length == 0) [[unlikely]]
                  {
                        throw std::out_of_range("Block is empty(non-valid)");
                  } else
                  {
                        std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                        typename decltype(CurrentBytes)::iterator  IteratorArgs = CurrentBytes.begin();

                        for(std::uint64_t IteratorBytes = 0; IteratorBytes < this->Length and IteratorArgs != CurrentBytes.end();)
                        {
                              this->Bytes[IteratorBytes] = *IteratorArgs;
                              ++IteratorArgs;
                              ++IteratorBytes;
                        }
                  }
                  return *this;
            }
            // Method, which is replacing elements of Block->Bytes from beginning by parameter pack in reversed order
            // Before: block[a1, a2, a3, a4, a5, ..., aN] | N ∈ ℕ
            // block.rfill(b1, b2, b3, ..., bM) | M ∈ ℕ, M <= N
            // After: block[bM, bM-1, bM-2, ..., b1, aM+1, aM+2, ..., aN] | block.size() = N
            // If #Args > block.size() or block.size() == 0 or block.data() == NULL,
            // then you will get a std::out_of_range exception, otherwise all will work
            template <typename... Args>
            constexpr auto & rfill(Args... args)
            {
                  if(sizeof...(args) > Length) [[unlikely]]
                        throw std::out_of_range("Args.size() > Length");

                  if(this->Bytes == NULL or this->Length == 0) [[unlikely]]
                  {
                        throw std::out_of_range("Block is empty(non-valid)");
                  } else
                  {
                        std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                        std::reverse(CurrentBytes.begin(), CurrentBytes.end());
                        typename decltype(CurrentBytes)::iterator IteratorArgs = CurrentBytes.begin();

                        for(std::uint64_t IteratorBytes = 0; IteratorBytes < this->Length and IteratorArgs != CurrentBytes.end();)
                        {
                              this->Bytes[IteratorBytes] = *IteratorArgs;
                              ++IteratorArgs;
                              ++IteratorBytes;
                        }
                  }
                  return *this;
            }
            // Method, which is replacing elements of Block->Bytes from beginning by parameter pack in reversed order
            // Before: block[a1, a2, a3, a4, a5, ..., aN] | N ∈ ℕ
            // block.bfill(b1, b2, b3, ..., bM) | M ∈ ℕ, M <= N
            // After: block[a1, a2, ..., aN, b1, b2, b3, ..., bM] | block.size() = N
            // If #Args > block.size() or block.size() == 0 or block.data() == NULL,
            // then you will get a std::out_of_range exception, otherwise all will work
            template <typename... Args>
            constexpr auto & bfill(Args... args)
            {
                  if(sizeof...(args) > Length) [[unlikely]]
                        throw std::out_of_range("Args.size() > Length");

                  if(this->Bytes == NULL or this->Length == 0) [[unlikely]]
                  {
                        throw std::out_of_range("Block is empty(non-valid)");
                  } else
                  {
                        std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                        std::reverse(CurrentBytes.begin(), CurrentBytes.end());
                        typename decltype(CurrentBytes)::iterator IteratorArgs = CurrentBytes.begin();

                        for(std::uint64_t IteratorBytes = this->Length - 1;
                            IteratorBytes >= 0 and IteratorArgs != CurrentBytes.end();)
                        {
                              this->Bytes[IteratorBytes] = *IteratorArgs;
                              ++IteratorArgs;
                              --IteratorBytes;
                        }
                  }
                  return *this;
            }
            // Method, which is replacing elements of Block->Bytes from beginning by parameter pack in reversed order
            // Before: block[a1, a2, a3, a4, a5, ..., aN] | N ∈ ℕ
            // block.brfill(b1, b2, b3, ..., bM) | M ∈ ℕ, M <= N
            // After: block[a1, a2, ..., aN, bM, bM-1, bM-2, ..., b1] | block.size() = N
            // If #Args > block.size() or block.size() == 0 or block.data() == NULL,
            // then you will get a std::out_of_range exception, otherwise all will work
            template <typename... Args>
            constexpr auto & brfill(Args... args)
            {
                  if(sizeof...(args) > Length) [[unlikely]]
                        throw std::out_of_range("Args.size() > Length");

                  if(this->Bytes == NULL or this->Length == 0) [[unlikely]]
                  {
                        throw std::out_of_range("Block is empty(non-valid)");
                  } else
                  {
                        std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                        typename decltype(CurrentBytes)::iterator  IteratorArgs = CurrentBytes.begin();

                        for(std::uint64_t IteratorBytes = this->Length - 1;
                            IteratorBytes >= 0 and IteratorArgs != CurrentBytes.end();)
                        {
                              this->Bytes[IteratorBytes] = *IteratorArgs;
                              ++IteratorArgs;
                              --IteratorBytes;
                        }
                  }
                  return *this;
            }
            // Method, which is replacing and element in block->Bytes at Position to Byte
            // Was: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.fill_at(p, b1) | p < N, N ∈ ℕ
            // After: Block[a1, a2, a3, ..., ap = b1, ..., aN] | Block.size() = N
            //
            // if Length =< Position < 0 then it will follow std::out_of_range() exception
            //
            // if Type of Position is not an integral, then it will asserted you
            template <typename Type>
            constexpr auto & fill_at(const Type Position, const unsigned char Byte)
            {
                  static_assert(std::is_integral_v<Type>,
                                "In method Block::fill_at(const Type Position, const unsigned char Byte) Type is not an "
                                "integral, however should be");

                  if(Position < 0 or Position > this->Length - 1)
                        throw std::out_of_range("Position is outing the range of bytes");
                  else
                        this->Bytes[Position] = Byte;

                  return *this;
            }
            // Method, which is replacing and range of bytes in block->Bytes at Position to FillBytes
            // Was: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.fill_at(p, Bytes[b1, b2, b3, ..., bM], M) | p < N, (M, p ∈ ℕ)
            // After: Block[a1, a2, a3, ..., ap = b1, ap+1 = b2, ap+2 = b3, ..., ap+M = bM, ..., aN] | Block.size() = N
            //
            // if Length =< Position < 0 then it will follow std::out_of_range() exception
            //
            // if ArrayLength < #FillBytes, then FillBytes will be cutted
            // if ArrayLength > #FillBytes, then Block->Bytes may contain zeros or memory rubbish
            //
            // if Type of Position or Type of ArrayLength is not an integral, then it will asserted you
            template <typename Type1, typename Type2>
            constexpr auto & fill_range(Type1 Position, const unsigned char * FillBytes, const Type2 ArrayLength)
            {
                  static_assert(std::is_integral_v<Type1>,
                                "In method Block::fill_at(const Type Position, const unsigned char Byte, const Type2 "
                                "ArrayLength) Type1 is not an integral, however should be");
                  static_assert(std::is_integral_v<Type2>,
                                "In method Block::fill_at(const Type Position, const unsigned char Byte, const Type2 "
                                "ArrayLength) Type2 is not an integral, however should be");

                  if(this->Bytes == NULL)
                        throw std::out_of_range("Block->Bytes = NULL");

                  if(Position < 0 or Position > this->Length - 1 or Position + ArrayLength > this->Length)
                  {
                        throw std::out_of_range(
                            "Position is outing the range of bytes(also may be Position + ArrayLength >= Block->Length)");
                  } else
                  {
                        typename std::remove_const_t<decltype(Position + ArrayLength)> Iterator = 0;

                        for(; Iterator != ArrayLength; ++Iterator)
                              this->Bytes[Position + Iterator] = FillBytes[Iterator];
                  }
                  return *this;
            }
            // Method, which is removing a leading zeros
            // Before: Block[a1 = 0, a2 = 0, a3 = 0, ..., ak != 0, ak+1 != 0, ak+2 != 0, ..., af = 0, ..., aN] | N, k ∈ ℕ
            // Block.shrink_to_fit()
            // After: Block[ak, ak+1, ak+2, ..., af = 0, ..., aN]
            constexpr Block & shrink_to_fit(void) noexcept
            {
                  if(this->Bytes == NULL or this->Length == 0)
                        return *this;

                  std::uint64_t CounterOfZeros = 0;
                  while(CounterOfZeros < this->Length and !this->Bytes[CounterOfZeros])
                        ++CounterOfZeros;

                  return this->front_resize(this->Length - CounterOfZeros);
            }
            // Method, which is erasing all data from Block
            // Before: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.clear()
            // After: Block[]
            //        Block.size() = 0
            //        Block.data() = NULL
            constexpr Block & clear(void) noexcept
            {
                  delete this->Bytes;
                  this->Bytes  = NULL;
                  this->Length = 0;
                  return *this;
            }
            // Methods, which are returning a Block->Length field
            std::uint64_t size(void) const noexcept { return this->Length; }
            std::uint64_t len(void) const noexcept { return this->Length; }
            std::uint64_t length(void) const noexcept { return this->Length; }
            // Method, which is resizing your Block.
            // if arguments <= 0, then is will call Block->clear() method.
            // Otherwise if Argument < Block.size(), that will cutting your Block
            // Before: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.back_resize(k) | k ∈ ℕ
            // After: Block[a1, a2, a3, ..., aN-k]
            //
            // Otherwise it will adding zeros
            // Before: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.resize(N + k) | k ∈ ℕ
            // After: Block[a1, a2, a3, ..., aN, aN+1 = 0, aN+2 = 0, ..., ak = 0]
            //
            // if Arguments is not an integral, than it will asserted
            template <typename Type>
            constexpr Block & back_resize(const Type NewSize) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "Type is not an integral, however should be");

                  if(NewSize == this->Length)
                        return *this;
                  if(NewSize <= 0)
                  {
                        this->clear();
                        return *this;
                  }

                  unsigned char * NewBytes = new unsigned char[NewSize];
                  assert((void("Bad allocation in Block::back_resize()"), NewBytes != NULL));

                  std::uint64_t Iterator = 0;
                  if(NewSize > this->Length)
                  {
                        for(; Iterator < this->Length; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                        for(; Iterator <= NewSize; ++Iterator)
                              NewBytes[Iterator] = 0;
                  } else if(NewSize < this->Length)
                  {
                        for(; Iterator < NewSize; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                  }
                  this->Length = NewSize;

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Method, which is resizing your Block.
            // if arguments <= 0, then is will call Block->clear() method.
            // Otherwise if Argument < Block.size(), that will cutting your Block
            // Before: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.front_resize(k) | k ∈ ℕ
            // After: Block[ak+1, ak+2, ak+3, ..., aN]
            //
            // Otherwise if will adding zeros
            // Before: Block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // Block.resize(N + k) | N ∈ ℤ, k ∈ ℕ
            // After: Block[a1, a2, a3, ..., aN, aN+1 = 0, aN+2 = 0, ..., ak = 0]
            //
            // if Arguments is not an integral, than it will asserted
            template <typename Type>
            constexpr Block & front_resize(const Type NewSize) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "Type is not an integral, however should be");

                  if(NewSize == this->Length)
                        return *this;
                  if(NewSize <= 0)
                  {
                        this->clear();
                        return *this;
                  }

                  unsigned char * NewBytes   = new unsigned char[NewSize];
                  const auto      Difference = NewSize - this->Length;
                  assert((void("Bad allocation in Block::front_resize()"), NewBytes != NULL));

                  std::uint64_t Iterator = 0;
                  if(NewSize > this->Length)
                        for(; Iterator < Difference; ++Iterator)
                              NewBytes[Iterator] = 0;
                  for(; Iterator < NewSize; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator - Difference];

                  this->Length = NewSize;

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Method, which is formatting a field Block->Bytes with conversation to std::string
            // Period is a period of Separator meeting. For example
            //__________________
            // a1a2a3a4a5a6...aN -> Period = 0, ∀Separator | N ∈ ℕ
            //_____________________
            // a1a2:a3a4:a5a6:...aN -> Period = 2, Separator = ":" | N ∈ ℕ
            // And thing so
            //
            // NS is following a number system that will be represented in std::string result
            // 16 = hexademical
            // 8 = octal
            // 2 = binary
            // otherwise will decimal
            //
            // Case is following an Uppercase or Lowercase symbols. You can choose them by ::toupper or ::tolower from STL
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
            // Method, which is pushing front a block.data()
            // For example if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                block2[b1, b2, b3, ..., bM] | M ∈ ℕ
            // then block1.push_front(block2) is following
            // block1[b1, b2, b3, ..., bM, a1, a2, a3, ..., aN] | block1.size() = N + M
            // block2[b1, b2, b3, ..., bM]
            template <std::uint64_t BlockSize>
            constexpr Block & push_front(Block<BlockSize> & block) noexcept
            {
                  const std::uint64_t NewSize = block.size();
                  this->Length += NewSize;
                  unsigned char *       NewBytes = new unsigned char[this->Length];
                  const unsigned char * Array    = block.data();
                  assert((void("Bad allocation in Block::push_front(Type Arg)"), NewBytes != NULL));

                  std::uint64_t Iterator = 0;
                  for(; Iterator < NewSize; ++Iterator)
                        NewBytes[Iterator] = Array[Iterator];
                  for(; Iterator < this->Length; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator - NewSize];

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Method, which is pushing front a block.data() and doing block->clear()
            // For example if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                block2[b1, b2, b3, ..., bM] | M ∈ ℕ
            // then block1.push_front(block2) is following
            // block1[b1, b2, b3, ..., bM, a1, a2, a3, ..., aN] | block1.size() = N + M
            // block2[]
            template <std::uint64_t BlockSize>
            constexpr Block & push_front(Block<BlockSize> && block) noexcept
            {
                  auto & Res = this->push_front(block);
                  block.clear();
                  return Res;
            }
            // Method, which is pushing front a dynamic Array | #Array = ArraySize
            // For example if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                Array[b1, b2, b3, ..., bM] | M ∈ ℕ, M = ArraySize
            // then block1.push_front(block2) is following
            // block1[b1, b2, b3, ..., bM, a1, a2, a3, ..., aN] | block1.size() = N + M
            // block2[b1, b2, b3, ..., bM]

            // If ArraySize < #Array, then it will cutting Array
            // else if ArraySize > #Array, then it will considering zeros or rubbish
            //
            // If ArraySize is not an integral type, then it will asserted you
            template <typename Type>
            constexpr Block & push_front(unsigned char * Array, const Type ArraySize) noexcept
            {
                  static_assert(std::is_integral_v<Type>,
                                "In method Block::push_front(unsigned char * Array, const Type ArraySize) Type is not an "
                                "integral, however should be");

                  this->Length += ArraySize;
                  unsigned char * NewBytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in Block::push_front(Type Arg)"), NewBytes != NULL));

                  std::uint64_t Iterator = 0;
                  for(; Iterator < ArraySize; ++Iterator)
                        NewBytes[Iterator] = Array[Iterator];
                  for(; Iterator < this->Length; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator - ArraySize];

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Method, which is pushing front a parameter pack
            // For example if block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // then block.push_front(b1, b2, b3, ..., bM) | M ∈ ℕ, M = sizeof...(args) is following
            // block[b1, b2, b3, ..., bM, a1, a2, a3, ..., aN] | block.size() = N + M
            //
            // If ArraySize is not an integral type, then it will asserted you
            template <typename... Args>
            constexpr Block & push_front(Args... args) noexcept
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
            // Method, which is pushing back a block.data()
            // For example if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                block2[b1, b2, b3, ..., bM] | M ∈ ℕ
            // then block1.push_front(block2) is following
            // block1[a1, a2, a3, ..., aN, b1, b2, b3, ..., bM] | block1.size() = N + M
            // block2[b1, b2, b3, ..., bM]
            template <std::uint64_t BlockSize>
            constexpr Block & push_back(Block<BlockSize> & block) noexcept
            {
                  const std::uint64_t PreviousSize = this->Length;
                  this->Length += block.size();
                  unsigned char * NewBytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in Block::push_front(Type Arg)"), NewBytes != NULL));

                  const std::uint64_t ArraySize    = block.size();
                  unsigned char *     CurrentBytes = block.data();

                  std::uint64_t Iterator           = 0;
                  for(; Iterator < PreviousSize; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator];
                  for(; Iterator < this->Length; ++Iterator)
                        NewBytes[Iterator] = CurrentBytes[Iterator - PreviousSize];

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Method, which is pushing back a block.data() and doing block->clear()
            // For example if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                block2[b1, b2, b3, ..., bM] | M ∈ ℕ
            // then block1.push_front(block2) is following
            // block1[a1, a2, a3, ..., aN, b1, b2, b3, ..., bM] | block1.size() = N + M
            // block2[]
            template <std::uint64_t BlockSize>
            constexpr Block & push_back(Block<BlockSize> && block) noexcept
            {

                  auto & Res = this->push_front(block);
                  block.clear();
                  return Res;
            }
            // Method, which is pushing back a dynamic Array | #Array = ArraySize
            // For example if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                Array[b1, b2, b3, ..., bM] | M ∈ ℕ, M = ArraySize
            // then block1.push_front(block2) is following
            // block1[b1, b2, b3, ..., bM, a1, a2, a3, ..., aN] | block1.size() = N + M
            // block2[b1, b2, b3, ..., bM]

            // If ArraySize < #Array, then it will cutting Array
            // else if ArraySize > #Array, then it will considering zeros or rubbish
            //
            // If ArraySize is not an integral type, then it will asserted you
            template <typename Type>
            constexpr Block & push_back(unsigned char * Array, const Type ArraySize) noexcept
            {
                  static_assert(std::is_integral_v<Type>,
                                "In method Block::push_back(unsigned char * Array, const Type ArraySize) Type is not an "
                                "integral, however should be");

                  const std::uint64_t PreviousSize = this->Length;
                  this->Length += ArraySize;
                  unsigned char * NewBytes = new unsigned char[this->Length];
                  assert((void("Bad allocation in Block::push_front(Type Arg)"), NewBytes != NULL));

                  std::uint64_t Iterator = 0;
                  for(; Iterator < PreviousSize; ++Iterator)
                        NewBytes[Iterator] = this->Bytes[Iterator];
                  for(; Iterator < this->Length; ++Iterator)
                        NewBytes[Iterator] = Array[Iterator - ArraySize];

                  delete this->Bytes;
                  this->Bytes = NewBytes;
                  return *this;
            }
            // Method, which is pushing back a parameter pack
            // For example if block[a1, a2, a3, ..., aN] | N ∈ ℕ
            // then block1.push_front(b1, b2, b3, ..., bM) | M ∈ ℕ, M = sizeof...(args) is following
            // block[b1, b2, b3, ..., bM, a1, a2, a3, ..., aN] | block.size() = N + M
            //
            // If ArraySize is not an integral type, then it will asserted you
            template <typename... Args>
            constexpr Block & push_back(Args... args) noexcept
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
            // Method, which is inserting a consiquence of bytes by dynamic Array
            // For example, if block[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                 Array[b1, b2, b3, ..., bM] | M ∈ ℕ, M = #Array
            // then block.insert(k, Array, ArraySize) | k ∈ ℕ, 0 <= k <= N is following
            // block[a1, a2, a3, ..., ak, b1, b2, b3, ..., bM, ..., aN+M] | block.size() = N + M
            // Array[b1, b2, b3, ..., bM]
            //
            // if Type of k is not an integral, then is will asserted you
            // if Type of #Array is not an integral, then is will asserted you
            //
            // if ArraySize < #Array, then Array will cutted and inserted
            // if ArraySize > #Array, then inserted array will considering zeros or rubbish
            template <typename Type1, typename Type2>
            constexpr Block & insert(const Type1 Position, unsigned char * Bytes, const Type2 ArraySize) noexcept
            {
                  static_assert(std::is_integral_v<Type1>,
                                "In method Block::insert(const Type1 Position, unsigned char * Bytes, const Type2 Size) Type1 is "
                                "not an integral, however should be");
                  static_assert(std::is_integral_v<Type2>,
                                "In method Block::insert(const Type2 Position, unsigned char * Bytes, const Type2 Size) Type2 is "
                                "not an integral, however should be");

                  if(Bytes == NULL or ArraySize == 0)
                        return *this;

                  if(Position <= 0)
                  {
                        return this->push_front(Bytes, ArraySize);
                  } else if(Position >= this->Length)
                  {
                        return this->push_back(Bytes, ArraySize);
                  } else [[likely]]
                  {
                        std::uint64_t   Iterator      = 0;
                        std::uint64_t   IteratorBytes = 0;
                        unsigned char * NewBytes      = new unsigned char[this->Length + ArraySize];
                        assert(((void)"Bad allocation in Block::insert(First Position, Block block)", NewBytes != NULL));
                        this->Length += ArraySize;

                        for(; Iterator < Position; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                        for(; IteratorBytes != ArraySize; ++IteratorBytes)
                              NewBytes[Iterator++] = Bytes[IteratorBytes];
                        for(; Iterator < this->Length; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator - ArraySize];

                        delete this->Bytes;
                        this->Bytes = NewBytes;
                        return *this;
                  }
            }
            // Method, which is inserting a consiquence of bytes by other block
            // For example, if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                 block2[b1, b2, b3, ..., bM] | M ∈ ℕ, M = block2.size()
            // then block1.insert(k, block2) | k ∈ ℕ, 0 <= k <= N is following
            // block1[a1, a2, a3, ..., ak, b1, b2, b3, ..., bM, ..., aN+M] | block1.size() = N + M
            // block2[b1, b2, b3, ..., bM]
            //
            // if Type of k is not an integral, then is will asserted you
            // if Type of #Array is not an integral, then is will asserted you
            template <typename First, std::uint64_t BlockSize>
            constexpr Block & insert(First Position, Block<BlockSize> & block) noexcept
            {
                  static_assert(
                      std::is_integral_v<First>,
                      "In method Block::insert(First Position, Args... args) type First is not an integral, however should be");

                  if(block.data() == NULL or block.size() == 0)
                        return *this;

                  if(Position <= 0)
                  {
                        return this->push_front(block);

                  } else if(Position >= this->Length)
                  {
                        return this->push_back(block);
                  } else [[likely]]
                  {
                        const std::uint64_t ArraySize     = block.size();
                        unsigned char *     CurrentBytes  = block.data();
                        std::uint64_t       Iterator      = 0;
                        std::uint64_t       IteratorBytes = 0;
                        unsigned char *     NewBytes      = new unsigned char[this->Length + ArraySize];
                        assert(((void)"Bad allocation in Block::insert(First Position, Block block)", NewBytes != NULL));
                        this->Length += ArraySize;

                        for(; Iterator < Position; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                        for(; IteratorBytes != ArraySize; ++IteratorBytes)
                              NewBytes[Iterator++] = CurrentBytes[IteratorBytes];
                        for(; Iterator < this->Length; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator - ArraySize];

                        delete this->Bytes;
                        this->Bytes = NewBytes;
                        return *this;
                  }
            };
            // Method, which is inserting a consiquence of bytes by other block and invoking block2->clear()
            // For example, if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            //                 block2[b1, b2, b3, ..., bM] | M ∈ ℕ, M = block2.size()
            // then block1.insert(k, block2) | k ∈ ℕ, 0 <= k <= N is following
            // block1[a1, a2, a3, ..., ak, b1, b2, b3, ..., bM, ..., aN+M] | block1.size() = N + M
            // block2[]
            //
            // if Type of k is not an integral, then is will asserted you
            // if Type of #Array is not an integral, then is will asserted you
            template <typename First, std::uint64_t BlockSize>
            constexpr Block & insert(const First Position, Block<BlockSize> && block) noexcept
            {
                  auto & Res = this->insert(Position, block);
                  block.clear();
                  return Res;
            }
            // Method, which is inserting a consiquence of bytes by parameter pack
            // For example, if block1[a1, a2, a3, ..., aN] | N ∈ ℕ
            // then block1.insert(k, b1, b2, b3, ..., bM) | k, M ∈ ℕ, 0 <= k <= N is following
            // block1[a1, a2, a3, ..., ak, b1, b2, b3, ..., bM, ..., aN+M] | block1.size() = N + M
            //
            // if Type of k is not an integral, then is will asserted you
            // if Type of #Array is not an integral, then is will asserted you
            template <typename First, typename... Args>
            constexpr Block & insert(First first, Args... args) noexcept
            {
                  static_assert(
                      std::is_integral_v<First>,
                      "In method Block::insert(First first, Args... args) type First is not an integral, however should be");

                  if(first <= 0)
                  {
                        return this->push_front(args...);

                  } else if(first >= Length)
                  {
                        return this->push_back(args...);
                  } else [[likely]]
                  {
                        std::array<unsigned char, sizeof...(args)> CurrentBytes = std::array{std::forward<Args>(args)...};
                        typename decltype(CurrentBytes)::iterator  IteratorArgs = CurrentBytes.begin();
                        constexpr std::uint64_t                    ArraySize    = CurrentBytes.size();
                        std::uint64_t                              Iterator     = 0;
                        unsigned char *                            NewBytes     = new unsigned char[this->Length + ArraySize];
                        assert(((void)"Bad allocation in Block::insert(First first, Args... args)", NewBytes != NULL));
                        this->Length += ArraySize;

                        for(; Iterator < first; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator];
                        for(; IteratorArgs != CurrentBytes.end(); ++IteratorArgs)
                              NewBytes[Iterator++] = *IteratorArgs;
                        for(; Iterator < this->Length; ++Iterator)
                              NewBytes[Iterator] = this->Bytes[Iterator - ArraySize];

                        delete this->Bytes;
                        this->Bytes = NewBytes;
                        return *this;
                  }
            };
            // Method, which is returning pointer to block->Bytes
            // !!!Use this method for read-only. Otherwise block.size() may be wrong!!!
            const unsigned char * data(void) noexcept { return this->Bytes; }
            std::string           bigdec(void) noexcept;
            std::string           bighex(void) noexcept;
            std::string           bigbin(void) noexcept;

            Block & operator+= (const Block & Other) noexcept;
            Block & operator-= (const Block & Other) noexcept;
            Block & operator*= (const Block & Other) noexcept;
            Block & operator/= (const Block & Other) noexcept;
            Block & operator%= (const Block & Other) noexcept;
            Block & operator= (const Block & Other) noexcept;
            Block & operator+ (const Block & Other) noexcept;
            Block & operator- (const Block & Other) noexcept;
            Block & operator* (const Block & Other) noexcept;
            Block & operator/ (const Block & Other) noexcept;
            Block & operator% (const Block & Other) noexcept;

            Block & operator+= (const Block && Other) noexcept;
            Block & operator-= (const Block && Other) noexcept;
            Block & operator*= (const Block && Other) noexcept;
            Block & operator/= (const Block && Other) noexcept;
            Block & operator%= (const Block && Other) noexcept;
            Block & operator= (const Block && Other) noexcept;
            Block & operator+ (const Block && Other) noexcept;
            Block & operator- (const Block && Other) noexcept;
            Block & operator* (const Block && Other) noexcept;
            Block & operator/ (const Block && Other) noexcept;
            Block & operator% (const Block && Other) noexcept;

            Block & operator+= (const unsigned char * Other) noexcept;
            Block & operator-= (const unsigned char * Other) noexcept;
            Block & operator*= (const unsigned char * Other) noexcept;
            Block & operator/= (const unsigned char * Other) noexcept;
            Block & operator%= (const unsigned char * Other) noexcept;
            Block & operator= (const unsigned char * Other) noexcept;
            Block & operator+ (const unsigned char * Other) noexcept;
            Block & operator- (const unsigned char * Other) noexcept;
            Block & operator* (const unsigned char * Other) noexcept;
            Block & operator/ (const unsigned char * Other) noexcept;
            Block & operator% (const unsigned char * Other) noexcept;

            template <typename Type>
            Block & operator+= (const Type Other) noexcept;
            template <typename Type>
            Block & operator-= (const Type Other) noexcept;
            template <typename Type>
            Block & operator*= (const Type Other) noexcept;
            template <typename Type>
            Block & operator/= (const Type Other) noexcept;
            template <typename Type>
            Block & operator%= (const Type Other) noexcept;
            template <typename Type>
            Block & operator= (const Type Other) noexcept;
            template <typename Type>
            Block & operator+ (const Type Other) noexcept;
            template <typename Type>
            Block & operator- (const Type Other) noexcept;
            template <typename Type>
            Block & operator* (const Type Other) noexcept;
            template <typename Type>
            Block & operator/ (const Type Other) noexcept;
            template <typename Type>
            Block & operator% (const Type Other) noexcept;

            bool operator> (const Block & Other) noexcept;
            bool operator>= (const Block & Other) noexcept;
            bool operator<(const Block & Other) noexcept;
            bool operator<= (const Block & Other) noexcept;
            bool operator&& (const Block & Other) noexcept;
            bool operator|| (const Block & Other) noexcept;
            bool operator== (const Block & Other) noexcept;
            bool operator!= (const Block & Other) noexcept;

            bool operator> (const Block && Other) noexcept;
            bool operator>= (const Block && Other) noexcept;
            bool operator<(const Block && Other) noexcept;
            bool operator<= (const Block && Other) noexcept;
            bool operator&& (const Block && Other) noexcept;
            bool operator|| (const Block && Other) noexcept;
            bool operator== (const Block && Other) noexcept;
            bool operator!= (const Block && Other) noexcept;

            bool operator> (const unsigned char * Other) noexcept;
            bool operator>= (const unsigned char * Other) noexcept;
            bool operator<(const unsigned char * Other) noexcept;
            bool operator<= (const unsigned char * Other) noexcept;
            bool operator&& (const unsigned char * Other) noexcept;
            bool operator|| (const unsigned char * Other) noexcept;
            bool operator== (const unsigned char * Other) noexcept;
            bool operator!= (const unsigned char * Other) noexcept;

            template <typename Type>
            bool operator> (const Type Other) noexcept;
            template <typename Type>
            bool operator>= (const Type Other) noexcept;
            template <typename Type>
            bool operator<(const Type Other) noexcept;
            template <typename Type>
            bool operator<= (const Type Other) noexcept;
            template <typename Type>
            bool operator&& (const Type Other) noexcept;
            template <typename Type>
            bool operator|| (const Type Other) noexcept;
            template <typename Type>
            bool operator== (const Type Other) noexcept;
            template <typename Type>
            bool operator!= (const Type Other) noexcept;

            Block & operator| (const Block & Other) noexcept;
            Block & operator|= (const Block & Other) noexcept;
            Block & operator^ (const Block & Other) noexcept;
            Block & operator^= (const Block & Other) noexcept;
            Block & operator& (const Block & Other) noexcept;
            Block & operator&= (const Block & Other) noexcept;

            Block & operator| (const Block && Other) noexcept;
            Block & operator|= (const Block && Other) noexcept;
            Block & operator^ (const Block && Other) noexcept;
            Block & operator^= (const Block && Other) noexcept;
            Block & operator& (const Block && Other) noexcept;
            Block & operator&= (const Block && Other) noexcept;

            Block & operator| (const unsigned char * Other) noexcept;
            Block & operator|= (const unsigned char * Other) noexcept;
            Block & operator^ (const unsigned char * Other) noexcept;
            Block & operator^= (const unsigned char * Other) noexcept;
            Block & operator& (const unsigned char * Other) noexcept;
            Block & operator&= (const unsigned char * Other) noexcept;

            template <typename Type>
            Block & operator| (const Type Other) noexcept;
            template <typename Type>
            Block & operator|= (const Type Other) noexcept;
            template <typename Type>
            Block & operator^ (const Type Other) noexcept;
            template <typename Type>
            Block & operator^= (const Type Other) noexcept;
            template <typename Type>
            Block & operator& (const Type Other) noexcept;
            template <typename Type>
            Block & operator&= (const Type Other) noexcept;

            template <typename Type>
            Block & operator>> (const Type Shift) noexcept;
            template <typename Type>
            Block & operator>>= (const Type Shift) noexcept;
            template <typename Type>
            Block & operator<< (const Type Shift) noexcept;
            template <typename Type>
            Block & operator<<= (const Type Shift) noexcept;

            Block &         operator!(void) noexcept;
            Block &         operator~(void) noexcept;
            unsigned char * operator* (void) noexcept;

            template <typename Type>
            unsigned char & operator[] (const Type Index) noexcept;
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
