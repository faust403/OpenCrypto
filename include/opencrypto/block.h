#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

// Base class(abstract), which was created for polymorphism with pointers
class Block
{
      protected:
            std::uint64_t   Length = 0;
            unsigned char * Bytes  = NULL;

      public:
            // Section constructors & distructors
            Block(void) noexcept = default;

            explicit Block(const std::string_view Other) noexcept { }

            template <typename UserType>
            explicit Block(const unsigned char * Bytes, const UserType & Size) noexcept
            {
            }
            template <typename UserType>
            explicit Block(const unsigned char * Bytes, const UserType && Size) noexcept
            {
            }
            explicit Block(const std::shared_ptr<Block> & Other) noexcept { }
            explicit Block(const std::unique_ptr<Block> & Other) noexcept { }

            virtual ~Block(void) noexcept { delete this->Bytes; };

            // Section getters
            virtual const unsigned char * data(void) const noexcept                                        = 0;
            virtual std::uint64_t         len(void) const noexcept                                         = 0;
            virtual std::uint64_t         size(void) const noexcept                                        = 0;
            virtual std::uint64_t         length(void) const noexcept                                      = 0;
            virtual std::string           string(void) const noexcept                                      = 0;
            virtual std::string           bignum(void) const noexcept                                      = 0;

            // Section setters
            virtual void load(const unsigned char * OtherData, const std::uint64_t OtherLength) noexcept   = 0;
            virtual void update(const unsigned char * OtherData, const std::uint64_t OtherLength) noexcept = 0;

            // Section utils
            virtual void clear(void) noexcept                                                              = 0;
};

class SBlock : public Block
{
      public:
            template <typename Type>
            explicit SBlock(const Type Length) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "SBlock<LenType>::SBlock(const Type Length): Type is not an integral");
                  static_assert(std::is_unsigned_v<Type>, "SBlock<LenType>::SBlock(const Type Length): Type is not unsigned");

                  this->Block::Bytes = new (std::nothrow) unsigned char[Length]();
                  assert((void("SBlock does not allocate memory. It may be followed by absence of memory or 0 size"),
                          this->Block::Bytes != NULL));

                  this->Block::Length = Length;
            };
            template <typename Type>
            explicit SBlock(const unsigned char * NewBytes, const Type Length) noexcept
            {
                  static_assert(std::is_integral_v<Type>, "SBlock<LenType>::SBlock(const Type Length): Type is not an integral");
                  static_assert(std::is_unsigned_v<Type>, "SBlock<LenType>::SBlock(const Type Length): Type is not unsigned");

                  this->Block::Bytes = new (std::nothrow) unsigned char[Length];
                  assert((void("SBlock does not allocate memory. It may be followed by absence of memory or 0 size"),
                          this->Block::Bytes != NULL));
                  std::memcpy(this->Block::Bytes, NewBytes, Length);

                  this->Block::Length = Length;
            };
            explicit SBlock(const SBlock & Other) noexcept
            {
                  if(Other.len()) [[likely]]
                  {
                        this->Block::Length            = Other.len();
                        this->Block::Bytes             = new (std::nothrow) unsigned char[this->Block::Length]();

                        const unsigned char * DataCopy = Other.data();
                        for(std::uint64_t Index = 0; Index < this->Block::Length; ++Index)
                              this->Block::Bytes[Index] = DataCopy[Index];
                  }
            }
            explicit SBlock(SBlock && Other) noexcept : SBlock(Other) { Other.clear(); }
            explicit SBlock(const std::string_view && BigNumber) noexcept;
            ~SBlock(void) noexcept = default;

            // Section getters
            const unsigned char * data(void) const noexcept override { return this->Block::Bytes; }
            std::uint64_t         len(void) const noexcept { return this->Block::Length; }
            std::uint64_t         size(void) const noexcept { return this->Block::Length; }
            std::uint64_t         length(void) const noexcept { return this->Block::Length; }
            std::string           string(void) const noexcept override { }
            std::string           bignum(void) const noexcept override { }

            // Section setters
            void load(const unsigned char * OtherData, const std::uint64_t OtherLength) noexcept override
            {
                  if(OtherData == NULL or OtherLength == 0)
                  {
                        this->Block::Bytes  = NULL;
                        this->Block::Length = 0;
                  } else
                  {
                        for(std::uint64_t Index = 0; Index < OtherLength; ++Index)
                              this->Block::Bytes[Index] = OtherData[Index];
                  }
            }
            void update(const unsigned char * OtherData, const std::uint64_t OtherLength) noexcept override
            {
                  this->load(OtherData, OtherLength);
            }
            // Section utils
            void clear(void) noexcept override
            {
                  delete this->Block::Bytes;
                  this->Block::Length = 0;
            }
};

#endif
