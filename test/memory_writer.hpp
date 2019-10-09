#ifndef FSIG_TEST_MEMORY_WRITER_HPP
#define FSIG_TEST_MEMORY_WRITER_HPP

#include <stdint.h>

#include <boost/asio/buffer.hpp>

#include "io_interfaces.hpp"

namespace fsig_test {

struct MemoryWriter : public fsig::IWriter
{
    MemoryWriter() noexcept;

    MemoryWriter(MemoryWriter const &) = default;
    virtual ~MemoryWriter() noexcept override = default;
    MemoryWriter & operator = (MemoryWriter const &) = default;

    virtual void write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_) override;
    virtual void flush() override;
    virtual void close() override;

    std::vector<uint8_t> content;
};

} // namespace fsig_test

#endif // FSIG_TEST_MEMORY_WRITER_HPP
