#include "command_line.hpp"

#include <limits>
#include <iostream>
#include <sstream>
#include <thread>

#include <boost/program_options.hpp>

#define FSIG_DEFAULT_BLOCK_SIZE (1 * 1024 * 1024) // 1MB

// recommended IO block size for Linux is 128KB
// https://github.com/coreutils/coreutils/blob/master/src/ioblksize.h#L23-L57
// recommended IO block size for Windows is 64KB (128KB is acceptable)
// https://docs.microsoft.com/en-us/previous-versions/windows/it-pro/windows-2000-server/cc938632(v=technet.10)?redirectedfrom=MSDN
#define FSIG_DEFAULT_IO_BLOCK_SIZE (128 * 1024) // 128KB

namespace fsig {

namespace bpo = boost::program_options;

static std::ostream & print_usage(
        std::ostream & stream_,
        std::string const & argv_0_,
        bpo::options_description const & description_,
        bpo::positional_options_description const & positional_)
{
    stream_ << "Usage: " << argv_0_;

    unsigned const max_count = positional_.max_total_count();
    if (max_count < std::numeric_limits<unsigned>::max()) {

        for (unsigned i = 0; i < max_count; i++)
            stream_ << " " << positional_.name_for_position(i);
    }
    else {

        std::string const & last = positional_.name_for_position(max_count);
        for (unsigned i = 0; true; i++) {

            std::string const & current = positional_.name_for_position(i);
            stream_ << " " << current;
            if (current == last) {

                stream_ << " [" << current << "] ...";
                break;
            }
        }
    }

    if (description_.options().size() > 0)
        stream_ << " " << "[options]";

    return stream_ << "\n" << description_;
}

static size_t parse_size(std::string const & value_)
{
    size_t result = 0;
    size_t i = 0;

    while (i < value_.size() && isdigit(value_.at(i))) {

        result *= 10;
        result += size_t(value_.at(i) - '0');
        ++i;
    }

    if (result == 0)
        throw bpo::invalid_option_value(value_);

    if (i == value_.size())
        return result;

    switch (value_.at(i)) {

        case 'M':
        case 'm':
            result *= 1024;
            BOOST_FALLTHROUGH;
        case 'K':
        case 'k':
            result *= 1024;
            return result;
    }

    throw bpo::invalid_option_value(value_);
}

static std::string size_to_string(size_t size_)
{
    std::string dimension = "B";
    if (size_ % 1024 == 0) {

        size_ /= 1024;
        if (size_ % 1024 == 0) {

            size_ /= 1024;
            dimension = "MB";
        }
        else dimension = "KB";
    }
    return std::to_string(size_) + dimension;
}

CommandLineArgs parse_command_line(int argc_, char const * const argv_[])
{
    CommandLineArgs result {
        "",
        "",
        FSIG_DEFAULT_BLOCK_SIZE,
        FSIG_DEFAULT_IO_BLOCK_SIZE,
        std::thread::hardware_concurrency(),
        "MD5"
    };

    bpo::options_description description("options");
    description.add_options()
            ("help,h", "Help")
            ("version,v", "Version")
            ("input,i", bpo::value<std::string>(&result.input)->required())
            ("output,o", bpo::value<std::string>(&result.output)->required())
            ("block-size",
             bpo::value<std::string>()
             ->default_value(size_to_string(result.block_size))
             ->notifier([&result] (std::string const & value_) {
                 result.block_size = parse_size(value_);
             }))
            ("io-block-size",
             bpo::value<std::string>()
             ->default_value(size_to_string(result.io_block_size))
             ->notifier([&result] (std::string const & value_) {
                 result.io_block_size = parse_size(value_);
             }))
            ("threads-count",
             bpo::value<size_t>()->default_value(result.threads_count)
             ->notifier([&result] (size_t value_) {
                 if (value_ == 0)
                     throw bpo::invalid_option_value("0");
                 result.threads_count = value_;
             }))
            ("hash-algo",
             bpo::value<std::string>(&result.hash_algo)
             ->default_value(result.hash_algo));

    bpo::positional_options_description positional;
    positional.add("input", 1);
    positional.add("output", 1);

    bpo::parsed_options parsed = bpo::command_line_parser(argc_, argv_)
                                 .options(description)
                                 .positional(positional)
                                 .run();

    bpo::variables_map variables;
    bpo::store(parsed, variables);

    if (variables.count("help") > 0) {

        std::stringstream stream;
        print_usage(stream, argv_[0], description, positional);
        throw stream.str();
    }

    if (variables.count("version") > 0) {

        std::stringstream stream;
        stream << "Version: " << FSIG_VERSION << std::endl;
        throw stream.str();
    }

    bpo::notify(variables);

    return result;
}

} // namespace fsig
