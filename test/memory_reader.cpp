#include "memory_reader.hpp"

#include <string.h>

#include <stdexcept>
#include <algorithm>

namespace fsig_test {

MemoryReader::MemoryReader() noexcept
    : _data(nullptr)
    , _size(0)
{ }

MemoryReader::MemoryReader(const void * data_, size_t size_)
    : _data(static_cast<uint8_t const *>(data_))
    , _size(size_)
{ }

size_t MemoryReader::read(uint64_t offset_,
                          boost::asio::mutable_buffer const & buffer_)
{
    if (buffer_.data() == nullptr)
        throw std::invalid_argument("buffer_ is NULL");

    if (offset_ >= _size)
        return 0;

    size_t offset = size_t(offset_);

    size_t bytes_to_copy = std::min(buffer_.size(), _size - offset);
    memcpy(buffer_.data(), _data + offset, bytes_to_copy);
    return bytes_to_copy;
}

void MemoryReader::close()
{
    _data = nullptr;
    _size = 0;
}

} // namespace fsig_test
