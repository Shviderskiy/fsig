#ifndef FSIG_FILE_WRITER_HPP
#define FSIG_FILE_WRITER_HPP

#include <string.h>

#include "io_interfaces.hpp"

namespace fsig {

struct FileWriter : public IWriter
{
    FileWriter(std::string const & file_path_);

    FileWriter(FileWriter const &) = delete;
    virtual ~FileWriter() noexcept override = default;
    FileWriter & operator = (FileWriter const &) = delete;

    virtual void write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_) override;
    virtual void flush() override;
    virtual void close() override;

private:

};

} // namespace fsig

#endif // FSIG_FILE_WRITER_HPP
