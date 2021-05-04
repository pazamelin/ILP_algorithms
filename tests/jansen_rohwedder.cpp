#include <ilp/ilp_task.hpp>
#include <ilp/digraph.hpp>
#include <ilp/jansen_rohwedder.hpp>

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

TEST_CASE("2x2", "[jansen_rohwedder]")
{   // feasible
    ilp::matrix<int> A(2, 2);
    ilp::cvector<int> b(2, 1);
    ilp::rvector<int> c(1, 2);

    A << 2, 1,
         1, 2;

    b << 30,
         30;

    c << 2, 3;

    ilp::ilp_task test_task = {A, b, c};
    ilp::jansen_rohwedder(test_task);
    auto result = ilp::jansen_rohwedder(test_task);

    /*
    REQUIRE(result.is_feasible == true);
    REQUIRE(result.is_bounded == true);
    REQUIRE(result.x == std::vector<int>{10, 10});
    REQUIRE(result.c_result == 50);
     */
}

TEST_CASE("4x4", "[jansen_rohwedder]")
{   // infeasible
    ilp::matrix<int> A(4, 4);
    ilp::cvector<int> b(4, 1);
    ilp::rvector<int> c(1, 4);

    A << 6, 1, 4, 8,
            5, 3, 2, 2,
            4, 2, 0, 3,
            8, 6, 8, 1;

    b << 40,
            40,
            40,
            40;

    c << 2, 3, 4, 5;

    ilp::ilp_task test_task = {A, b, c};
    auto result = ilp::jansen_rohwedder(test_task);

    /*
    REQUIRE(result.is_feasible == false);
    REQUIRE(result.is_bounded == false);
     */
}