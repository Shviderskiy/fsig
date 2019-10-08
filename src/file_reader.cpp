#include "file_reader.hpp"

#include <stdexcept>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <iostream>
namespace fsig {

FileReader::FileReader(std::string const & file_path_, size_t io_block_size_)
    : _file_path(file_path_)
    , _io_block_size(io_block_size_)
    , _file_mutex()
    , _file()
    , _file_size(0)
    , _expected_offset(0)
    , _is_stopped(false)
    , _cv_mutex()
    , _cv()
{
    if (!boost::filesystem::is_regular_file(file_path_))
        throw std::runtime_error("file '" + file_path_ + "' not found");

    _file.open(file_path_, _file.binary);
    if (_file.fail())
        throw std::runtime_error("cannot open '" + file_path_ + "'");

    _file_size = boost::filesystem::file_size(file_path_);
}

size_t FileReader::read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_)
{
    if (offset_ >= _file_size)
        return 0;

    if (buffer_.data() == nullptr)
        throw std::invalid_argument("buffer_ is NULL");

    {
        std::unique_lock<std::mutex> lock(_cv_mutex);
        _cv.wait(lock, [&] {
            return _is_stopped.load() || offset_ == _expected_offset.load();
        });
    }

    if (_is_stopped.load())
        return 0;

    _expected_offset.store(offset_ + _io_block_size);
    _cv.notify_all();

    std::lock_guard<std::mutex> _(_file_mutex);

    _file.seekg(std::ios::off_type(offset_), _file.beg);

    size_t bytes_to_read =
            std::min(buffer_.size(), size_t(_file_size - offset_));
    _file.read(static_cast<char *>(buffer_.data()),
               std::streamsize(bytes_to_read));
    if (_file.fail())
        throw std::runtime_error(
                "cannot read '" + _file_path + "' "
                "at " + std::to_string(offset_) + " offset");
    return size_t(_file.gcount());
}

void FileReader::close()
{
    _is_stopped.store(true);
    _cv.notify_all();

    std::lock_guard<std::mutex> _(_file_mutex);
    _file.close();
}

} // namespace fsig
