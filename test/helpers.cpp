#include "helpers.hpp"

#include <iostream>

#include <botan/md5.h>

namespace fsig_test {

std::string hex_str(const std::vector<uint8_t> & vector_)
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

std::vector<uint8_t> md5(void const * data_, size_t size_)
{
    Botan::MD5 hash;
    hash.update(static_cast<uint8_t const *>(data_), size_);

    std::vector<uint8_t> result(hash.output_length());
    hash.final(result);
    return result;
}

std::vector<uint8_t> md5(std::string const & string_)
{
    return md5(string_.data(), string_.size());
}

} // namespace fsig_test
