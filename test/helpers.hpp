#ifndef FSIG_TEST_HELPERS_HPP
#define FSIG_TEST_HELPERS_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace fsig_test {

template <typename Type_, typename Allocator_>
std::vector<Type_, Allocator_> operator + (
        std::vector<Type_, Allocator_> const & left_,
        std::vector<Type_, Allocator_> const & right_)
{
    std::vector<Type_> result { };
    result.reserve(left_.size() + right_.size());
    result.insert(result.end(), left_.begin(), left_.end());
    result.insert(result.end(), right_.begin(), right_.end());
    return result;
}

std::string hex_str(std::vector<uint8_t> const & vector_);

std::vector<uint8_t> h(void const * data_, size_t size_);

template <size_t Size_>
std::vector<uint8_t> h(char const (&string_)[Size_])
{
    return h(string_, Size_ - 1);
}

std::vector<uint8_t> h(std::string const & string_);

} // namespace fsig_test

#endif // FSIG_TEST_HELPERS_HPP
