#include <libcryptocxx/block.h>

int add(int, int);

template <std::uint64_t>
class Block
{
      public:
            constexpr explicit Block(void) noexcept = default;
            ~Block(void) noexcept                   = default;
};

template <std::uint64_t>
class VBlock : public Block
{
      public:
            constexpr explicit Block(void) noexcept = default;
            ~Block(void) noexcept                   = default;
};
