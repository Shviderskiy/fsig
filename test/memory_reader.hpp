#ifndef FSIG_TEST_MEMORY_READER_HPP
#define FSIG_TEST_MEMORY_READER_HPP

#include <stddef.h>
#include <stdint.h>

#include <boost/asio/buffer.hpp>

#include "io_interfaces.hpp"

namespace fsig_test {

struct MemoryReader : public fsig::IReader
{
    MemoryReader() noexcept;
    MemoryReader(void const * data_, size_t size_);

    MemoryReader(MemoryReader const &) = default;
    virtual ~MemoryReader() noexcept override = default;
    MemoryReader & operator = (MemoryReader const &) = default;

    virtual size_t read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_) override;
    virtual void close() override;

private:

    uint8_t const * _data;
    size_t          _size;
};

} // namespace fsig_test

#endif // FSIG_TEST_MEMORY_READER_HPP
