#include "helpers.hpp"

#include <iostream>

#include <botan/hash.h>

namespace fsig_test {

std::string hex_str(std::vector<uint8_t> const & vector_)
{
    static char const alphabet[] = "0123456789abcdef";

    std::string result = "";
    result.reserve(2 * vector_.size());
    for (uint8_t x : vector_) {

        result += alphabet[(x >> 8) & 0xf];
        result += alphabet[x & 0xf];
    }
    return result;
}

std::vector<uint8_t> h(void const * data_, size_t size_)
{
    std::unique_ptr<Botan::HashFunction> hash =
            Botan::HashFunction::create_or_throw(FSIG_DEFAULT_HASH_ALGO);
    hash->update(static_cast<uint8_t const *>(data_), size_);

    std::vector<uint8_t> result(hash->output_length());
    hash->final(result);
    return result;
}

std::vector<uint8_t> h(std::string const & string_)
{
    return h(string_.data(), string_.size());
}

} // namespace fsig_test
