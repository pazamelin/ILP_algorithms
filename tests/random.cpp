#include <ilp/ilp_task.hpp>
#include <ilp/eisenbrand_weismantel.hpp>
#include <ilp/jansen_rohwedder.hpp>

#include "utility.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("level_0", "[random]")
{
    int m = 2;
    int n = 2;
    int max_A = 5;
    int max_b = 20;

    for (int i = 0; i < 1000; ++i)
    {
        ilp::ilp_task task = ilp::utility::generate_task(m, n, max_A, max_b);
        ilp::eisenbrand_weismantel(task);
        ilp::ilp_solution solution = ilp::eisenbrand_weismantel(task);
        std::cout << "i: " << i << "\n";
        ilp::utility::print(task, solution);
    }
}

TEST_CASE("level_1", "[random]")
{
    int m = 4;
    int n = 4;
    int max_A = 2;
    int max_b = 10;

    for (int i = 0; i < 100; ++i)
    {
        ilp::ilp_task task = ilp::utility::generate_task(m, n, max_A, max_b);
        ilp::eisenbrand_weismantel(task);
        ilp::ilp_solution solution = ilp::eisenbrand_weismantel(task);
        std::cout << "i: " << i << "\n";
        ilp::utility::print(task, solution);
    }
}