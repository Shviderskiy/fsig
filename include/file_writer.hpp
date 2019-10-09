#ifndef FSIG_FILE_WRITER_HPP
#define FSIG_FILE_WRITER_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include <stack>
#include <map>

#include <boost/asio/buffer.hpp>

#include "io_interfaces.hpp"

namespace fsig {

struct FileWriter : public IWriter
{
    FileWriter(std::string const & file_path_, size_t io_block_size_);

    FileWriter(FileWriter const &) = delete;
    virtual ~FileWriter() noexcept override;
    FileWriter & operator = (FileWriter const &) = delete;

    virtual void write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_) override;
    virtual void flush() override;
    virtual void close() override;

private:

    std::vector<char> _create_buffer(void const * data_, size_t size_);

    std::string _file_path;
    size_t const _max_buffer_size;
    std::mutex _mutex;
    std::ofstream _file;

    uint64_t _offset;
    std::vector<char> _to_write;
    std::map<uint64_t, std::vector<char>> _parts;
    std::stack<std::vector<char>> _buffers_storage;
};

} // namespace fsig

#endif // FSIG_FILE_WRITER_HPP
