#ifndef FSIG_WORKER_HPP
#define FSIG_WORKER_HPP

#include <stddef.h>

#include <memory>
#include <exception>
#include <atomic>

#include <botan/hash.h>

#include "io_interfaces.hpp"

namespace fsig {

struct WorkerContext
{
    WorkerContext(size_t block_size_,
                  size_t io_block_size_,
                  size_t threads_count_,
                  std::unique_ptr<IReader> reader_,
                  std::unique_ptr<IWriter> writer_,
                  std::unique_ptr<Botan::HashFunction> hash_) noexcept
        : block_size(block_size_)
        , io_block_size(io_block_size_)
        , threads_count(threads_count_)
        , reader(std::move(reader_))
        , writer(std::move(writer_))
        , hash(std::move(hash_))
        , is_stopped(false)
    { }

    WorkerContext(WorkerContext const &) = delete;
    ~WorkerContext() noexcept = default;
    WorkerContext & operator = (WorkerContext const &) = delete;

    size_t const block_size;
    size_t const io_block_size;
    size_t const threads_count;
    std::unique_ptr<IReader> reader;
    std::unique_ptr<IWriter> writer;
    std::unique_ptr<Botan::HashFunction const> hash;
    std::atomic_bool is_stopped;
};

void worker_logic(std::shared_ptr<WorkerContext> context_,
                  size_t thread_index_);

void worker(std::shared_ptr<WorkerContext> context_,
            size_t thread_index_,
            std::exception_ptr & exception_) noexcept;

} // namespace fsig

#endif // FSIG_WORKER_HPP
