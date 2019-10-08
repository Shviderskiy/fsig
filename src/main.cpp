#include <stddef.h>

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <exception>
#include <iostream>

#include "command_line.hpp"
#include "file_reader.hpp"
#include "file_writer.hpp"
#include "worker.hpp"

int main(int argc_, char * argv_[])
{
    try {

        fsig::CommandLineArgs command_line_args =
                fsig::parse_command_line(argc_, argv_);

        std::shared_ptr<fsig::WorkerContext> context =
                std::make_shared<fsig::WorkerContext>(
                    command_line_args.block_size,
                    command_line_args.io_block_size,
                    command_line_args.threads_count,
                    std::unique_ptr<fsig::IReader>(
                        new fsig::FileReader(command_line_args.input_file,
                                             command_line_args.io_block_size)),
                    std::unique_ptr<fsig::IWriter>(
                        new fsig::FileWriter(command_line_args.output_file)),
                    Botan::HashFunction::create_or_throw(
                        command_line_args.hash_algo));

        std::vector<std::exception_ptr> exceptions(
                    command_line_args.threads_count, nullptr);

        std::vector<std::thread> threads { };
        threads.reserve(command_line_args.threads_count - 1);

        {
            size_t i = 0;
            for ( ; i < command_line_args.threads_count - 1; i++)
                threads.emplace_back([=, &exceptions] {
                    fsig::worker(std::move(context), i, exceptions[i]);
                });

            fsig::worker(context, i, exceptions[i]);
        }

        for (std::thread & x : threads) {

            if (x.joinable())
                x.join();
        }

        for (std::exception_ptr const & x : exceptions) {

            if (x != nullptr)
                std::rethrow_exception(x);
        }

        context->writer->flush();

        context->reader->close();
        context->writer->close();

        return 0;
    }
    catch (std::string const & info) {

        std::cout << info;
        return 0;
    }
    catch (std::exception const & error) {

        std::cerr << error.what() << std::endl;
        return -1;
    }
    catch (...) {

        std::cerr << "Unknown exception" << std::endl;
        return -1;
    }
}
