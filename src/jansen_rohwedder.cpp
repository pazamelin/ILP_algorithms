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

        detail::debug_log("     H: ", result);
        return result;
    }

    int_t compute_K(const ilp_task& ilpTask)
    {
        auto m = static_cast<double>(ilpTask.A.rows());
        auto delta = static_cast<double>(ilpTask.A.lpNorm<Eigen::Infinity>());

        double a = m * std::log2(m);
        double b = m * std::log2(delta + ilpTask.b.lpNorm<Eigen::Infinity>());
        auto result = static_cast<int_t>(a + b);

        detail::debug_log("     K: ", result);
        return result;
    }

    DynamicTable::DynamicTable(const ilp_task& ilpTask) : ilpTask{ilpTask}
    {
        detail::debug_log("building JR DynamicTable ...");

        H = compute_H(ilpTask.A);
        K = compute_K(ilpTask);

        b_cuts.reserve(K);
        for (int_t i = 0; i < K; i++)
        {
            cvector<double> i_cut = ilpTask.b.cast<double>();
            i_cut *= std::pow(2, i - K);
            b_cuts.emplace_back(i_cut.cast<int_t>());
        }

        bounds.reserve(K);
        for (int_t i = 0; i < K; ++i)
        {
            bounds.emplace_back((6 / 5) ^ i);
        }
    }

    bool DynamicTable::entry_condition(const cvector<int>& p,
                                       int_t entry_index) const
    {
        cvector<int_t> distance = b_cuts[entry_index] - p;
        int_t norm = distance.lpNorm<Eigen::Infinity>();
        bool result = (norm <= 4 * H);

        return result;
    }

    bool DynamicTable::bound_condition(const cvector<int>& p,
                                       int_t entry_index) const
    {
        bool result = p.lpNorm<1>() <= bounds[entry_index];
        return result;
    }

    void DynamicTable::populate_entry_from(int_t entry_index, PointId from)
    {
        detail::debug_log("populate_entry_from on:", entry_index);
        const Entry& previous_entry = data[entry_index - 1][from];

        std::stack<std::pair<Path, cvector<int_t>>> populated;
        populated.push({Path(), previous_entry.point});

        while (!populated.empty())
        {
            auto [current_path, current_point] = std::move(populated.top());
            populated.pop();

            // iterate over A's columns
            for (index_t i = 0; i != ilpTask.A.cols(); i++)
            {
                current_path.x[i] +=1;
                current_path.distance += ilpTask.c(i);

                // check if the point is within the bound (6/5)^i by lp-1 norm
                if (bound_condition(current_path.x, entry_index - 1))
                {
                    const cvector<int>& column = ilpTask.A.col(i);
                    cvector<int> possible_new_point = current_point + column;

                    // check if the point is an entry point
                    if (entry_condition(possible_new_point, entry_index))
                    {
                        // try update path to the entry point
                        this->upd_to(0, entry_index, current_path);
                    }

                    //  add the current path and point to the queue
                    populated.push({current_path, current_point});
                }
            }
        }
    }

    void DynamicTable::populate()
    {
        auto n = ilpTask.A.cols();
        auto m = ilpTask.A.rows();

        this->data.resize(K);

        auto id = this->get_point_id(cvector<int>::Zero(n, 1));
        this->upd_to(id, 0, Path{cvector<int>::Zero(n, 1), 0});

        // for every entry block
        for (int_t i = 0; i < K; ++i)
        {
            // populate dynamic table from all the entries on the block
            for (int_t j = 0; j < data[i].size(); ++j)
            {
                populate_entry_from(i, j);
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