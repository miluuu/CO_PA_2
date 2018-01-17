#include "my_subroutines.hpp"
#include "HK_classes.hpp"
#include <fstream> //ifstream
#include <string>
#include <vector>
#include <algorithm> //stable_sort



namespace HK
{

std::vector<Node> read_tsplib_input	(std::string filename)
{
	std::string next_line;
	std::ifstream tsp_file (filename);
	std::vector<Node> input_nodes;
	
	if(tsp_file)
	{
		
		getline(tsp_file, next_line);
		
		unsigned int dimension;
		bool nodesection = false;
		while(next_line!="EOF")
		{
			//normalize input
			unsigned int current_position = next_line.find(":");
			
			while(current_position <= next_line.size())
			{

				next_line = next_line.erase(current_position, 1);
				current_position = next_line.find(":");
			}
			
			current_position = next_line.find("  ");
			while(current_position <= next_line.size())
			{
				next_line = next_line.replace(current_position, 2, " ");
				current_position = next_line.find("  ");
			}
			
			//set dimension
			if(next_line.substr(0, 9) == "DIMENSION")
			{
				dimension = std::stoul(next_line.substr(10));				
			}
			
			//create nodes
			if(nodesection == true)
			{
				unsigned int gap1 = next_line.find(" ", 1);
				unsigned int gap2 = next_line.find(" ", gap1+1);
				double coord_1 = stod(next_line.substr(gap1+1,gap2-gap1));
				double coord_2 = stod(next_line.substr(gap2+1));
				
				Node current_node (coord_1, coord_2);
				input_nodes.push_back(current_node);	
							}
			
			
			//found start of node_coord_section 
			if(next_line.substr(0,18) == "NODE_COORD_SECTION")
			{
				nodesection = true;
			}
			
			getline(tsp_file, next_line);
		}
		
		if(input_nodes.size() != dimension)
		{
			std::cout<<"The number of nodes created differs from the given dimension"<<std::endl;
			throw std::runtime_error("wrong number of nodes");
		}
		return input_nodes;
	}
	
	
	
	else
	{
		std::cout <<"No such file found!"<<std::endl;
		throw std::runtime_error("The file does not exist!");
	}
	
	
	
	
}
	
std::vector<Edge> create_complete_graph(std::vector<Node> & nodes)
{
	std::vector<Edge> edges;
	unsigned int dimension = nodes.size();
	for(unsigned int i = 0; i < nodes.size();i++)
	{
		for(unsigned int j = i+1; j < nodes.size(); j++)
		{
			edges.push_back(Edge(i, j, nodes, i*dimension + j));
			//std::cout << "edge, i, j:" << i << j << "cost:" << edges.at(edges.size() -1).cost()<< std::endl;
		}
		
	}
	
	return edges;
}


bool compare_edge_costs (Edge edge_1, Edge edge_2)
{
	if(edge_1.cost() <= edge_2.cost())
	{
		return true;
	}
	else
	{
		return false;
	}
}
	
Min_1_tree min_1_tree (std::vector<Edge> & edges, size_type num_nodes, BranchingNode & branching_node)
{
	std::vector<NodeId> ancestors;
	for(unsigned int i = 0; i < num_nodes; i++)
	{
		ancestors.push_back(i);
	}
	std::vector<NodeId> tree_sizes (num_nodes, 0);
	
	
	Min_1_tree result(num_nodes);
	std::vector<NodeId> previously_added_edges;
	unsigned int count = 0;
	unsigned int num_1_edges = 0;
	
	//add required edges
	for(unsigned int i = 0; i < branching_node.required_edges().size(); i++)
	{
		Edge current_edge = branching_node.required_edges().at(i);
		NodeId node_1 = current_edge.nodes().first;
		NodeId node_2 = current_edge.nodes().second;
		
		if(node_1 != 0 && node_2 != 0)
		{			
			result.add_edge(node_1, current_edge);
			result.add_edge(node_2, current_edge);
			count++;
			merge_unions(node_1, node_2, ancestors, tree_sizes);
		}
		else if(num_1_edges < 2)
		{
			result.add_edge(node_1, current_edge);
			result.add_edge(node_2, current_edge);
			num_1_edges++;
		}
		//mark edge as already added
		previously_added_edges.push_back(current_edge.get_id());
		
	}
	//TODO Delete?
	std::vector<NodeId> forbidden_edges_ids = std::vector<NodeId> (branching_node.forbidden_edges().size());
	for(size_type i = 0; i< branching_node.forbidden_edges().size(); i++)
	{
		forbidden_edges_ids.at(i) = branching_node.forbidden_edges().at(i).get_id();
		
		
	}
	//TODO end delete
	
	//sort edges by costs
	std::stable_sort(edges.begin(), edges.end(), compare_edge_costs);
	// for(unsigned int i = 0; i < edges.size(); i++)
		// std::cout << "sort - edge, i, j:" << edges.at(i).nodes().first << edges.at(i).nodes().second << "cost:" << edges.at(i).cost()<< std::endl;
	//create MST by Kruskal
	std::pair<Edge, Edge> edges_at_0;
	for(unsigned int i = 0; i < edges.size() && (count != num_nodes - 2 || num_1_edges != 2); i++)
	{
		
		
		Edge current_edge = edges.at(i);
		//TODO? Delete line
		if(find(forbidden_edges_ids.begin(), forbidden_edges_ids.end(), current_edge.get_id())!= forbidden_edges_ids.end())
			continue;
		NodeId node_1 = current_edge.nodes().first;
		NodeId node_2 = current_edge.nodes().second;
		//std::cout << "Kruskal - edge, i, j:" << edges.at(i).nodes().first << edges.at(i).nodes().second << std::endl;
		if(std::find(previously_added_edges.begin(), previously_added_edges.end(), current_edge.get_id())!=previously_added_edges.end())
		{
			//std::cout<<"Kruskal Fall 1"<<std::endl;
			continue;
		}
		if(node_1 != 0 && node_2 != 0 && count != num_nodes - 2)
		{			
			if(find_union(node_1, ancestors) != find_union(node_2, ancestors))
			{
				//std::cout<<"Kruskal Fall 2"<<std::endl;
				result.add_edge(node_1, current_edge);
				result.add_edge(node_2, current_edge);
				count++;
				merge_unions(node_1, node_2, ancestors, tree_sizes);
			}
		}
		else if((node_1 == 0 || node_2 == 0) && num_1_edges < 2)
		{
			//std::cout<<"Kruskal Fall 3"<<std::endl;
			if(num_1_edges == 0)
				edges_at_0.first = current_edge;
			else
				edges_at_0.second = current_edge;
			num_1_edges++;
		}
	}
	NodeId first_endpoint;
	NodeId second_endpoint;
	if(edges_at_0.first.nodes().first == 0)
		first_endpoint = edges_at_0.first.nodes().second;
	else
		first_endpoint = edges_at_0.first.nodes().first;
	if(edges_at_0.second.nodes().first == 0)
		second_endpoint = edges_at_0.second.nodes().second;
	else
		second_endpoint = edges_at_0.second.nodes().first;
	result.add_edge(0, edges_at_0.first);
	result.add_edge(first_endpoint, edges_at_0.first);
	result.add_edge(0, edges_at_0.second);
	result.add_edge(second_endpoint, edges_at_0.second);
	
	return result;
}

NodeId find_union(NodeId node, std::vector<NodeId> & ancestors)
{
	NodeId candidate = node;
	while(ancestors.at(candidate) != candidate)
	{
		candidate = ancestors.at(candidate);
		
	}
	ancestors.at(node)=candidate;
	return candidate;
	
}

void merge_unions(NodeId node_1, NodeId node_2, std::vector<NodeId> & ancestors, std::vector<NodeId> & tree_sizes)
{
	NodeId root_1 = find_union(node_1, ancestors);
	NodeId root_2 = find_union(node_2, ancestors);
	
	if(tree_sizes.at(root_1) >= tree_sizes.at(root_2))
	{
		ancestors.at(root_2) = root_1;
		ancestors.at(node_2) = root_1;
		if(tree_sizes.at(root_1) == tree_sizes.at(root_2))
		{
			tree_sizes.at(root_1)++;
		}
		
	}
	else
	{
		ancestors.at(root_1) = root_2;
		ancestors.at(node_1) = root_2;
		
	}
		
		
}



}