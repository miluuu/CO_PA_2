#ifndef MY_SUBROUTINES_HPP
#define MY_SUBROUTINES_HPP
#include "HK_classes.hpp"
namespace HK
{
//Creates a vector of node coordinates from the file @c filename
std::vector<Node> read_tsplib_input	(std::string filename);
//Creates a vector of all edges in a complete graph with the costs given by the coordinates in @c nodes
std::vector<Edge> create_complete_graph(std::vector<Node> & nodes);
//Calculates a minimum 1-tree from the edges in @c edges on a graph with @c num_nodes nodes, wrt to the required and forbidden edges in the given vectors
Min_1_tree min_1_tree (std::vector<Edge> & edges, size_type num_nodes, std::vector<Edge> & required_edges, std::vector<Edge> & forbidden_edges);
//The Find-step of union-find, finds the representative ( or root) of the class of @c node, with ancestors given in @c ancestors
NodeId find_union(NodeId node, std::vector<NodeId> & ancestors);
//The Merge-Step in union-find, merges the two unions of @c node_1 and @c node_2, ancestors given by @c ancestors, the logarithmic sizes of the unions given by @c tree_sizes (at any given time, the entries in tree_sizes only have a sense for nodes that are actual roots)
void merge_unions(NodeId node_1, NodeId node_2, std::vector<NodeId> & ancestors, std::vector<NodeId> & tree_sizes);
//returns true, if edge_1 has cost at most as high as edge 2
bool compare_edge_costs (Edge edge_1, Edge edge_2);

}
#endif