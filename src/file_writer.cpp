#include "file_writer.hpp"

#include <stdexcept>

namespace fsig {

FileWriter::FileWriter(std::string const & file_path_)
    : _file_path(file_path_)
    , _mutex()
    , _file()
{
    _file.open(file_path_);
    if (_file.fail())
        throw std::runtime_error("cannot open '" + file_path_ + "'");
}

void FileWriter::write(uint64_t offset_,
                       boost::asio::const_buffer const & buffer_)
{
    if (buffer_.data() == nullptr)
        throw std::invalid_argument("buffer_ is NULL");

    std::lock_guard<std::mutex> _(_mutex);

    _file.seekp(std::ios::off_type(offset_), _file.beg);
    _file.write(static_cast<char const *>(buffer_.data()),
                std::streamsize(buffer_.size()));
    if (_file.fail())
        throw std::runtime_error(
                "cannot write '" + _file_path + "' "
                "at " + std::to_string(offset_) + " offset");
}

void FileWriter::flush()
{
    _file.flush();
}

void FileWriter::close()
{
    _file.close();
}

} // namespace fsig
