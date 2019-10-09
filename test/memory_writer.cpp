#include "memory_writer.hpp"

#include <string.h>

#include <algorithm>

namespace fsig_test {

MemoryWriter::MemoryWriter() noexcept
    : content { }
{ }

void MemoryWriter::write(uint64_t offset_,
                         boost::asio::const_buffer const & buffer_)
{
    if (buffer_.data() == nullptr)
        throw std::invalid_argument("buffer_ is NULL");

    size_t offset = size_t(offset_);

    content.resize(std::max(content.size(), offset + buffer_.size()), 0);
    memcpy(content.data() + offset, buffer_.data(), buffer_.size());
}

void MemoryWriter::close()
{
    content.clear();
}

} // namespace fsig_test
