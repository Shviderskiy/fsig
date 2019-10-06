#ifndef FSIG_FILE_READER_HPP
#define FSIG_FILE_READER_HPP

#include <string>
#include <fstream>
#include <mutex>

#include "io_interfaces.hpp"

namespace fsig {

struct FileReader : public IReader
{
    FileReader(std::string const & file_path_);

    FileReader(FileReader const &) = delete;
    virtual ~FileReader() noexcept override = default;
    FileReader & operator = (FileReader const &) = delete;

    virtual size_t read(uint64_t offset_,
                        const boost::asio::mutable_buffer & buffer_) override;
    virtual void close() override;

private:

    std::string _file_path;
    std::mutex _mutex;
    std::ifstream _file;
};

} // namespace fsig

#endif // FSIG_FILE_READER_HPP
