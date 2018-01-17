#include "HK_subroutines.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream> //ifstream
#include <string>

namespace HK
{

bool HK_comp (BranchingNode const & node1, BranchingNode const & node2)
{
	return (node1.HK_bound() < node2.HK_bound());
}

NodeId tour_check (Min_1_tree const & tree, Edge & edge1, Edge & edge2, std::vector <NodeId> required_edge_ids)
{
	// std::cout<<"TEST HERE!"<<std::endl;
	size_type num_nodes = tree.num_nodes();
	// for(size_type i = 0; i < required_edge_ids.size(); i++)
		// std::cout<<"Required Edge Id "<<required_edge_ids.at(i)<<std::endl;
	//check if min-cost 1-tree tree is 2-regular (i.e. an optimum tour)
	// if tree is not an optimum tour, there exists a vertex with degree > 2
	for(size_type id = 0; id < num_nodes; id++)
	{
		// std::cout << "tree.degree(id)" << tree.degree(id) << "id" << id << std::endl;
		
		if (tree.degree(id) > 2)
		{
			//choose as new required edges edges that are not yet in required edges
			size_type count = 0;
			size_type iter = 0;
			// std::cout << "vor while 1" << std::endl;
			while(count < 1)
			{
				// std::cout<<"incident edge id"<<tree.incident_edges(id).at(iter).get_id()<<std::endl;
				if(find(required_edge_ids.begin(), required_edge_ids.end(), tree.incident_edges(id).at(iter).get_id()) == required_edge_ids.end())
				{
					edge1 = tree.incident_edges(id).at(iter);
					count ++;
				}	
				iter++;
			}
			// std::cout << "vor while 2" << std::endl;
			// std::cout << "Iterator "<< iter << "bei Größe "<<tree.incident_edges(id).size()<<std::endl;
			while(count < 2)
			{
				if(find(required_edge_ids.begin(), required_edge_ids.end(), tree.incident_edges(id).at(iter).get_id()) == required_edge_ids.end())
				{
					
			// std::cout << "in if 2"<< std::endl;
					edge2 = tree.incident_edges(id).at(iter);
					count ++;
				}	
				iter++;
			}
			
			// std::cout << "nach while 2" << std::endl;
			
			return id;
		}
	}
	
	//tree is an optimum tour or not a 1-tree
	return invalid_node_id;
}

void update_queue (BranchingNode & child1, BranchingNode & child2, BranchingNode & child3, std::list<BranchingNode> & candidates, bool check)
{
	// std::list<BranchingNode>::iterator it;
	// it = candidates.begin();
	// it++;
	
	candidates.push_back(child1);
	candidates.push_back(child2);
	if (check == 0) candidates.push_back(child3);	
	candidates.pop_front(); //remove parent 
	candidates.sort(HK_comp);

}

bool check_if_already_incident_edge (NodeId id, BranchingNode & branching_node)
{
	size_type iter;
	size_type size = branching_node.required_edges().size();
	
	for(iter = 0; iter < size; iter++)
	{
		Edge edge = branching_node.required_edges().at(iter);
		if(edge.nodes().first == id or edge.nodes().second == id) return 1;
	}
	return 0;
}

std::vector<NodeId> incident_required_edges(NodeId node, BranchingNode & branching_node)
{
	// std::cout<<"In inc_req_edges, node is "<<node<<", and we have "<<branching_node.required_edges().size()<<" req edges"<<std::endl;
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
		
		// std::cout<<"New incident edge id "<<incident_edge_ids.at(incident_edge_ids.size()-1)<<std::endl;
	}
	
	// std::cout << "incident_edge_ids.size()" << incident_edge_ids.size() << std::endl;
	
	for(unsigned int i=0; i < branching_node.required_edges().size(); i++)
	{
		
		NodeId edge_id = branching_node.required_edges().at(i).get_id();
		//std::cout << "edge_id" << edge_id << std::endl;
		//std::vector<NodeId>::iterator it;
		if(find(incident_edge_ids.begin(), incident_edge_ids.end(), edge_id)!= incident_edge_ids.end())
		{
			incident_required_edge_ids.push_back(edge_id);
			count++;
			
		}
	
		if(count == 2)
		{
			
	// std::cout << " in inc_req_edges incident_required_edge_ids.size()" << incident_required_edge_ids.size() << std::endl;
			return incident_required_edge_ids;
		}
		
	}
	// std::cout << " in inc_req_edges incident_required_edge_ids.size()" << incident_required_edge_ids.size() << std::endl;
		
	return incident_required_edge_ids;	
}

void forbid_all_other_edges(NodeId node, BranchingNode & branching_node, std::vector<NodeId> required_edge_ids, std::vector<Edge> & graph_edges)
{//TODO: Möglichkeit, Edges nach Id hinzuzufügen?
	unsigned int count = 0;
	unsigned int dimension = branching_node.lambda().size();
	//std::cout << "dimension" << dimension << std::endl;
	//std::cout << "graph_edges.size()"  << graph_edges.size() << std::endl;
	//std::cout << "required_edge_ids.size()"  << required_edge_ids.size() << std::endl;
	//TODO: DElETE
	//for (size_type j = 0; j < required_edge_ids.size(); j++) std::cout << "required_edge_ids.at() at" << j << ":"  << required_edge_ids.at(j) << std::endl;
	
	for(unsigned int i = 0; i < graph_edges.size() && count < dimension - 3; i++)
	{
		Edge edge = graph_edges.at(i);
		//std::cout << "edge.get_id()" << edge.get_id() << std::endl;
		
		if((edge.nodes().first == node || edge.nodes().second == node) && (edge.get_id() != required_edge_ids.at(0)) && (edge.get_id() != required_edge_ids.at(1)))
		{
			branching_node.add_forbidden_edge(edge);
			count++;
		}
		//std::cout << "inner count in forbidden edges" << i << std::endl;
		//std::cout << "count variable" << count << std::endl;
	}
	//std::cout << "line 117"  << std::endl;
}

void branch_and_update_queue (BranchingNode & branching_node, std::list <BranchingNode> & candidates, Edge & edge1, Edge & edge2, std::vector <Edge> & graph_edges, BranchingNode & branching_root, NodeId num_nodes, NodeId id)
{
	BranchingNode child1 = BranchingNode(branching_node);
	BranchingNode child2 = BranchingNode(branching_node);
	BranchingNode child3 = BranchingNode(branching_node);
	
	child1.add_forbidden_edge(edge1);
	child2.add_required_edge(edge1);
	child2.add_forbidden_edge(edge2);
	

	NodeId node_1=edge1.nodes().first;
	NodeId node_2=edge1.nodes().second;
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
	
	// std::cout << "HK child1" << std::endl;
	HK_lower_bound_alg(graph_edges, child1, branching_root, num_nodes, 0);
	//std::cout << "HK_bound of child 1:" << child1.HK_bound() << std::endl;
	// std::cout << "HK child2" << std::endl;
	
	HK_lower_bound_alg(graph_edges, child2, branching_root, num_nodes, 0);
	//std::cout << "HK_bound of child 2:" << child2.HK_bound() << std::endl;
	
	// std::cout << "line 151"  << std::endl;
	
	bool check = check_if_already_incident_edge (id, branching_node);
	//std::cout << "line 154"  << std::endl;
	// std::cout << "check" << check << std::endl;
	
	
	if (check == 0)
	{
		// std::cout << "check == 0" << std::endl;
	
		child3.add_required_edge(edge1);
		// std::cout<<"Edge1 hat Endpunkte "<<edge1.nodes().first<<" und "<<edge1.nodes().second<<" , id ist "<<id<<std::endl; 
		child3.add_required_edge(edge2);
		// std::cout<<"child3 nr of required edges "<<child3.required_edges().size()<<std::endl;
		// std::cout<<"child3 req edge hat Endpunkte "<<child3.required_edges().at(0).nodes().first<<" und "<<child3.required_edges().at(0).nodes().second<<"edgeid is"<<child3.required_edges().at(0).get_id() <<" , id ist "<<id<<std::endl;
		
		// std::cout<<"child3 req edge hat Endpunkte "<<child3.required_edges().at(1).nodes().first<<" und "<<child3.required_edges().at(1).nodes().second<<"edgeid is"<<child3.required_edges().at(0).get_id() <<" , id ist "<<id<<std::endl;
		// std::cout<<"Edge2 hat Endpunkte "<<edge2.nodes().first<<" und "<<edge2.nodes().second<<" , id ist "<<id<<std::endl;
		//std::cout << "line 162"  << std::endl;
	
		NodeId node_3 = id;
		NodeId node_4;
		NodeId node_5;
		// std::cout << "line 167"  << std::endl;
	
		if(edge1.nodes().first == id)
		{
			// std::cout << "line 171"  << std::endl;
	
			node_4 = edge1.nodes().second;
		}
		else
		{
			// std::cout << "line 177"  << std::endl;
	
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
		// std::cout << "line 189"  << std::endl;
	
		std::vector<NodeId> incident_required_ids_3=incident_required_edges(node_3, child3);
		std::vector<NodeId> incident_required_ids_4=incident_required_edges(node_4, child3);
		std::vector<NodeId> incident_required_ids_5=incident_required_edges(node_5, child3);
		// std::cout << "incident_required_ids_3.size()" <<incident_required_ids_3.size()<< std::endl; //PROBLEM: size 0 (sollte 2 sein?), wird aber bei forbidden edges in der if-Abfrage drauf zugegriffen
		// std::cout << "line 194"  << std::endl; 
		forbid_all_other_edges(node_3, child3, incident_required_ids_3, graph_edges);
		// std::cout << "line 193"  << std::endl;
	
		if(incident_required_ids_4.size()==2)
		{
			forbid_all_other_edges(node_4, child3, incident_required_ids_4, graph_edges);
		}
		
		if(incident_required_ids_5.size()==2)
		{
			forbid_all_other_edges(node_5, child3, incident_required_ids_5, graph_edges);
		}
		
		// END insert

		// std::cout << "line 207"  << std::endl;
	
		HK_lower_bound_alg(graph_edges, child3, branching_root, num_nodes, 0);
		//std::cout << "HK_bound of child 3" << child3.HK_bound() << std::endl;
	
	}

	update_queue (child1, child2, child3, candidates, check);
}

size_type process_branching_node (size_type upper_bound, std::list <BranchingNode> & candidates, std::vector <Edge> & graph_edges, BranchingNode & branching_root, Min_1_tree & opt_min_1_tree)
{
	
	// BranchingNode branching_node (candidates.front().lambda().size());
	//std::cout<<"New branching_node created "<<std::endl;
	// std::cout<<"Candidates list has size "<<candidates.size()<<std::endl;
	// branching_node = candidates.front();
	//std::cout<<"New branching_node assigned "<<std::endl;
	
	//if branching_node does not represent a solution leading to better cost than upper_bound, discard it
	// std::cout<<"In process_branching_node, HK_bound is "<<candidates.front().HK_bound()<<std::endl;
	
	//sum over all degrees. Discard node, if sum < 2*num_nodes (since with this branching node no 1-tree can be computed).
	double sum = 0;
	size_type num_nodes = candidates.front().HK_min_tree().num_nodes();
	for(size_type i = 0; i < num_nodes; i++)
		sum += candidates.front().HK_min_tree().degree(i);
	
	if (candidates.front().HK_bound() >= upper_bound || sum < 2* num_nodes)
	{
		
			candidates.pop_front();
			//std::cout<<"FALL 0"<<std::endl;
			return upper_bound;
	}
	else
	{
		// std::cout << "in else" << std::endl;
 	
		Edge edge1 = Edge();
		Edge edge2 = Edge();
		
		//pass all required edges as argument for tour_check
		size_type required_edges_size = candidates.front().required_edges().size();
		std::vector <NodeId> required_edge_ids = std::vector<NodeId> (required_edges_size);
		for (size_type iter = 0; iter < required_edges_size; iter++) required_edge_ids.at(iter) = candidates.front().required_edges().at(iter).get_id();
			// std::cout << "vor tour_check" << std::endl;
		NodeId id = tour_check(candidates.front().HK_min_tree(), edge1, edge2, required_edge_ids);
		// std::cout << "nach tour_check" << std::endl;
		//else if it is an optimum tour, update upper_bound
		if (id == invalid_node_id) 
		{
			//TODO DELETE
			double sum = 0;
			for(size_type i = 0; i<candidates.front().lambda().size(); i++)
				for(size_type j = 0; j < candidates.front().HK_min_tree().incident_edges(i).size(); j++)
				{
					sum += candidates.front().HK_min_tree().incident_edges(i).at(j).cost();
					// std::cout << "sum:" << 0.5*sum << std::endl;
					
					// double lambdasum = 0;
					// for (size_type i = 0; i < candidates.front().lambda().size(); i++) 
						// lambdasum += candidates.front().lambda().at(i);
					// std::cout << "lambda" << lambdasum << std::endl;
						
					// std::cout<<"Edge from "<<candidates.front().HK_min_tree().incident_edges(i).at(j).nodes().first<<" zu "<<candidates.front().HK_min_tree().incident_edges(i).at(j).nodes().second<<std::endl;
			
				}					
			// std::cout<<"FALL 1"<<std::endl;		
			//TODO END DELETE
			upper_bound = std::floor(candidates.front().HK_bound());
			opt_min_1_tree = candidates.front().HK_min_tree();
			candidates.pop_front();
			return upper_bound;
		}
		
		//else branch 
		else
		{	
			// std::cout<<"FALL 2"<<std::endl;
			
			branch_and_update_queue (candidates.front(), candidates, edge1, edge2, graph_edges, branching_root, candidates.front().HK_min_tree().num_nodes(), id);
			
			return upper_bound;
		}
	}
	
}
	void branch_and_bound(std::string filename, std::string optimal_tour_filename)
	{
		
		//read in graph from file
		std::vector <Node> graph_coordinates = read_tsplib_input(filename);
		std::vector <Edge> graph_edges = create_complete_graph(graph_coordinates);
		
		size_type upper_bound = invalid_cost;
		size_type num_nodes = graph_coordinates.size();
		std::list<HK::BranchingNode> candidates;
		
		//initialize branching_root
		BranchingNode branching_root = BranchingNode(graph_coordinates.size());
		HK_lower_bound_alg(graph_edges, branching_root, branching_root, num_nodes, 1); 
		candidates.push_back(branching_root);
		//std::cout << "branching_root.HK_bound():" << branching_root.HK_bound() << std::endl;
		
		//branch and bound algorithm	
		int count = 0;
		Min_1_tree opt_min_1_tree = Min_1_tree (num_nodes);
		while (candidates.size() != 0)
		{
			// std::cout << "current upper bound" << upper_bound << std::endl;
			// std::cout<<"First Candidate has upper bound "<<candidates.front().HK_bound()<<std::endl;
			upper_bound = process_branching_node(upper_bound, candidates, graph_edges, branching_root, opt_min_1_tree);
			count ++;
		
			// std::cout<<"Count is "<<count<<std::endl;
			
		}
		// std::cout<<"Count is "<<count<<std::endl;
		std::cout << upper_bound << std::endl;
		// std::cout << opt_min_1_tree.cost()<<std::endl;
		
		std::cout << optimal_tour_filename << std::endl;
	}

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
		else
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
		// std::cout<<"forbidden edge id "<<branching_node.forbidden_edges().at(i).get_id()<<std::endl;
		
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
		//std::cout << "Kruskal - edge, i, j:" << edges.at(i).nodes().first << edges.at(i).nodes().second << std::endl;
		//TODO? Delete line
		if(find(forbidden_edges_ids.begin(), forbidden_edges_ids.end(), current_edge.get_id())!= forbidden_edges_ids.end())
		{
			//std::cout<<"Kruskal - forbidden edge"<<std::endl;
			continue;
		}
			
		NodeId node_1 = current_edge.nodes().first;
		NodeId node_2 = current_edge.nodes().second;
		
		if(std::find(previously_added_edges.begin(), previously_added_edges.end(), current_edge.get_id())!=previously_added_edges.end())
		{
			//std::cout<<"Kruskal - previously added edge"<<std::endl;
			continue;
		}
		if(node_1 != 0 && node_2 != 0 && count != num_nodes - 2)
		{			
			if(find_union(node_1, ancestors) != find_union(node_2, ancestors))
			{
				//std::cout<<"Kruskal - tree edge"<<std::endl;
				result.add_edge(node_1, current_edge);
				result.add_edge(node_2, current_edge);
				count++;
				merge_unions(node_1, node_2, ancestors, tree_sizes);
			}
		}
		else if((node_1 == 0 || node_2 == 0) && num_1_edges < 2)
		{
			//std::cout<<"Kruskal - 1-edge"<<std::endl;
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
	//std::cout<<"NEW MIN-1-TREE"<<std::endl;
	// for(size_type i = 0; i<num_nodes;i++)
	// {
		//for(size_type j = 0; j < result.incident_edges(i).size(); j++)
			//std::cout<<"Edge from "<<result.incident_edges(i).at(j).nodes().first<<" to "<<result.incident_edges(i).at(j).nodes().second<<std::endl;
		
	// }
	
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

} //namespace HK

