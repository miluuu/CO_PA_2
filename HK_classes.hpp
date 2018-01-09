#ifndef HK_CLASSES_HPP
#define HK_CLASSES_HPP


//This file provides classes for the Held-Karp-lower-bound-algorithm: an edge class and a class for the branching nodes.

#include <iostream>
#include <limits>
#include <vector>

namespace HK //for Held-Karp(-lower-bound)
{
	
//typedefs from previous programming exercise
using size_type = std::size_t;

//! Use different typedefs for internal standard indexing starting with 0
//! and TSPLIB-based indexing starting with 1.
using NodeId = size_type;
using TsplibId = size_type;

/** Useful constant different from the id of any actual node: **/
NodeId constexpr invalid_node_id = std::numeric_limits<NodeId>::max();
TsplibId constexpr invalid_tsplib_id = std::numeric_limits<TsplibId>::max();

/**
   Nodes in TSPLIB files are counted from 1, but here we count them from 0 so they match their std::vector indices.
   These two trivial functions should help make the transition between the two models clear (instead of just having
   some unexplained -1's and +1's in the middle of the code).
**/
NodeId from_tsplib_id(TsplibId const tsplib_id); //!< Subtracts 1 (throws if @c tsplib_id is 0)
TsplibId to_tsplib_id(NodeId const node_id);     //!< Adds 1 (throws if overflow would occur)

class Node 
{
private:	
	double const _x_coord;
	double const _y_coord;

public:
	Node(double x_coord, double y_coord): _x_coord(x_coord), _y_coord(y_coord)
	{
	}

	// Computes rounded euclidean distance
	friend int const distance(Node &node1, Node &node2);
};



class Edge
{
private:
	std::pair<NodeId, NodeId> _nodes;
	size_type _cost;

public:
	typedef std::size_t size_type;

	// Creates an edge (the node_id's are the indices of the respective nodes in the vector graph_nodes, which stores at index i the coordinates of node i), and computes its rounded down euclidean length.
	Edge(NodeId const node1_id, NodeId const node2_id, std::vector<Node> & graph_nodes)
	{
		_nodes.first = node1_id;
		_nodes.second = node2_id;
		
		Node & node1 = graph_nodes.at(node1_id);
		Node & node2 = graph_nodes.at(node2_id);
		
		_cost = distance(node1, node2);
	}

	// Returns the ids of the incident nodes of this edge.
	std::pair <NodeId, NodeId> const & nodes() const;
	
	// Returns the rounded Euclidean length of this edge.
	int const cost() const;
};


class BranchingNode
{
private:
	std::vector <Edge>  _required_edges;
	std::vector <Edge>  _forbidden_edges;
	std::vector <double> _lambda; 
	
public:	
	BranchingNode ():
	_required_edges(std::vector <Edge>()), _forbidden_edges(std::vector <Edge>()), _lambda(std::vector <double>())
	{}
	
	//Constructor for the branching nodes
	BranchingNode(BranchingNode const & parent):
	_required_edges(parent._required_edges), _forbidden_edges (parent._forbidden_edges), _lambda(parent._lambda)
	{}
	
	void add_required_edge(Edge edge);
	void add_forbidden_edge(Edge edge);
	
	//The candidate list Q is implemented in the vector "candidates".
	void branch (std::vector <BranchingNode> & candidates, Edge & edge1, Edge & edge2);	
};
	

//BEGIN: Inline section

inline
std::pair <NodeId, NodeId> const & Edge::nodes() const
{
	return _nodes;
}

inline
int const Edge::cost() const
{
	return _cost;
}

//END: Inline section
	

} //namespace HK


#endif /* HK_CLASSES_HPP */