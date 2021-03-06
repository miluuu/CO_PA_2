#include "HK_classes.hpp"
#include <cmath>

namespace HK
{

///////////////// Edge definitions /////////////////////

Edge::Edge()
{
	_nodes = std::pair <NodeId, NodeId> (invalid_node_id, invalid_node_id);
	_cost = invalid_cost;
	_euclidean_cost = invalid_cost;
	_id = -1;
}

Edge::Edge(NodeId const node1_id, NodeId const node2_id, std::vector<Node> & _nodes_coordinates, NodeId id)
{
	_nodes.first = node1_id;
	_nodes.second = node2_id;
	
	Node & node1 = _nodes_coordinates.at(node1_id);
	Node & node2 = _nodes_coordinates.at(node2_id);
	
	_cost = distance(node1, node2);
	_euclidean_cost = distance(node1, node2);
	
	_id = id;
}

void Edge::set_cost (double new_cost)
{
	_cost = new_cost;
}

//////////////// Min_1_tree definitions //////////////////////

Min_1_tree::Min_1_tree(NodeId const num_nodes)
   :
   _num_nodes(num_nodes),
   _incident_edges (std::vector < std::vector<Edge> > (num_nodes))
{}



double Min_1_tree::cost()
{
	double cost = 0;
	for(size_type iter1 = 0; iter1 < _num_nodes; iter1++) 
	{
		for (size_type iter2 = 0; iter2 < _incident_edges.at(iter1).size(); iter2++)
			cost += _incident_edges.at(iter1).at(iter2).cost();
	}
	return 0.5 * cost; //since every edge appears twice as an incident edge
}

void Min_1_tree::add_edge (NodeId id, Edge & edge)
{
	(_incident_edges.at(id)).push_back(edge);
}


///////////////// BranchingNode definitions ////////////

BranchingNode::BranchingNode(NodeId num_nodes)
	:
	_required_edges(std::vector <Edge>()),
	_forbidden_edges(std::vector <Edge>()),
	_lambda(std::vector <double>(num_nodes)),
	_HK_bound(invalid_cost),
	_HK_min_tree(Min_1_tree(num_nodes))
{}
	
BranchingNode::BranchingNode(BranchingNode const & parent)
	:
	_required_edges(parent._required_edges), 
	_forbidden_edges (parent._forbidden_edges), 
	_lambda(parent._lambda),
	_HK_bound(parent._HK_bound),
	_HK_min_tree(parent._HK_min_tree)
	
{}

void BranchingNode::set_HK_bound(size_type value) 
{
	_HK_bound = value;
}

void BranchingNode::add_required_edge(Edge edge)
{
	_required_edges.push_back(edge);
}


void BranchingNode::add_forbidden_edge(Edge edge)
{
	_forbidden_edges.push_back(edge);
}


void BranchingNode::set_HK_min_tree(Min_1_tree & min_tree)
{
		_HK_min_tree = min_tree;
}

void BranchingNode::set_lambda_at(NodeId id, double value)
{
	_lambda.at(id) = value;
}

//////////////// Global functions //////////////////////
		
NodeId from_tsplib_id(TsplibId const tsplib_id)
{
   if (tsplib_id == 0)
   {
      throw std::runtime_error("Invalid (0) TSPLIB id.");
   }

   return static_cast<NodeId>(tsplib_id - 1);
}

TsplibId to_tsplib_id(NodeId const node_id)
{
   if (node_id == std::numeric_limits<NodeId>::max())
   {
      throw std::runtime_error("Invalid (inf) node id.");
   }

   return static_cast<TsplibId>(node_id + 1);
}

int distance(Node &node1, Node &node2) 
{
	double const x1 = node1._x_coord;
	double const x2 = node2._x_coord;
	double const y1 = node1._y_coord;
	double const y2 = node2._y_coord;
	
	return std::lround(std::sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}


} //namespace HK