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

//useful constant different from the cost of any actual edge:
size_type constexpr invalid_cost = std::numeric_limits<size_type>::max();

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
	friend size_type distance(Node &node1, Node &node2);
};



class Edge
{
private:
	std::pair<NodeId, NodeId> _nodes;
	size_type _cost;
	size_type _id;
public:
	typedef std::size_t size_type;

	//Creates an edge with nodes of invalid_node_id cost of infinity.
	Edge();
	
	// Creates an edge (the node_id's are the indices of the respective nodes in the vector _nodes_coordinates, which stores at index i the coordinates of node i) (_nodes_coordinates is only needed for the initialization fo the costs), and computes its rounded down euclidean length.
	Edge(NodeId const node1_id, NodeId const node2_id, std::vector<Node> & _nodes_coordinates, size_type id);

	// Returns the ids of the incident nodes of this edge.
	std::pair <NodeId, NodeId> const & nodes() const;
	
	// Returns the cost of this edge (not necessarily its rounded Euclidean length).
	int cost();
	
	//Sets a modified new cost for cost (needed in Held-Karp-lower-bound-algorithm)
	void set_cost (double new_cost);
	
	//Returns the edge id
	NodeId get_id() const;
};


// class for the minimum spanning trees
class Min_1_tree
{
private:
	NodeId _num_nodes;
	std::vector< std::vector<Edge> > _incident_edges;
	
public:
	// creates a graph with num_nodes isolated nodes
	Min_1_tree (NodeId const num_nodes);
	
	//returns the number of nodes in the graph
	NodeId num_nodes() const;
	  
	//returns the degree of the node with NodeId node_id  
	size_type degree (NodeId node_id) const;
	
	//returns the cost of the Min_1_tree
	double cost();
	
	//returns the array of incident edges of the node_id_th node
	std::vector <Edge> const & incident_edges(NodeId const id) const;
	
	//adds edge at the end of the incidence-edges-vector of node id
	void add_edge (NodeId id, Edge & edge);
};	



class BranchingNode
{
private:
	std::vector <Edge>  _required_edges;
	std::vector <Edge>  _forbidden_edges;
	std::vector <double> _lambda; 
	size_type _HK_bound;
	Min_1_tree _HK_min_tree; //min-cost 1-tree from Held-Karp-lower-bound-algorithm
	
public:	
	//Constructor for the root
	BranchingNode (NodeId num_nodes);
	
	//Constructor for the branching nodes, copies all the values from the parent
	BranchingNode(BranchingNode const & parent);
	
	//returns HK(K_n, c, R, F) (c= cost, R=required_edges, F=forbidden_edges)
	size_type HK_bound() const; 
	
	//set the value of HK(K_n, c, R, F)
	void set_HK_bound(size_type value);
	
	//adds an edge to _required_edges
	void add_required_edge(Edge edge);
	
	//adds an edge to _forbidden_edges
	void add_forbidden_edge(Edge edge);
	
	//returns the id-th entry in required_edges
	Edge const & required_edge(size_type id) const;
	
	//returns _HK_min_tree
	Min_1_tree & HK_min_tree();
	
	//sets _HK_min_tree to min_tree
	void set_HK_min_tree(Min_1_tree & min_tree);
	
	//returns the number of required edges
	size_type required_edges_size() const;
	
	//returns the vector with the lambda values
	std::vector <double> & lambda();
};
	

	
//BEGIN: Inline section


inline
std::pair <NodeId, NodeId> const & Edge::nodes() const
{
	return _nodes;
}

inline
int Edge::cost()
{
	return _cost;
}

inline
NodeId Edge::get_id() const
{
	return _id;
}


inline
NodeId Min_1_tree::num_nodes() const
{
   return _num_nodes;
}


inline
size_type Min_1_tree::degree(NodeId node_id) const
{
   return (_incident_edges.at(node_id)).size();
}

inline
std::vector <Edge> const & Min_1_tree::incident_edges(NodeId const id) const
{
	return _incident_edges.at(id);
}

inline
size_type BranchingNode::HK_bound()   const
{
	return _HK_bound;
}

inline
Edge const & BranchingNode::required_edge(size_type id) const
{
	return _required_edges.at(id);
}

inline
Min_1_tree & BranchingNode::HK_min_tree() 
{
	return _HK_min_tree;
}

inline
size_type BranchingNode::required_edges_size() const
{
	return _required_edges.size();
}

inline
std::vector <double> & BranchingNode::lambda() 
{
	return _lambda;
}

//END: Inline section
	

} //namespace HK


#endif /* HK_CLASSES_HPP */