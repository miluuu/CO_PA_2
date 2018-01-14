#include "HK_classes.hpp"
#include "HK_subroutines.hpp"
#include "HK_lower_bound_alg.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace HK
{

void set_new_cost (std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_node, size_type num_nodes, std::vector<double> & lambda)
{
	size_type iter;
	size_type num_edges = num_nodes * (num_nodes - 1) / 2; //since we have a complete graph	
	
	for (iter = 0; iter < num_edges; iter++) 
		{
			Edge edge= graph_edges.at(iter);
			edge.set_cost(euclidean_costs.at(iter) + branching_node.lambda().at(edge.nodes().first) + branching_node.lambda().at(edge.nodes().second));
		}
}
	

void update_lambda(Min_1_tree & current_tree, std::vector<size_type> & previous_degrees, std::vector<double> & lambda, size_type iter, size_type num_nodes, double stepsize)
{
	double dampening_constant = 1;
	if (iter >= 2) dampening_constant = 0.6;

	for(size_type inner_iter = 0; inner_iter < num_nodes; iter++)
	{
		lambda.at(inner_iter) += stepsize * (dampening_constant * ( current_tree.degree(inner_iter) - 2 ) + (1 - dampening_constant) * ( previous_degrees.at(inner_iter) - 2 ));
	}
	
}
	
double HK_lower_bound_alg(std::vector <Edge> & graph_edges, std::vector <size_type> & euclidean_costs, BranchingNode & branching_node, BranchingNode & branching_root, size_type num_nodes, bool is_root)
{
	//TODO: Delete
	Min_1_tree MIN_COST_TREE = Min_1_tree (num_nodes);
	double COST_MIN_COST_TREE = 1;


	// Comparing to notation on exercise sheet: iter_max = N, stepsize_0 = t_0
	double stepsize_0 = 0; 
	size_type iter_max;
	size_type iter;
	
	switch (is_root)
	{
		case 1: //root
		{
			//TODO: Funktion anpassen
			stepsize_0 = COST_MIN_COST_TREE / (2 * num_nodes);
			iter_max = std::ceil ( num_nodes * num_nodes / 50 ) + num_nodes + 15;
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
	
	double delta_0 = 3 * stepsize_0 / 2 * iter_max;
	double delta = delta_0;
	double double_delta = stepsize_0 / ( iter_max * iter_max - iter_max);
	double stepsize = stepsize_0;
	
	
	//initialize lambda and the cost vector that saves the cost of the alculated min-cost-1-trees from all iterations
	branching_node.lambda() = std::vector <double> (num_nodes, 0);
	std::vector<double> tree_costs = std::vector<double> (iter_max);
	std::vector<size_type> previous_degrees = std::vector<size_type> (num_nodes); //saves for all nodes the degrees in the min-cost-1-tree of the previous iteration	
	
	//actual algorithm
	for(iter = 0; iter < iter_max; iter++)
	{
		set_new_cost(graph_edges, euclidean_costs, branching_node, num_nodes, branching_node.lambda());
		
		//TODO: Einfügen: Computer minimum weight-1-tree
		tree_costs.at(iter) = COST_MIN_COST_TREE;
		update_lambda(MIN_COST_TREE, previous_degrees, branching_node.lambda(), iter, num_nodes, stepsize);
		stepsize -= delta;
		delta -= double_delta;
		//TODO: MIN_COST_TREE ersetzen
		for(size_type inner_iter = 0; inner_iter < num_nodes; inner_iter++) previous_degrees.at(inner_iter) = MIN_COST_TREE.degree(inner_iter);
	}
	
	return *max_element(tree_costs.begin(), tree_costs.end());
}
	
}//namespace HK