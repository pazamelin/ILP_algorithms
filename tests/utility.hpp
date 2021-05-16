#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ilp/ilp_task.hpp>

#include <vector>
#include <iostream>
#include <chrono>
#include <optional>
#include <string>
#include <sstream>
#include <random>
#include <limits>

#define SEED RAND
#define PRINT_SEED 0

namespace ilp::utility
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
        std::uniform_int_distribution<> dist_A(1, max_A);
        std::uniform_int_distribution<> dist_b(3, max_b);
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

    void print(const ilp::ilp_task& task, const ilp::ilp_solution& solution);

    void simplify_task(ilp::ilp_task& task)
    {
/*        ilp::ilp_solution ds;
        std::cout << "before:" << std::endl;
        print(task, ds);*/

        const index_t m = task.A.rows();
        index_t n = task.A.cols();

        // simplify:
        // col - price
        cvector<int> zero_col = cvector<int>::Zero(m, 1);
        std::unordered_map<cvector<int>, std::pair<int, int>, detail::VectorHash<int>> cols_needed;
        for (int ci = 0; ci < n; ++ci)
        {
            const auto& col = task.A.col(ci);
            auto col_price = task.c(0, ci);
            if (col == zero_col)
            {
                continue;
            }

            auto it = cols_needed.find(col);
            if (it != cols_needed.end())
            {
                auto current_price = it->second.second;
                if (col_price > current_price)
                {
                    it->second.first = ci;
                    it->second.second = col_price;
                }
            }
            else
            {
                cols_needed.insert({col, {ci, col_price}});
            }
        }

        // construct new A, c
        n = static_cast<index_t>(cols_needed.size());
        task.n = n;
        task.A = ilp::matrix<int>(m, n);
        task.c = ilp::rvector<int>(1, n);

        int col_new_index = 0;
        for (auto [column, cp] : cols_needed)
        {
            for (int j = 0; j < m; j++)
            {
                task.A(j, col_new_index) = column(j, 0);
            }
            task.c(0, col_new_index) = cp.second;
            ++col_new_index;
        }
        
/*        std::cout << "after:" << std::endl;
        print(task, ds);*/
    }

    void print(const ilp::ilp_task& task, const ilp::ilp_solution& solution)
    {
        std::cout << "A: " << "\n";
        std::cout <<                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            task.A << "\n";
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
    }

    template <typename AnswerX>
    bool check_solution(const ilp::ilp_solution& solution, AnswerX&& x)
    {
        for (int row = 0; row < solution.x.rows(); row++)
        {
            if (solution.x(row, 0) != x[row])
            {
                return false;
            }
        }

        return true;
    }

    class LogDuration {
    public:
        explicit LogDuration(const std::string& msg = "")
                : message(msg + ": ")
                , start(std::chrono::steady_clock::now())
        {
        }

        ~LogDuration() {
            auto finish = std::chrono::steady_clock::now();
            auto dur = finish - start;
            std::ostringstream os;
            os << message
               << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()
               << " ms" << std::endl;
            std::cerr << os.str();
        }
    private:
        std::string message;
        std::chrono::steady_clock::time_point start;
    };

    class AccumulateDuration
    {
    public:
        explicit AccumulateDuration(double& accumulator)
                : accumulator{accumulator}, start{std::chrono::steady_clock::now()}
        { }

        ~AccumulateDuration()
        {
            auto finish = std::chrono::steady_clock::now();
            std::chrono::duration<double> duration = finish - start;
            accumulator += duration.count();
        }

    private:
        double& accumulator;
        std::chrono::steady_clock::time_point start;
    };

} // ilp::utility

#ifndef UNIQ_ID
#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)
#endif

#define LOG_DURATION(message) \
    ilp::utility::LogDuration UNIQ_ID(__LINE__){message};

#define ACCUMULATE_DURATION(accumulator) \
    ilp::utility::AccumulateDuration UNIQ_ID(__LINE__){accumulator};


#endif // UTILITY_HPP