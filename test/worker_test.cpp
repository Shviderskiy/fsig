#define BOOST_TEST_MAIN

#include <string>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <botan/hash.h>

#include "io_interfaces.hpp"
#include "worker.hpp"

#include "helpers.hpp"
#include "memory_reader.hpp"
#include "memory_writer.hpp"

BOOST_AUTO_TEST_SUITE(fsig_worker)

struct Fixture
{
    Fixture()
        : digits("0123456789")
        , reader(new fsig_test::MemoryReader(digits.data(), digits.size()))
        , writer(new fsig_test::MemoryWriter)
        , result(writer->content)
        , hash(Botan::HashFunction::create_or_throw(FSIG_DEFAULT_HASH_ALGO))
        , hash_zeros(hash->output_length(), 0)
    { }

    std::string digits;
    std::unique_ptr<fsig::IReader> reader;
    std::unique_ptr<fsig_test::MemoryWriter> writer;
    std::vector<uint8_t> & result;
    std::unique_ptr<Botan::HashFunction> hash;
    std::vector<uint8_t> const hash_zeros;
};

BOOST_FIXTURE_TEST_CASE(empty_data_8_1_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                8, // block_size_
                1, // io_block_size_
                1, // threads_count_
                std::unique_ptr<fsig::IReader>(new fsig_test::MemoryReader),
                std::move(writer),
                std::move(hash));

    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK(result.empty());
}

BOOST_FIXTURE_TEST_CASE(empty_data_8_16_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                8,  // block_size_
                16, // io_block_size_
                1,  // threads_count_
                std::unique_ptr<fsig::IReader>(new fsig_test::MemoryReader),
                std::move(writer),
                std::move(hash));

    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK(result.empty());
}

BOOST_FIXTURE_TEST_CASE(digits_10_10_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                10, // block_size_
                10, // io_block_size_
                1, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result), hex_str(h(digits)));
}

BOOST_FIXTURE_TEST_CASE(digits_4_4_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                4, // block_size_
                4, // io_block_size_
                1, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(h("0123") + h("4567") + h("89")));
}

BOOST_FIXTURE_TEST_CASE(digits_4_3_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                4, // block_size_
                3, // io_block_size_
                1, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(h("0123") + h("4567") + h("89")));
}

BOOST_FIXTURE_TEST_CASE(digits_4_2_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                4, // block_size_
                2, // io_block_size_
                1, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(h("0123") + h("4567") + h("89")));
}

BOOST_FIXTURE_TEST_CASE(digits_3_4_1_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                3, // block_size_
                4, // io_block_size_
                1, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(
                hex_str(result),
                hex_str(h("012") + h("345") + h("678") + h("9")));
}

BOOST_FIXTURE_TEST_CASE(digits_3_4_2_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                3, // block_size_
                4, // io_block_size_
                2, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(h("012") + hash_zeros + h("678")));
}

BOOST_FIXTURE_TEST_CASE(digits_5_7_2_1, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                5, // block_size_
                7, // io_block_size_
                2, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 1));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(hash_zeros + h("56789")));
}

BOOST_FIXTURE_TEST_CASE(digits_2_2_2_0, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                2, // block_size_
                2, // io_block_size_
                2, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(h("01") + hash_zeros +
                              h("45") + hash_zeros +
                              h("89")));
}

BOOST_FIXTURE_TEST_CASE(digits_2_2_3_1, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                2, // block_size_
                2, // io_block_size_
                3, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 1));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(hash_zeros + h("23") + hash_zeros +
                              hash_zeros + h("89")));
}

BOOST_FIXTURE_TEST_CASE(digits_2_2_5_5, Fixture)
{
    std::shared_ptr<fsig::WorkerContext> context =
            std::make_shared<fsig::WorkerContext>(
                2, // block_size_
                2, // io_block_size_
                5, // threads_count_
                std::move(reader),
                std::move(writer),
                std::move(hash));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 5));
    BOOST_CHECK(result.empty());
}

BOOST_AUTO_TEST_SUITE_END()
