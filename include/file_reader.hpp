#ifndef FSIG_FILE_READER_HPP
#define FSIG_FILE_READER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "io_interfaces.hpp"

namespace fsig {

struct FileReader : public IReader
{
    FileReader(std::string const & file_path_, size_t io_block_size_);

    FileReader(FileReader const &) = delete;
    virtual ~FileReader() noexcept override;
    FileReader & operator = (FileReader const &) = delete;

    virtual size_t read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_) override;
    virtual void close() override;

private:

    std::string _file_path;
    size_t const _io_block_size;
    std::mutex _file_mutex;
    std::ifstream _file;
    uint64_t _file_size;
    std::atomic<uint64_t> _expected_offset;
    std::atomic_bool _is_stopped;
    std::mutex _cv_mutex;
    std::condition_variable _cv;
};

} // namespace fsig

#endif // FSIG_FILE_READER_HPP
