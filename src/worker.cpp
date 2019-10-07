#include "worker.hpp"

#include <stdint.h>

#include <vector>

namespace fsig {

struct Buffer
{
    Buffer(IReader & reader_, size_t size_)
        : _reader(reader_)
        , _buffer(size_)
        , _real_offset(0)
        , _data_offset(0)
        , _bytes_read(0)
    { }

    void read(uint64_t offset_)
    {
        if (_real_offset <= offset_ && offset_ < _real_offset + _bytes_read) {

            _data_offset = size_t(offset_ - _real_offset);
            return;
        }

        _real_offset = offset_;
        _data_offset = 0;
        _bytes_read =
                _reader.read(offset_, { _buffer.data(), _buffer.size() });
    }

    uint8_t const * data() const noexcept
    {
        return _buffer.data() + _data_offset;
    }

    size_t size() const noexcept
    {
        return _bytes_read - _data_offset;
    }

private:

    IReader & _reader;
    std::vector<uint8_t> _buffer;
    uint64_t _real_offset;
    size_t _data_offset;
    size_t _bytes_read;
};

void worker_logic(std::shared_ptr<WorkerContext> context_,
                  size_t thread_index_)
{
    size_t block_index = thread_index_;

    Buffer buffer(*context_->reader, context_->io_block_size);

    std::unique_ptr<Botan::HashFunction> hash = context_->hash->copy_state();
    size_t const hash_size = hash->output_length();

    std::vector<uint8_t> digest(hash_size);

    size_t bytes_hashed = 0;

    uint64_t offset = block_index * context_->block_size;

    while (!context_->is_stopped.load()) {

        buffer.read(offset);
        if (buffer.size() != 0) {

            if (bytes_hashed == 0)
                hash->clear();

            size_t bytes_to_update =
                    std::min(buffer.size(),
                             context_->block_size - bytes_hashed);
            hash->update(buffer.data(), bytes_to_update);
            bytes_hashed += bytes_to_update;
            if (bytes_hashed != context_->block_size) {

                offset += buffer.size();
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
