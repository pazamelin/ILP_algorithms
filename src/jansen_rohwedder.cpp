#include "ilp/jansen_rohwedder.hpp"
#include "ilp/detail/debug.hpp"

#include <stack>
#include <algorithm>
#include <cmath>

namespace ilp
{
    int_t compute_H(const matrix<int_t>& A)
    {
        int_t result = 0;
        for (index_t i = 0; i < A.cols(); ++i)
        {
            result = std::max(result, A.col(i).lpNorm<1>());
        }

        return result;
    }

    int_t compute_K(const ilp_task& ilpTask)
    {
        auto m = static_cast<double>(ilpTask.A.rows());
        auto delta = static_cast<double>(ilpTask.A.lpNorm<Eigen::Infinity>());

        double a = m * std::log2(m);
        double b = m * std::log2(delta + ilpTask.b.lpNorm<Eigen::Infinity>());
        auto result = static_cast<int_t>(a + b);

        return result;
    }

    DynamicTable::DynamicTable(const ilp_task& ilpTask) : ilpTask{ilpTask}
    {
        H = compute_H(ilpTask.A);
        K = compute_K(ilpTask);

        b_cuts.emplace_back(cvector<int_t>::Zero(ilpTask.b.rows(), 1));

        for (int_t i = 1; i < K + 1; i++)
        {
            cvector<double> i_cut = ilpTask.b.cast<double>();
            i_cut *= std::pow(2, i - K);
            cvector<int_t> i_cut_int = i_cut.cast<int_t>();
            if (i_cut_int != b_cuts.back())
            {
                detail::debug_log("     i:", i);
                detail::debug_log("     cut:", i_cut);
                detail::debug_log("     cut_int:", i_cut_int);
                detail::debug_log("");

                b_cuts.emplace_back(std::move(i_cut_int));
            }
        }

        K = static_cast<int_t>(b_cuts.size());

        detail::debug_log("");
        detail::debug_log("K ", K);
        detail::debug_log("H ", H);

        bounds.emplace_back(1.0);
        detail::debug_log("bound ", bounds[0]);
        for (int_t i = 1; i < K + 1; ++i)
        {
            double bound = bounds[i - 1] * 2.0;
            bounds.emplace_back(bound);
            detail::debug_log("bound ", bound);
        }
    }

    bool DynamicTable::entry_condition(const cvector<int_t>& p,
                                       int_t entry_index) const
    {
        detail::debug_log("check entry_condition:");
        detail::debug_log("     point:", p);
        detail::debug_log("bound:", bounds[entry_index]);

        bool result = false;
        if (entry_index != K)
        {
            cvector<int_t> distance = b_cuts[entry_index] - p;
            int_t norm = distance.lpNorm<Eigen::Infinity>();
            result = (norm <= 4 * H);

            detail::debug_log("     b_cut:", b_cuts[entry_index]);
            detail::debug_log("     distance:", distance);
        }
        else
        {
            detail::debug_log("     b:", this->ilpTask.b);
            result = (p == this->ilpTask.b);
            if (result)
            {
                detail::debug_log("***");
                std::cout << "***";
            }
        }

        detail::debug_log("     bound:", 4 * H);
        result ? detail::debug_log("     OK, populating!")
               : detail::debug_log("     NO, out bounds.");

        return result;
    }

    bool DynamicTable::bound_condition(const cvector<int_t>& p,
                                       int_t entry_index) const
    {
        bool result = p.lpNorm<1>() <= bounds[entry_index];
        return result;
    }

    void DynamicTable::populate_entry_from(int_t entry_index,
                                          const cvector<int_t>& from)
    {
        detail::debug_log("populate_entry_from on:", entry_index);

        std::stack<std::pair<Path, cvector<int_t>>> populated;
        populated.push({Path(cvector<int_t>::Zero(ilpTask.A.cols(), 1), 0), from});

        while (!populated.empty())
        {
            auto [current_path, current_point] = std::move(populated.top());
            populated.pop();

            // iterate over A's columns
            for (index_t i = 0; i != ilpTask.A.cols(); i++)
            {
                current_path.x[i] += 1;
                current_path.distance += ilpTask.c(i);

                // check if the point is within the bound (6/5)^i by lp_1 norm
                int_t ei = (entry_index == 0) ? 0 : entry_index - 1;
                if (bound_condition(current_path.x, ei))
                {
                    detail::debug_log("");
                    detail::debug_log("x:", current_path.x);
                    detail::debug_log("x_bound", bounds[ei]);

                    const cvector<int_t>& column = ilpTask.A.col(i);
                    cvector<int_t> possible_new_point = current_point + column;

                    // check if the point is an entry point
                    if (entry_condition(possible_new_point, entry_index + 1))
                    {
                        // try to update path to the entry point
                        this->upd_from(entry_index, from, possible_new_point, current_path);

                        // try to insert the entry point into the next entry of the dtable
                        this->add_entry_point(possible_new_point, entry_index + 1);
                    }

                    // add the current path and point to the queue
                    populated.push({current_path, possible_new_point});
                }

                current_path.x(i) -=1;
                current_path.distance -= ilpTask.c(i);
            }
        }
    }

    void DynamicTable::populate()
    {
        auto n = ilpTask.A.cols();
        auto m = ilpTask.A.rows();

        this->data.resize(K + 1);
        this->add_entry_point(cvector<int_t>::Zero(n, 1), 0);

        // for every entry block
        for (int_t i = 0; i < K; ++i)
        {
            // populate dynamic table from all the entries on the block
            for (const auto& [point, entry] : this->data[i])
            {
                populate_entry_from(i, point);
            }

        }
    }

    ilp_solution jansen_rohwedder(const ilp_task& ilpTask)
    {
        DynamicTable dtable(ilpTask);
        dtable.populate();

        // chose the optimal path using naive approach or max-convolution
        // dtable.naive_merge();
        // dtable.convolution();

        return ilp_solution();
    }

}  // namespace ilp