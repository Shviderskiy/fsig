#include "file_reader.hpp"

#include <stdexcept>

#include <boost/filesystem/operations.hpp>

namespace fsig {

FileReader::FileReader(std::string const & file_path_)
    : _file_path(file_path_)
    , _mutex()
    , _file()
{
    if (!boost::filesystem::is_regular_file(file_path_))
        throw std::runtime_error("file '" + file_path_ + "' not found");

    _file.open(file_path_);
    if (_file.fail())
        throw std::runtime_error("cannot open '" + file_path_ + "'");
}

size_t FileReader::read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_)
{
    std::lock_guard<std::mutex> _(_mutex);

    _file.seekg(std::ios::off_type(offset_), std::ios_base::beg);
    _file.read(static_cast<char *>(buffer_.data()),
               std::streamsize(buffer_.size()));
    if (_file.fail())
        throw std::runtime_error(
                "cannot read '" + _file_path + "' "
                "at " + std::to_string(offset_));
    return uint64_t(std::streamoff(_file.tellg())) - offset_;
}

void FileReader::close()
{
    _file.close();
}

} // namespace fsig
