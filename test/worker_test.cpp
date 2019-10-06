#define BOOST_TEST_MAIN

#include <string>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <botan/md5.h>

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
        , hash(new Botan::MD5)
        , md5_zero_space(hash->output_length(), 0)
    { }

    std::string digits;
    std::unique_ptr<fsig::IReader> reader;
    std::unique_ptr<fsig_test::MemoryWriter> writer;
    std::vector<uint8_t> & result;
    std::unique_ptr<Botan::HashFunction> hash;
    std::vector<uint8_t> const md5_zero_space;
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
    BOOST_CHECK_EQUAL(hex_str(result), hex_str(md5(digits)));
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
                      hex_str(md5("0123") + md5("4567") + md5("89")));
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
                      hex_str(md5("0123") + md5("4567") + md5("89")));
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
                      hex_str(md5("0123") + md5("4567") + md5("89")));
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
                hex_str(md5("012") + md5("345") + md5("678") + md5("9")));
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
                std::unique_ptr<Botan::HashFunction>(new Botan::MD5));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(md5("012") + md5_zero_space + md5("678")));
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
                std::unique_ptr<Botan::HashFunction>(new Botan::MD5));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 1));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(md5_zero_space + md5("56789")));
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
                std::unique_ptr<Botan::HashFunction>(new Botan::MD5));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 0));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(md5("01") + md5_zero_space +
                              md5("45") + md5_zero_space +
                              md5("89")));
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
                std::unique_ptr<Botan::HashFunction>(new Botan::MD5));

    using namespace fsig_test;
    BOOST_CHECK_NO_THROW(fsig::worker_logic(context, 1));
    BOOST_CHECK_EQUAL(hex_str(result),
                      hex_str(md5_zero_space + md5("23") + md5_zero_space +
                              md5_zero_space + md5("89")));
}

BOOST_AUTO_TEST_SUITE_END()
