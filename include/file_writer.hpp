#ifndef FSIG_FILE_WRITER_HPP
#define FSIG_FILE_WRITER_HPP

#include <string>
#include <fstream>
#include <mutex>

#include "io_interfaces.hpp"

namespace fsig {

struct FileWriter : public IWriter
{
    FileWriter(std::string const & file_path_);

    FileWriter(FileWriter const &) = delete;
    virtual ~FileWriter() noexcept override;
    FileWriter & operator = (FileWriter const &) = delete;

    virtual void write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_) override;
    virtual void flush() override;
    virtual void close() override;

private:

    std::string _file_path;
    std::mutex _mutex;
    std::ofstream _file;
};

} // namespace fsig

#endif // FSIG_FILE_WRITER_HPP
