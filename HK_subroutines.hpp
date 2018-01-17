#ifndef HK_SUBROUTINES_HPP
#define HK_SUBROUTINES_HPP
#include "HK_classes.hpp"
#include "HK_lower_bound_alg.hpp"
#include <list>

namespace HK
{

//comparison operator for BranchingNodes wrt their HK_bound (in order to be able to sort the BranchingNodes)
bool HK_comp (BranchingNode const & node1, BranchingNode const & node2);

		
//Returns 1 if Min_1_tree is already a tour, otherwise returns 0 and writes two incident edges of a vertex with degree greater than 2 in edge1 and edge2.
NodeId tour_check (Min_1_tree const & tree, Edge & edge1, Edge & edge2, std::vector <NodeId> required_edge_ids);

//updates priority queue in the branching case (the processed parent branching node is removed, the new nodes are inserted into the sorted list). 
//(If check_if_already_incident_edge yields positive answer, the parent branching node is only replaced by two new nodes, otherwise by three.)
void update_queue (BranchingNode & child1, BranchingNode & child2, BranchingNode & child3, std::list<BranchingNode> & candidates, bool check);

//Subroutine for branch_and_update_queue (check if the branching child node with two new edges added to required_edges needs to be omitted because their shared adjacent vertex already has a required edge)
bool check_if_already_incident_edge (NodeId id, BranchingNode & branching_node);

//Branches in the way described in the exercise (subroutine for process_branching_node).
void branch_and_update_queue (BranchingNode & branching_node, std::list <BranchingNode> & candidates, Edge & edge1, Edge & edge2, std::vector <Edge> & graph_edges, BranchingNode & branching_root, NodeId num_nodes, NodeId id);

/*(The candidate list Q is implemented in the list "candidates", sorted by their HK-lower-bound in ascending order. We always process the first branching node of the list.) 
 Processes branching_node: 
 Checks if respective min-cost-1-tree has lower cost than U: 
 N) If no, discard it. 
 Y) If yes, check if it is already an optimum tour: 
	Y.Y) If yes, then update the upper bound. 
	Y.N) If no, branch. 
 In all cases: Remove branching_node from the candidate list and return the current upper bound
*/
size_type process_branching_node (size_type upper_bound, std::list <BranchingNode> & candidates, std::vector <Edge> & graph_edges, BranchingNode & branching_root, Min_1_tree & opt_min_1_tree);
	
//final algorithm: computes the cost of the optimal TSP tour in the graph stored in file
void branch_and_bound(std::string filename, std::string optimal_tour_filename);
	
	
	
std::vector<NodeId> incident_required_edges(NodeId node, BranchingNode & branching_node);

void forbid_all_other_edges(NodeId node, BranchingNode & branching_node, std::vector<NodeId> required_edge_ids, std::vector<Edge> & graph_edges);	
//Creates a vector of node coordinates from the file @c filename
std::vector<Node> read_tsplib_input	(std::string filename);
//Creates a vector of all edges in a complete graph with the costs given by the coordinates in @c nodes
std::vector<Edge> create_complete_graph(std::vector<Node> & nodes);
//Calculates a minimum 1-tree from the edges in @c edges on a graph with @c num_nodes nodes, wrt to the required and forbidden edges in the given vectors
Min_1_tree min_1_tree (std::vector<Edge> & edges, size_type num_nodes, BranchingNode & branching_node);
//The Find-step of union-find, finds the representative ( or root) of the class of @c node, with ancestors given in @c ancestors
NodeId find_union(NodeId node, std::vector<NodeId> & ancestors);
//The Merge-Step in union-find, merges the two unions of @c node_1 and @c node_2, ancestors given by @c ancestors, the logarithmic sizes of the unions given by @c tree_sizes (at any given time, the entries in tree_sizes only have a sense for nodes that are actual roots)
void merge_unions(NodeId node_1, NodeId node_2, std::vector<NodeId> & ancestors, std::vector<NodeId> & tree_sizes);
//returns true, if edge_1 has cost at most as high as edge 2
bool compare_edge_costs (Edge edge_1, Edge edge_2);

} //namespace HK

#endif /* HK_SUBROUTINES_HPP */
