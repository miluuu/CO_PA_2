#ifndef HK_SUBROUTINES_HPP
#define HK_SUBROUTINES_HPP

#include "HK_classes.hpp"
#include <list>

namespace HK
{

		
//Returns 1 if Min_1_tree is already a tour, otherwise returns 0 and writes two incident edges of a vertex with degree greater than 2 in edge1 and edge2.
bool tour_check (Min_1_tree const & tree, Edge & edge1, Edge & edge2);

//updates priority queue in the branching case (the processed parent branching node is removed, the three new nodes are inserted into the sorted list)
void update_queue (BranchingNode const & parent, BranchingNode const & child1, BranchingNode const & child2, BranchingNode const & child3, std::list<BranchingNode> & candidates);

//Branches in the way described in the exercise (subroutine for process_branching_node).
void branch_and_update_queue (BranchingNode const & branching_node, std::list <BranchingNode> & candidates, Edge & edge1, Edge & edge2);

/*(The candidate list Q is implemented in the list "candidates", sorted by their HK-lower-bound in ascending order. We always process the first branching node of the list.) 
 Processes branching_node: 
 Checks if respective min-cost-1-tree has lower cost than U: 
 N) If no, discard it. 
 Y) If yes, check if it is already an optimum tour: 
	Y.Y) If yes, then update the upper bound. 
	Y.N) If no, branch. 
 In all cases: Remove branching_node from the candidate list.
*/
void process_branching_node (BranchingNode const & branching_node, Min_1_tree const min_tree, size_type min_tree_cost, size_type upper_bound, std::list <BranchingNode> & candidates);
	
} //namespace HK

#endif /* HK_SUBROUTINES_HPP */