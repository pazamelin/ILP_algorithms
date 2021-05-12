#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <ilp/ilp_task.hpp>

#include <vector>
#include <random>
#include <limits>

#define SEED RAND
#define PRINT_SEED 0

namespace ilp::debug
{
    auto get_seed()
    {
#if SEED == RAND
        std::random_device rd;
        const auto seed = rd();
#else
        const auto seed = SEED;
#endif

#if PRINT_SEED == 1
        std::cout << "SEED: " << seed << "\n";
#endif
        return seed;
    }

    ilp::ilp_task generate_task(int m, int n, int max_A, int max_b)
    {
        ilp::matrix<int> A(m, n);
        ilp::cvector<int> b(m, 1);
        ilp::rvector<int> c(1, n);

        std::mt19937 gen(get_seed());
        std::uniform_int_distribution<> dist_A(0, max_A);
        std::uniform_int_distribution<> dist_b(0, max_b);
        std::uniform_int_distribution<> dist_c(0, 10);

        for (int row = 0; row < m; ++row)
        {
            for (int col = 0; col < n; ++col)
            {
                A(row, col) = dist_A(gen);
            }
        }

        for (int row = 0; row < m; ++row)
        {
            b(row, 0) = dist_b(gen);
        }

        for (int col = 0; col < n; ++col)
        {
            c(0, col) = dist_c(gen);
        }

        return {A, b, c};
    }
}

void print(const ilp::ilp_task& task, const ilp::ilp_solution& solution)
{
    std::cout << "A: " << "\n";
    std::cout << task.A << "\n";
    std::cout << "b: " << task.b << "\n";
    std::cout << "c: " << task.c << "\n";

    std::cout << "feasible: " << solution.is_feasible
              << ", bounded: " << solution.is_bounded << "\n";

    bool has_solution = solution.is_feasible && solution.is_bounded;
    if (has_solution)
    {
        std::cout << "x: " << solution.x << "\n";
        std::cout << "SOLVED" << "\n";
    }
    std::cout << "\n";
}

#endif // GENERATOR_HPP