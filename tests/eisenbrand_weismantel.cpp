#include <ilp/ilp_task.hpp>
#include <ilp/digraph.hpp>
#include <ilp/eisenbrand_weismantel.hpp>

#include "utility.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("2x2", "[eisenbrand_weismantel]")
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
    auto result = ilp::eisenbrand_weismantel(test_task);

    REQUIRE(result.is_feasible == true);
    REQUIRE(result.is_bounded == true);
    REQUIRE(ilp::utility::check_solution(result, std::vector<int>{10, 10}) == true);
    REQUIRE(result.c_result == 50);
}

TEST_CASE("4x4", "[eisenbrand_weismantel]")
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
    auto result = ilp::eisenbrand_weismantel(test_task);

    REQUIRE(result.is_feasible == false);
    REQUIRE(result.is_bounded == false);
}


TEST_CASE("5x2", "[eisenbrand_weismantel]")
{   // feasible
    ilp::matrix<int> A(2, 5);
    ilp::cvector<int> b(2, 1);
    ilp::rvector<int> c(1, 5);

    A <<    1, 2, 0, 1, 5,
            6, 3, 4, 0, 0;

    b <<    5,
            6;

    c << 1, 3, 0, 0, 0;

    ilp::ilp_task test_task = {A, b, c};
    auto result = ilp::eisenbrand_weismantel(test_task);

    REQUIRE(result.is_feasible == true);
    REQUIRE(result.is_bounded == true);
    REQUIRE(ilp::utility::check_solution(result, std::vector<int>{0, 2, 0, 1, 0}) == true);
}

TEST_CASE("6x3", "[eisenbrand_weismantel]")
{   // feasible
    ilp::matrix<int> A(3, 6);
    ilp::cvector<int> b(3, 1);
    ilp::rvector<int> c(1, 6);

    A << 5, 3, 2, 1, 4, 8,
         4, 2, 6, 9, 7, 1,
         0, 6, 3, 4, 2, 2;

    b << 20,
         20,
         20;

    c << 6, 1, 6, 8, 2, 5;

    ilp::ilp_task test_task = {A, b, c};
    auto result = ilp::eisenbrand_weismantel(test_task);

    REQUIRE(result.is_feasible == false);
    REQUIRE(result.is_bounded == false);
}

TEST_CASE("2x8", "[eisenbrand_weismantel]")
{   // feasible
    ilp::matrix<int> A(2, 8);
    ilp::cvector<int> b(2, 1);
    ilp::rvector<int> c(1, 8);

    A << 3, 6, 2, 10, 4, 2, 2, 6,
         6, 9, 6, 8, 8, 5, 3, 3;

    b << 20,
         27;

    c << 8, 4, 3, 7, 7, 4, 0, 7;

    double time = 0.0;
    {
        ACCUMULATE_DURATION(time);
        ilp::ilp_task test_task = {A, b, c};
        auto result = ilp::eisenbrand_weismantel(test_task);

        REQUIRE(result.is_feasible == true);
        REQUIRE(result.is_bounded == true);
        ilp::utility::print(test_task, result);
    }

    std::cout << time << std::endl;
}

TEST_CASE("random 50x50", "[eisenbrand_weismantel]")
{
    int m = 3;
    int n = 10;
    int max_A = 10;
    int max_b = 50;

    ilp::ilp_task test_task = ilp::utility::generate_task(m, n, max_A, max_b);
    {
        LOG_DURATION("time");
        auto result = ilp::eisenbrand_weismantel(test_task);
        ilp::utility::print(test_task, result);
    }

}