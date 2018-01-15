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

bool check_if_already_incident_edge (NodeId id, BranchingNode & branching_node)
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

std::vector<NodeId> incident_required_edges(NodeId node, BranchingNode & branching_node)
{
	NodeId count = 0;
	std::vector<NodeId> incident_edge_ids;
	std::vector<NodeId> incident_required_edge_ids;
	unsigned int dimension = branching_node.lambda().size();
	for(unsigned i = 0; i < dimension; i++)
	{
		
		if(i<node)
		{
			incident_edge_ids.push_back(i * dimension + node);
		}
		else if(i>node)
		{
			incident_edge_ids.push_back(node * dimension + i);
		}
		
	}
	for(unsigned int i; i < branching_node.required_edges_size(); i++)
	{
		NodeId edge_id = branching_node.required_edge(i).get_id();
		//std::vector<NodeId>::iterator it;
		bool found = false;
		for(unsigned int j = 0; j < incident_edge_ids.size(); j++)
		{
			
			if(incident_edge_ids.at(j) == edge_id)
				found = true;
		}
		if(found == true)
		{
			incident_required_edge_ids.push_back(edge_id);
			count++;
		}
		if(count == 2)
		{
			return incident_required_edge_ids;
		}
		
	}
	return incident_required_edge_ids;	
}

void forbid_all_other_edges(NodeId node, BranchingNode & branching_node, std::vector<NodeId> required_edge_ids, std::vector<Edge> & graph_edges)
{//TODO: Möglichkeit, Edges nach Id hinzuzufügen?
	unsigned int count = 0;
	unsigned int dimension = branching_node.lambda().size();
	for(unsigned int i = 0; i < graph_edges.size() && count < dimension - 3; i++)
	{
		Edge edge = graph_edges.at(i);
		if((edge.nodes().first == node || edge.nodes().second == node) && edge.get_id() != required_edge_ids.at(0) && edge.get_id() != required_edge_ids.at(1))
		{
			branching_node.add_forbidden_edge(edge);
			count++;
		}
	}
	
}

void branch_and_update_queue (BranchingNode const & branching_node, std::list <BranchingNode> & candidates, Edge & edge1, Edge & edge2, std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_root, NodeId num_nodes, NodeId id)
{
	BranchingNode child1 = BranchingNode(branching_node);
	BranchingNode child2 = BranchingNode(branching_node);
	BranchingNode child3 = BranchingNode(branching_node);
	
	child1.add_forbidden_edge(edge1);
	child2.add_required_edge(edge1);
	child2.add_forbidden_edge(edge2);
	
	//TODO: add case of a node with two incident required edges
	//BEGIN insert
	NodeId node_1=edge2.nodes().first;
	NodeId node_2=edge2.nodes().second;
	std::vector<NodeId> incident_required_ids_1=incident_required_edges(node_1, child2);
	
	std::vector<NodeId> incident_required_ids_2=incident_required_edges(node_2, child2);
	if(incident_required_ids_1.size()==2)
	{
		forbid_all_other_edges(node_1, child2, incident_required_ids_1, graph_edges);
	}
	
	if(incident_required_ids_2.size()==2)
	{
		forbid_all_other_edges(node_2, child2, incident_required_ids_2, graph_edges);
	}
	//END insert
	child1.set_HK_bound(HK_lower_bound_alg(graph_edges, euclidean_costs, child1, branching_root, num_nodes, 0));
	child2.set_HK_bound(HK_lower_bound_alg(graph_edges, euclidean_costs, child2, branching_root, num_nodes, 0));

	bool check = check_if_already_incident_edge (id, branching_node);
	if (check == 0)
	{
		child3.add_required_edge(edge1);
		child3.add_required_edge(edge2);
		//BEGIN insert
		NodeId node_3 = id;
		NodeId node_4;
		NodeId node_5;
		if(edge1.nodes().first == id)
		{
			node_4 = edge1.nodes().second;
		}
		else
		{
			node_4 = edge1.nodes().first;	
		}
		if(edge2.nodes().first == id)
		{
			node_5 = edge2.nodes().second;
		}
		else
		{
			node_5 = edge2.nodes().first;
		}
		std::vector<NodeId> incident_required_ids_3=incident_required_edges(node_3, child3);
		std::vector<NodeId> incident_required_ids_4=incident_required_edges(node_4, child3);
		std::vector<NodeId> incident_required_ids_5=incident_required_edges(node_5, child3);
		forbid_all_other_edges(node_3, child3, incident_required_ids_3, graph_edges);
		if(incident_required_ids_4.size()==2)
		{
			forbid_all_other_edges(node_4, child3, incident_required_ids_4, graph_edges);
		}
		
		if(incident_required_ids_5.size()==2)
		{
			forbid_all_other_edges(node_5, child3, incident_required_ids_5, graph_edges);
		}
		
		// END insert
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