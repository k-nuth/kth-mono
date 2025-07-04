// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <filesystem>

#include <test_helpers.hpp>

#include <kth/database.hpp>

using namespace boost::system;
using namespace std::filesystem;
using namespace kth::domain::chain;
using namespace kth::database;

#define DIRECTORY "history_database"

class history_database_directory_setup_fixture {
public:
    history_database_directory_setup_fixture() {
        std::error_code ec;
        remove_all(DIRECTORY, ec);
        REQUIRE(create_directories(DIRECTORY, ec));
    }

    ////~history_database_directory_setup_fixture() {
    ////    error_code ec;
    ////    remove_all(DIRECTORY, ec);
    ////}
};

BOOST_FIXTURE_TEST_SUITE(database_tests, history_database_directory_setup_fixture)

#ifdef KTH_DB_HISTORY
TEST_CASE("history database  test", "[None]")
{
    const short_hash key1 = base16_literal("a006500b7ddfd568e2b036c65a4f4d6aaa0cbd9b");
    output_point out11{ hash_literal("4129e76f363f9742bc98dd3d40c99c9066e4d53b8e10e5097bd6f7b5059d7c53"), 110 };
    size_t const out_h11 = 110;
    const uint64_t value11 = 4;
    output_point out12{ hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"), 4 };
    size_t const out_h12 = 120;
    const uint64_t value12 = 8;
    output_point out13{ hash_literal("4129e76f363f9742bc98dd3d40c99c90eefa5d23968584be9d8d064bcf99c246"), 8 };
    size_t const out_h13 = 222;
    const uint64_t value13 = 6;

    input_point spend11{ hash_literal("4742b3eac32d35961f9da9d42d495ff1d90aba96944cac3e715047256f7016d1"), 0 };
    size_t const spend_h11 = 115;
    input_point spend13{ hash_literal("3cc768bbaef30587c72c6eba8dbf6aeec4ef24172ae6fe357f2e24c2b0fa44d5"), 0 };
    size_t const spend_h13 = 320;

    const short_hash key2 = base16_literal("9c6b3bdaa612ceab88d49d4431ed58f26e69b90d");
    output_point out21{ hash_literal("80d9e7012b5b171bf78e75b52d2d149580d9e7012b5b171bf78e75b52d2d1495"), 9 };
    size_t const out_h21 = 3982;
    const uint64_t value21 = 65;
    output_point out22{ hash_literal("4742b3eac32d35961f9da9d42d495ff13cc768bbaef30587c72c6eba8dbf6aee"), 0 };
    size_t const out_h22 = 78;
    const uint64_t value22 = 9;

    input_point spend22{ hash_literal("3cc768bbaef30587c72c6eba8dbfffffc4ef24172ae6fe357f2e24c2b0fa44d5"), 0 };
    size_t const spend_h22 = 900;

    const short_hash key3 = base16_literal("3eb84f6a98478e516325b70fecf9903e1ce7528b");
    output_point out31{ hash_literal("d90aba96944cac3e715047256f7016d1d90aba96944cac3e715047256f7016d1"), 0 };
    size_t const out_h31 = 378;
    const uint64_t value31 = 34;

    const short_hash key4 = base16_literal("d60db39ca8ce4caf0f7d2b7d3111535d9543473f");
    ////output_point out42{ hash_literal("aaaaaaaaaaacac3e715047256f7016d1d90aaa96944cac3e715047256f7016d1"), 0};
    size_t const out_h41 = 74448;
    const uint64_t value41 = 990;

    store::create(DIRECTORY "/lookup");
    store::create(DIRECTORY "/rows");
    history_database db(DIRECTORY "/lookup", DIRECTORY "/rows", 1000, 50);
    REQUIRE(db.create());
    db.add_output(key1, out11, out_h11, value11);
    db.add_output(key1, out12, out_h12, value12);
    db.add_output(key1, out13, out_h13, value13);
    db.add_input(key1, spend11, spend_h11, out11);
    db.add_input(key1, spend13, spend_h13, out13);
    db.add_output(key2, out21, out_h21, value21);
    db.add_output(key2, out22, out_h22, value22);

    auto fetch_s1 = [=](const history_compact::list& history) {
        REQUIRE(history.size() == 5);

        auto entry4 = history[4];
        REQUIRE(entry4.point.is_valid());
        REQUIRE(history[4].kind == point_kind::output);
        REQUIRE(history[4].point.hash() == out11.hash());
        REQUIRE(history[4].point.index() == out11.index());
        REQUIRE(history[4].height == out_h11);
        REQUIRE(history[4].value == value11);

        REQUIRE(history[3].kind == point_kind::output);
        REQUIRE(history[3].point.hash() == out12.hash());
        REQUIRE(history[3].point.index() == out12.index());
        REQUIRE(history[3].height == out_h12);
        REQUIRE(history[3].value == value12);

        REQUIRE(history[2].kind == point_kind::output);
        REQUIRE(history[2].point.hash() == out13.hash());
        REQUIRE(history[2].point.index() == out13.index());
        REQUIRE(history[2].height == out_h13);
        REQUIRE(history[2].value == value13);

        REQUIRE(history[1].kind == point_kind::spend);
        REQUIRE(history[1].point.hash() == spend11.hash());
        REQUIRE(history[1].point.index() == spend11.index());
        REQUIRE(history[1].height == spend_h11);
        REQUIRE(history[1].previous_checksum == out11.checksum());

        REQUIRE(history[0].kind == point_kind::spend);
        REQUIRE(history[0].point.hash() == spend13.hash());
        REQUIRE(history[0].point.index() == spend13.index());
        REQUIRE(history[0].height == spend_h13);
        REQUIRE(history[0].previous_checksum == out13.checksum());
    };
    auto res_s1 = db.get(key1, 0, 0);
    fetch_s1(res_s1);
    auto no_spend = [=](const history_compact::list& history) {
        REQUIRE(history.size() == 2);
        REQUIRE(history[0].kind == point_kind::output);
        REQUIRE(history[1].kind == point_kind::output);
    };
    auto res_ns = db.get(key2, 0, 0);
    no_spend(res_ns);
    db.add_input(key2, spend22, spend_h22, out22);
    auto has_spend = [=](const history_compact::list& history) {
        REQUIRE(history.size() == 3);

        REQUIRE(history[0].kind == point_kind::spend);
        REQUIRE(history[0].point.hash() == spend22.hash());
        REQUIRE(history[0].point.index() == spend22.index());
        REQUIRE(history[0].height == spend_h22);
        REQUIRE(history[0].previous_checksum == out22.checksum());

        REQUIRE(history[1].kind == point_kind::output);
        REQUIRE(history[1].point.hash() == out22.hash());
        REQUIRE(history[1].point.index() == out22.index());
        REQUIRE(history[1].height == out_h22);
        REQUIRE(history[1].value == value22);

        REQUIRE(history[2].kind == point_kind::output);
        REQUIRE(history[2].point.hash() == out21.hash());
        REQUIRE(history[2].point.index() == out21.index());
        REQUIRE(history[2].height == out_h21);
        REQUIRE(history[2].value == value21);
    };
    auto res_has_sp = db.get(key2, 0, 0);
    has_spend(res_has_sp);
    db.delete_last_row(key2);
    auto res_no_sp = db.get(key2, 0, 0);
    no_spend(res_no_sp);

    db.add_output(key3, out31, out_h31, value31);
    db.add_output(key4, out31, out_h41, value41);
    auto has_one_row = [=](const history_compact::list& history) {
        REQUIRE(history.size() == 1);
    };
    auto res_1r1 = db.get(key3, 0, 0);
    has_one_row(res_1r1);
    auto res_1r2 = db.get(key4, 0, 0);
    has_one_row(res_1r2);
    auto has_no_rows = [=](const history_compact::list& history) {
        REQUIRE(history.empty());
    };
    db.delete_last_row(key3);
    auto res_1nr1 = db.get(key3, 0, 0);
    has_no_rows(res_1nr1);
    auto res_1nr2 = db.get(key4, 0, 0);
    has_one_row(res_1nr2);

    db.synchronize();
}
#endif // KTH_DB_HISTORY

// End Test Suite

