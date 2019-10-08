#include "file_writer.hpp"

#include <string.h>

#include <stdexcept>
#include <iostream>

namespace fsig {

FileWriter::FileWriter(std::string const & file_path_)
    : _file_path(file_path_)
    , _mutex()
    , _file()
    , _buffer { }
{
    _file.open(file_path_, _file.binary);
    if (_file.fail())
        throw std::runtime_error("cannot open '" + file_path_ + "'");
}

FileWriter::~FileWriter() noexcept
{
    try { close(); }
    catch (...) { }
}

void FileWriter::write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_)
{
    if (buffer_.data() == nullptr)
        throw std::invalid_argument("buffer_ is NULL");

    std::lock_guard<std::mutex> _(_mutex);

    size_t offset = size_t(offset_);
    _buffer.resize(std::max(offset + buffer_.size(), _buffer.size()), 0);
    memcpy(_buffer.data() + offset, buffer_.data(), buffer_.size());

//    _file.seekp(std::ios::off_type(offset_), _file.beg);
//    _file.write(static_cast<char const *>(buffer_.data()),
//                std::streamsize(buffer_.size()));
//    if (_file.fail())
//        throw std::runtime_error(
//                "cannot write '" + _file_path + "' "
//                "at " + std::to_string(offset_) + " offset");
}

void FileWriter::flush()
{
    std::lock_guard<std::mutex> _(_mutex);
    _file.write(_buffer.data(), std::streamsize(_buffer.size()));
    _file.flush();
    _buffer.clear();
}

void FileWriter::close()
{
    flush();
    std::lock_guard<std::mutex> _(_mutex);
    _file.close();
}

} // namespace fsig
