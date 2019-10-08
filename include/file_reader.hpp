#ifndef FSIG_FILE_READER_HPP
#define FSIG_FILE_READER_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <fstream>
#include <mutex>
#include <queue>
#include <future>

#include <boost/asio/buffer.hpp>

#include "io_interfaces.hpp"

namespace fsig {

struct FileReader : public IReader
{
    struct ReadRequest
    {
        ReadRequest(size_t offset_,
                    boost::asio::mutable_buffer const & buffer_,
                    std::promise<size_t> bytes_read_) noexcept
            : offset(offset_)
            , buffer(buffer_)
            , bytes_read(std::move(bytes_read_))
        { }

        size_t offset;
        boost::asio::mutable_buffer buffer;
        std::promise<size_t> bytes_read;
    };

    FileReader(std::string const & file_path_);

    FileReader(FileReader const &) = delete;
    virtual ~FileReader() noexcept override;
    FileReader & operator = (FileReader const &) = delete;

    virtual size_t read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_) override;
    virtual void close() override;

private:

    ReadRequest _get_request();
    void _execute_request();

    std::string _file_path;
    uint64_t _file_size;
    std::mutex _file_mutex;
    std::ifstream _file;

    std::mutex _requests_mutex;
    std::priority_queue<ReadRequest> _requests;
};

} // namespace fsig

#endif // FSIG_FILE_READER_HPP
