#include "file_reader.hpp"

#include <stdexcept>

#include <boost/filesystem/operations.hpp>

namespace fsig {

FileReader::FileReader(std::string const & file_path_)
{
    if (!boost::filesystem::is_regular_file(file_path_))
        throw std::runtime_error("file '" + file_path_ + "' not found");
}

size_t FileReader::read(uint64_t offset_,
                        boost::asio::mutable_buffer const & buffer_)
{
    return 0;
}

void FileReader::close()
{

}

} // namespace fsig
