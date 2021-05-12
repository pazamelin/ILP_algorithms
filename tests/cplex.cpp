#include <ilp/ilp_task.hpp>
#include <ilp/eisenbrand_weismantel.hpp>
#include <ilp/jansen_rohwedder.hpp>

#include "utility.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// Magic tricks to have CPLEX behave well:
#ifndef IL_STD
#define IL_STD
#endif
#include <cstring>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
// End magic tricks


/*
TEST_CASE("level_0", "[CPLEX_TEST]")
{
    // create CPLEX environment handler (PIMPL) object
    IloEnv env;

    // 1. Create optimization problem:
    // 1.1 construct a modeling object within an existing environment named env
    IloModel model(env);

    // 1.2 construct modeling variables
    IloNumVar x1(env, 0, 40, ILOINT);
    // 1.3 define objective function
    IloObjective obj = IloMinimize(env, x1 + 2*x2 + 3*x3);

    // 1.4 add the objects to the model
    model.add(IloMinimize(env, x1 + 2*x2 + 3*x3));

    // 2.1 create the object for solving the problem
    IloCplex cplex(model);

    // 2.2 export model to file (useful for debugging!)
    cplex.exportModel("model.lp");

    // 2.3 call to a solver
    cplex.solve ();

    // explicitly destroy the implementation object
    env.end();
}
 */