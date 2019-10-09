#include "file_writer.hpp"

#include <string.h>

#include <stdexcept>

namespace fsig {

FileWriter::FileWriter(std::string const & file_path_, size_t io_block_size_)
    : _file_path(file_path_)
    , _max_buffer_size(io_block_size_)
    , _mutex()
    , _file()
    , _offset(0)
    , _to_write { }
    , _parts { }
    , _buffers_storage { }
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

    _parts[offset_] = _create_buffer(buffer_.data(), buffer_.size());
    offset_ = _offset;

    auto iterator = _parts.begin();
    while (iterator != _parts.end() && iterator->first == _offset) {

        std::vector<char> & part = iterator->second;
        if (_to_write.size() + part.size() > _max_buffer_size)
            break;

        _to_write.insert(_to_write.end(), part.begin(), part.end());
        _offset += part.size();
        _buffers_storage.push(std::move(part));
        iterator = _parts.erase(iterator);
    }

    if (iterator != _parts.end() &&
        _to_write.size() + iterator->second.size() > _max_buffer_size) {

        _file.write(static_cast<char const *>(_to_write.data()),
                    std::streamsize(_to_write.size()));
        _to_write.clear();
        if (_file.fail())
            throw std::runtime_error(
                    "cannot write '" + _file_path + "' "
                    "at " + std::to_string(offset_) + " offset");
    }
}

void FileWriter::close()
{
    std::lock_guard<std::mutex> _(_mutex);
    _file.write(_to_write.data(), std::streamsize(_to_write.size()));
    if (_file.fail())
        throw std::runtime_error(
                "cannot write '" + _file_path + "' "
                "at " + std::to_string(_offset) + " offset");
    _to_write.clear();
    _file.flush();
    _file.close();
    if (_file.fail())
        throw std::runtime_error("cannot close '" + _file_path + "'");
}

std::vector<char> FileWriter::_create_buffer(void const * data_, size_t size_)
{
    std::vector<char> result { };
    if (!_buffers_storage.empty()) {
        result = std::move(_buffers_storage.top());
        _buffers_storage.pop();
    }
    result.resize(size_);
    memcpy(result.data(), data_, size_);
    return result;
}

} // namespace fsig
