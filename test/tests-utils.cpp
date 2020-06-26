//
// Created by Amogh on 6/25/20.
//

#include <catch2/catch.hpp>

#include "utils.hpp"

std::vector<std::string> v;
std::vector<std::string> actual;
std::string test;

TEST_CASE("Basic string split testing", "[split_string]")
{

    test = "this is my string";
    v = util::split_string(test);

    actual = {
            "this", "is", "my", "string"
    };

    REQUIRE(v.size() == 4);
    REQUIRE(v == actual);
}

TEST_CASE("Edge case: empty string", "[split_string]")
{
    test = "";
    v = util::split_string(test);
    actual = {};
    REQUIRE(v.size() == actual.size());
    REQUIRE(v == actual);
}

TEST_CASE("Case 2: no space delimiters", "[split_string]")
{
    test = "abcde";
    v = util::split_string(test);
    actual = {"abcde"};
    REQUIRE(v.size() == actual.size());
    REQUIRE(v == actual);
}
