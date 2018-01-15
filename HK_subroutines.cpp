#include "HK_subroutines.hpp"
#include <vector>
#include <cmath>

namespace HK
{

bool HK_comp (BranchingNode const & node1, BranchingNode const & node2)
{
	return (node1.HK_bound() < node2.HK_bound());
}

NodeId tour_check (Min_1_tree const & tree, Edge & edge1, Edge & edge2)
{
	size_type num_nodes = tree.num_nodes();
	
	//check if min-cost 1-tree tree is 2-regular (i.e. an optimum tour)
	// if tree is not an optimum tour, there exists a vertex with degree > 2
	for(size_type id = 0; id < num_nodes; id++)
	{
		if (tree.degree(id) > 2)
		{
			edge1 = (tree.incident_edges(id)).at(0);
			edge2 = (tree.incident_edges(id)).at(1);
			return id;
		}
	}
	//tree is an optimum tour
	return invalid_node_id;
}

void update_queue (BranchingNode const & parent, BranchingNode const & child1, BranchingNode const & child2, BranchingNode const & child3, std::list<BranchingNode> & candidates, bool check)
{
	std::list<BranchingNode>::iterator it;
	it = candidates.begin();
	it++;
	
	candidates.insert(it, child1);
	candidates.insert(it, child2);
	if (check == 0) candidates.insert(it, child3);	
	candidates.pop_front(); //remove parent 
	candidates.sort(HK_comp);

}

bool check_if_already_incident_edge (NodeId id, BranchingNode const & branching_node)
{
	size_type iter;
	size_type size = branching_node.required_edges_size();
	
	for(iter = 0; iter < size; iter++)
	{
		Edge edge = branching_node.required_edge(iter);
		if(edge.nodes().first == id or edge.nodes().second == id) return 1;
	}
	return 0;
}

void branch_and_update_queue (BranchingNode const & branching_node, std::list <BranchingNode> & candidates, Edge & edge1, Edge & edge2, std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_root, NodeId num_nodes, NodeId id)
{
	BranchingNode child1 = BranchingNode(branching_node);
	BranchingNode child2 = BranchingNode(branching_node);
	BranchingNode child3 = BranchingNode(branching_node);
	
	child1.add_forbidden_edge(edge1);
	child2.add_required_edge(edge1);
	child2.add_forbidden_edge(edge2);
	
	child1.set_HK_bound(HK_lower_bound_alg(graph_edges, euclidean_costs, child1, branching_root, num_nodes, 0));
	child2.set_HK_bound(HK_lower_bound_alg(graph_edges, euclidean_costs, child2, branching_root, num_nodes, 0));

	bool check = check_if_already_incident_edge (id, branching_node);
	if (check == 0)
	{
		child3.add_required_edge(edge1);
		child3.add_required_edge(edge2);
		child3.set_HK_bound(HK_lower_bound_alg(graph_edges, euclidean_costs, child3, branching_root, num_nodes, 0));
	}

	update_queue (branching_node, child1, child2, child3, candidates, check);
}

void process_branching_node (BranchingNode const & branching_node, Min_1_tree const min_tree, double min_tree_cost, size_type upper_bound, std::list <BranchingNode> & candidates, std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_root)
{
	//if branching_node does not represent a solution leading to better cost than upper_bound, discard it
	if (min_tree_cost >= upper_bound)
	{
			candidates.pop_front();
			return;
	}
	else
	{
		Edge edge1 = Edge();
		Edge edge2 = Edge();
		NodeId id = tour_check(min_tree, edge1, edge2);
		
		//else if it is an optimum tour, update upper_bound
		if (id == invalid_node_id) 
		{
			candidates.pop_front();
			upper_bound = std::floor(min_tree_cost);
			return;
		}
		
		//else branch 
		else
		{	
			branch_and_update_queue (branching_node, candidates, edge1, edge2, graph_edges, euclidean_costs, branching_root, min_tree.num_nodes(), id);
			return;
		}
	}
}


} //namespace HK