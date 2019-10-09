#include "file_reader.hpp"

#include <stdexcept>
#include <algorithm>

#include <boost/filesystem/operations.hpp>

namespace fsig {

bool operator < (FileReader::ReadRequest const & left_,
                 FileReader::ReadRequest const & right_) noexcept
{
    return left_.offset > right_.offset;
}

FileReader::FileReader(std::string const & file_path_)
    : _file_path(file_path_)
    , _file_size(boost::filesystem::file_size(file_path_))
    , _file_mutex()
    , _file()
    , _requests_mutex()
    , _requests { }
{
    if (!boost::filesystem::is_regular_file(file_path_))
        throw std::runtime_error("file '" + file_path_ + "' not found");

    _file.open(file_path_, _file.binary);
    if (_file.fail())
        throw std::runtime_error("cannot open '" + file_path_ + "'");
}

FileReader::~FileReader() noexcept
{
    try { close(); }
    catch (...) { }
}

size_t FileReader::read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_)
{
    if (offset_ >= _file_size)
        return 0;

    if (buffer_.data() == nullptr)
        throw std::invalid_argument("buffer_ is NULL");

    std::promise<size_t> bytes_read;
    std::future<size_t> result = bytes_read.get_future();

    {
        std::lock_guard<std::mutex> _(_requests_mutex);
        _requests.emplace(offset_, buffer_, std::move(bytes_read));
    }

    _execute_request();
    return result.get();
}

void FileReader::close()
{
    std::lock_guard<std::mutex> _(_file_mutex);
    _file.close();
    if (_file.fail())
        throw std::runtime_error("cannot close '" + _file_path + "'");
}

FileReader::ReadRequest FileReader::_get_request()
{
    std::lock_guard<std::mutex> _(_requests_mutex);

    ReadRequest request =
            std::move(const_cast<ReadRequest &>(_requests.top()));
    _requests.pop();
    return request;
}

void FileReader::_execute_request()
{
    std::lock_guard<std::mutex> _(_file_mutex);

    ReadRequest request = _get_request();

    try {

        _file.seekg(std::ios::off_type(request.offset), _file.beg);
        _file.read(static_cast<char *>(request.buffer.data()),
                   std::min(std::streamsize(request.buffer.size()),
                            std::streamsize(_file_size - request.offset)));
        if (_file.fail())
            throw std::runtime_error(
                    "cannot read '" + _file_path + "' "
                    "at " + std::to_string(request.offset) + " offset");

        request.bytes_read.set_value(size_t(_file.gcount()));
    }
    catch (...) {

        request.bytes_read.set_exception(std::current_exception());
    }
}

} // namespace fsig
