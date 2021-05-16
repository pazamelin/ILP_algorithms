#include <ilp/ilp_task.hpp>
#include <ilp/eisenbrand_weismantel.hpp>
#include <ilp/jansen_rohwedder.hpp>

#include "utility.hpp"

#include <charconv>
#include <fstream>

// Magic tricks to have CPLEX behave well:
#ifndef IL_STD
#define IL_STD
#endif

#include <cstring>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN
// End magic tricks

std::pair<IloModel, IloNumVarArray> to_CPLEX_model(const ilp::ilp_task& ilpTask, IloEnv env)
{
    const auto m = static_cast<int>(ilpTask.A.rows());
    const auto n = static_cast<int>(ilpTask.A.cols());

    // construct a modeling object named model, within an existing environment
    IloModel model(env);

    // construct variables array
    IloNumVarArray x(env, n, 0, IloInfinity, ILOINT);

    // construct objective function
    IloObjective obj = IloMaximize(env);
    for (int i = 0; i < n; i++)
    {
        auto value = ilpTask.c(0, i);
        obj.setLinearCoef(x[i], value);
    }
    model.add(obj);

    // construct constraint expressions
    for (int i = 0; i < m; i++)
    {
        IloExpr exp_i(env);
        for (int j = 0; j < n; j++)
        {
            exp_i += x[j] * ilpTask.A(i, j);
        }

        model.add(exp_i == ilpTask.b(i, 0));
    }

    return {model, x};
}

ilp::ilp_solution solve_CPLEX_model(const ilp::ilp_task& task,
                                    IloEnv env,
                                    IloModel& model,
                                    IloNumVarArray& x)
{
    // result solution:
    ilp::ilp_solution result;

    // const auto m = task.A.rows();
    const auto n = task.A.cols();

    // create the object for solving the problem
    IloCplex cplex(model);
    cplex.setOut(env.getNullStream());

    try
    {
        // solve the problem
        bool has_solved = cplex.solve();

        if (!has_solved)
        {
            return result; // is_feasible = false, is_bounded = false
        }

        auto status = cplex.getStatus();

        if (status == IloAlgorithm::Optimal)
        {
            result.is_feasible = true;
            result.is_bounded = (status != IloAlgorithm::Unbounded);

            // save x from the CPLEX solver's solution to result variable
            result.x = ilp::cvector<int>::Zero(n, 1);
            for (int i = 0; i < n; i++)
            {
                result.x(i, 0) = static_cast<int>(cplex.getValue(x[i]));
            }

            result.c_result = static_cast<int>(cplex.getObjValue());
        }
    }
    catch (IloException& e)
    {
        cerr << "CPLEX Concert Exception: " << e << endl;
    }
    catch (...)
    {
        cerr << "Unknown Exception" << endl;
    }

    return result;
}

bool is_solution_valid(const ilp::ilp_task& task, const ilp::ilp_solution& solution)
{
    if (!(solution.is_feasible && solution.is_bounded))
    {
        return false;
    }
    // assert Ax == b
    if (task.A * solution.x != task.b)
    {
        return false;
    }

    return true;
}

void profile(int m,
             int n_min, int n_max, int n_step,
             int max_A, int max_b,
             int iterations_per_size,
             const std::string& filename)
{

    std::ofstream fs{filename, std::ofstream::out | std::ios::trunc };
    fs << "m,n,max_A,max_b,EW,CPLEX\n";

    for (int n = n_min; n < n_max; n += n_step)
    {
        double total_time_EW = 0;
        double total_time_CPLEX = 0;
        int total_n = 0;

        for (int i = 0; i < iterations_per_size; ++i)
        {
            // generate random task with the given parameters
            ilp::ilp_task task = ilp::utility::generate_task(m, n, max_A, max_b);
            // simplify
            ilp::utility::simplify_task(task);
            total_n += task.A.cols();

            // solve using EW
            double time_EW = 0;
            ilp::ilp_solution result_EW;
            {
                ACCUMULATE_DURATION(time_EW);
                result_EW = ilp::eisenbrand_weismantel(task);
            }
            result_EW.time = time_EW;
            total_time_EW += time_EW;

            // solve using CPLEX
            ilp::ilp_solution result_CPLEX;
            {
                // create CPLEX environment handler (PIMPL) object
                IloEnv env;

                // convert the task to CPLEX model and solve it
                auto[model, x] = to_CPLEX_model(task, env);
                double time_CPLEX = 0;
                {
                    ACCUMULATE_DURATION(time_CPLEX);
                    result_CPLEX = solve_CPLEX_model(task, env, model, x);
                }
                result_CPLEX.time = time_CPLEX;
                total_time_CPLEX += time_CPLEX;

                // explicitly destroy the environment implementation object
                env.end();

                // compare the solutions:
                // both are valid/invalid
                // bool is_valid_EW = is_solution_valid(task, result_EW);
                // bool is_valid_CPLEX = is_solution_valid(task, result_CPLEX);

                // REQUIRE(is_valid_EW == is_valid_CPLEX);
                // same objective function value
                assert(std::abs(result_EW.c_result - result_CPLEX.c_result) <= 1);
            }
        }

        double average_time_EW = total_time_EW / iterations_per_size;
        double average_time_CPLEX = total_time_CPLEX / iterations_per_size;
        int average_n = total_n / iterations_per_size;
        fs << m << "," << average_n << "," << max_A << "," << max_b << ","
           << average_time_EW << ","
           << average_time_CPLEX << "\n";
    }
}

int main()
{
    profile(2, 10, 5000, 100, 10, 20, 10, "profile_1.csv");
    return 0;
}