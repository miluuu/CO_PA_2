#include "HK_subroutines.hpp"
#include <vector>

namespace HK
{

bool HK_comp (BranchingNode & node1, BranchingNode & node2)
{
	return (node1.HK_bound() < node2.HK_bound());
}

bool tour_check (Min_1_tree const & tree, Edge & edge1, Edge & edge2)
{
	size_type num_nodes = tree.num_nodes();
	
	//check if min-cost 1-tree tree is 2-regular (i.e. an optimum tour)
	for(size_type id = 0; id < num_nodes; id++)
	{
		if (tree.degree(id) > 2)
		{
			edge1 = (tree.incident_edges(id)).at(0);
			edge2 = (tree.incident_edges(id)).at(1);
			return 0;
		}
	}
	// if tree is not an optimum tour, there exists a vertex with degree > 2
	return 1;
}

void update_queue (BranchingNode const & parent, BranchingNode const & child1, BranchingNode const & child2, BranchingNode const & child3, std::list<BranchingNode> & candidates)
{
	std::list<BranchingNode>::iterator it;
	it = candidates.begin();
	it++;
	
	candidates.insert(it, child1);
	candidates.insert(it, child2);
	candidates.insert(it, child3);	
	candidates.pop_front(); //remove parent 
	candidates.sort(HK_comp);

}

void branch_and_update_queue (BranchingNode const & branching_node, std::list <BranchingNode> & candidates, Edge & edge1, Edge & edge2)
{
	BranchingNode child1 = BranchingNode(branching_node);
	BranchingNode child2 = BranchingNode(branching_node);
	BranchingNode child3 = BranchingNode(branching_node);
	
	child1.add_forbidden_edge(edge1);
	child2.add_required_edge(edge1);
	child2.add_forbidden_edge(edge2);
	child3.add_required_edge(edge1);
	child3.add_required_edge(edge2);

	//TODO: calculate HK_bounds for children
	update_queue (branching_node, child1, child2, child3, candidates);
}

void process_branching_node (BranchingNode const & branching_node, Min_1_tree const min_tree, size_type min_tree_cost, size_type upper_bound, std::list <BranchingNode> & candidates)
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
		
		//else if it is an optimum tour, update upper_bound
		if (tour_check(min_tree, edge1, edge2) == 1) 
		{
			candidates.pop_front();
			upper_bound = min_tree_cost;
			return;
		}
		
		//else branch 
		else
		{	
			branch_and_update_queue (branching_node, candidates, edge1, edge2);
			return;
		}
	}
}


} //namespace HK