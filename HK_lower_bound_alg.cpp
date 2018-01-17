#include "HK_classes.hpp"
#include "HK_lower_bound_alg.hpp"
#include "my_subroutines.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace HK
{

void set_new_cost (std::vector <Edge> & graph_edges, BranchingNode & branching_node, size_type num_nodes, std::vector<NodeId> forbidden_edges_ids)
{
	size_type iter;
	size_type num_edges = num_nodes * (num_nodes - 1) / 2; //since we have a complete graph	
	
	for (iter = 0; iter < num_edges; iter++) 
		{
			
			//Edge edge= graph_edges.at(iter);
			NodeId edge_id = graph_edges.at(iter).get_id();
			//std::cout<<"Edge cost before change "<<graph_edges.at(iter).cost()<<std::endl;
			//std::cout<<"Forbidden edges size "<<forbidden_edges_ids.size()<<std::endl;
			if(forbidden_edges_ids.size()!=0)
			{
				if(find(forbidden_edges_ids.begin(), forbidden_edges_ids.end(), edge_id) == forbidden_edges_ids.end())
				{
					graph_edges.at(iter).set_cost(graph_edges.at(iter).euclidean_cost() + branching_node.lambda().at(graph_edges.at(iter).nodes().first) + branching_node.lambda().at(graph_edges.at(iter).nodes().second));
				}
			}
			else
				graph_edges.at(iter).set_cost(graph_edges.at(iter).euclidean_cost() + branching_node.lambda().at(graph_edges.at(iter).nodes().first) + branching_node.lambda().at(graph_edges.at(iter).nodes().second));
			
			//std::cout<<"Edge cost after change "<<graph_edges.at(iter).cost()<<std::endl;
		}
		
		//TODO: DELETE
		// for(unsigned int i = 0; i < num_edges; i++)
			// std::cout<<"Cost of Edge from "<<graph_edges.at(i).nodes().first<<" to "<<graph_edges.at(i).nodes().second<<" is "<<graph_edges.at(i).cost()<<std::endl;
}
	

void update_lambda(Min_1_tree & current_tree, std::vector<int> & previous_degrees, BranchingNode & branching_node, size_type iter, size_type num_nodes, double stepsize)
{
	double dampening_constant = 1;
	if (iter >= 2) dampening_constant = 0.6; 

	for(size_type inner_iter = 0; inner_iter < num_nodes; inner_iter++)
	{
		double temp = branching_node.lambda().at(inner_iter);
		//std::cout << "stepsize" << stepsize << std::endl;
		//std::cout << "current_tree.degree(inner_iter), inner_iter:" << inner_iter << "," << current_tree.degree(inner_iter) << std::endl;	
		//std::cout << "temp" << temp << std::endl;
		temp += stepsize * (dampening_constant * ( current_tree.degree(inner_iter) - 2 ) + (1 - dampening_constant) * ( previous_degrees.at(inner_iter) - 2 ));
		branching_node.set_lambda_at(inner_iter, temp);  
	}
	
}
	
void HK_lower_bound_alg(std::vector <Edge> & graph_edges, BranchingNode & branching_node, BranchingNode & branching_root, size_type num_nodes, bool is_root)
{
	// Comparing to notation on exercise sheet: iter_max = N, stepsize_0 = t_0
	double stepsize_0 = 0; 
	size_type iter_max;
	size_type iter;
	
	switch (is_root)
	{
		case 1: //root
		{
			stepsize_0 = min_1_tree(graph_edges, num_nodes, branching_root).cost() / (2 * num_nodes);
			iter_max = std::ceil ( num_nodes * num_nodes / 50 ) + num_nodes + 15;
			branching_node.lambda() = std::vector <double> (num_nodes, 0);
			break;
		}

		case 0: //not root
		{
			for (iter = 0; iter < num_nodes; iter++) stepsize_0 += std::abs(branching_root.lambda().at(iter));
			stepsize_0 /= 2 * num_nodes;
			iter_max = std::ceil ( num_nodes / 4 ) + 5;
			break;
		}
	}
	
	double delta = 3. * stepsize_0 / (2. * iter_max);
	double double_delta = stepsize_0 / ( iter_max * iter_max - iter_max);
	double stepsize = stepsize_0;
	double epsilon = 0.; //TODO: epsilon-Wert?
	branching_node.set_HK_bound(0); // the maximal cost from all iterations is positive, since the cost of the min-cost-1-tree from the first iteration is positive
	//std::cout<<"HK Bound is "<<branching_node.HK_bound()<<std::endl;
	double current_cost;
	
	std::vector<int> previous_degrees = std::vector<int> (num_nodes, 0); //saves for all nodes the degrees in the min-cost-1-tree of the previous iteration	
	Min_1_tree current_tree = Min_1_tree(num_nodes);
	
	//set cost of forbidden edges to infinity
	size_type forbidden_edges_size = branching_node.forbidden_edges().size();
	std::vector<NodeId> forbidden_edges_ids = std::vector <NodeId> (forbidden_edges_size);
	for (size_type j = 0; j < forbidden_edges_size; j++)
	{
		forbidden_edges_ids.at(j) = branching_node.forbidden_edges().at(j).get_id();
		branching_node.forbidden_edges().at(j).set_cost(invalid_cost);
	}
	
	
	//actual algorithm
	for(iter = 0; iter < iter_max; iter++)
	{
		set_new_cost(graph_edges, branching_node, num_nodes, forbidden_edges_ids);
		
		current_tree = min_1_tree(graph_edges, num_nodes, branching_node);
		std::cout<<"Current tree cost is "<<current_tree.cost()<<std::endl;
		// std::cout << "lambda.at(2)" << branching_node.lambda().at(2) << std::endl;
		// std::cout << "lambda.at(3)" << branching_node.lambda().at(3) << std::endl;
		current_cost = std::ceil((1 - epsilon)* current_tree.cost()); // (1-epsilon) correction factor for floating point arithmetic
		//std::cout<<"Current cost is "<<current_cost<<std::endl;
		if (current_cost > branching_node.HK_bound())
		{
			branching_node.set_HK_bound(current_cost);
			branching_node.set_HK_min_tree(current_tree);
			//std::cout<<"HK Bound is "<<branching_node.HK_bound()<<std::endl;
		}			
		// std::cout<<"HERE before update_lambda"<<std::endl;
		update_lambda(current_tree, previous_degrees, branching_node, iter, num_nodes, stepsize);
		
		// std::cout<<"HERE after update_lambda"<<std::endl;
		stepsize -= delta;
		delta -= double_delta;
		for(size_type inner_iter = 0; inner_iter < num_nodes; inner_iter++) previous_degrees.at(inner_iter) = current_tree.degree(inner_iter);
		//std::cout<<"t the end HK Bound is "<<branching_node.HK_bound()<<std::endl;

	}
	std::cout << "hier zumindest" << std::endl;
}
	
}//namespace HK