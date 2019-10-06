#ifndef FSIG_COMMAND_LINE_HPP
#define FSIG_COMMAND_LINE_HPP

#include <stddef.h>

#include <string>
#include <vector>

namespace fsig {

struct CommandLineArgs
{
    std::string input_file;
    std::string output_file;
    size_t block_size;
    size_t io_block_size;
    size_t threads_count;
    std::string hash_algo;
};

CommandLineArgs parse_command_line(int argc_, char const * const argv_[]);

} // namespace fsig

#endif // FSIG_COMMAND_LINE_HPP
