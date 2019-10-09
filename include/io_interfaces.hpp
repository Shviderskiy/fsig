#ifndef FSIG_IO_INTERFACES_HPP
#define FSIG_IO_INTERFACES_HPP

#include <stddef.h>
#include <stdint.h>

#include <boost/asio/buffer.hpp>

namespace fsig {

struct IReader
{
    virtual ~IReader() noexcept = default;
    virtual size_t read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_) = 0;
    virtual void close() = 0;
};

struct IWriter
{
    virtual ~IWriter() noexcept = default;
    virtual void write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_) = 0;
    virtual void close() = 0;
};

} // namespace fsig

#endif // FSIG_IO_INTERFACES_HPP
