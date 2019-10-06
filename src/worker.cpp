#include "worker.hpp"

#include <stdint.h>

#include <vector>

namespace fsig {

void worker_logic(std::shared_ptr<WorkerContext> context_,
                  size_t thread_index_)
{
    size_t block_index = thread_index_;

    std::vector<uint8_t> buffer(context_->io_block_size);

    std::unique_ptr<Botan::HashFunction> hash = context_->hash->copy_state();
    size_t const hash_size = hash->output_length();

    std::vector<uint8_t> digest(hash_size);

    size_t bytes_hashed = 0;

    uint64_t offset = block_index * context_->block_size;

    while (!context_->is_stopped.load()) {

        size_t bytes_read = context_->reader->read(
                                offset, { buffer.data(), buffer.size() });
        if (bytes_read != 0) {

            if (bytes_hashed == 0)
                hash->clear();

            size_t bytes_to_update =
                    std::min(bytes_read, context_->block_size - bytes_hashed);
            hash->update(buffer.data(), bytes_to_update);
            bytes_hashed += bytes_to_update;
            if (bytes_hashed != context_->block_size) {

                offset += bytes_read;
            }
            else {

                hash->final(digest);
                context_->writer->write(block_index * hash_size,
                                        { digest.data(), digest.size() });

                block_index += context_->threads_count;
                offset = block_index * context_->block_size;
                bytes_hashed = 0;
            }
        }
        else {

            if (bytes_hashed != 0) {

                hash->final(digest);
                context_->writer->write(block_index * hash_size,
                                        { digest.data(), digest.size() });
            }
            return;
        }
    }
}

void worker(std::shared_ptr<WorkerContext> context_,
            size_t thread_index_,
            std::exception_ptr & exception_) noexcept
{
    try {

        worker_logic(context_, thread_index_);
    }
    catch (...) {

        context_->is_stopped.store(true);
        exception_ = std::current_exception();
    }
}

} // namespace fsig
