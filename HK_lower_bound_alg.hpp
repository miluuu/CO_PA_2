#ifndef HK_LOWER_BOUND_ALG_HPP
#define HK_LOWER_BOUND_ALG_HPP

#include "HK_subroutines.hpp"

namespace HK 
{

//subroutine for the Held-Karp-lower-bound-algorithm: updates the edge-costs in each iteration. 
void set_new_cost (std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_node, size_type num_nodes);

//subroutine for the Held-Karp-lower-bound-algorithm: update lambda (according to whether iter >= 2 (dampened case)or not)
void update_lambda(Min_1_tree & current_tree, std::vector<size_type> & previous_degrees, std::vector<double> & lambda, size_type iter, size_type num_nodes, double stepsize);

//performs the Held-Karp-lower-bound-algorithm (two variants; depending on whether given branching node is root or not). Writes HK(K_n, c, R F) in _HK_bound, writes tree with maximal cost in _HK_min_tree.
void HK_lower_bound_alg(std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_node, BranchingNode & branching_root, size_type num_nodes, bool is_root);


} //namespace HK


#endif /* HK_LOWER_BOUND_ALG_HPP */