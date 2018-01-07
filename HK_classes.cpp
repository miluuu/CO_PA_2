#include "HK_classes.hpp"
#include <iostream>
#include <cmath>

namespace HK
{

//////////////// GLOBAL FUNCTIONS //////////////////////
		
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

int const distance(Node &node1, Node &node2) 
{
	double const x1 = node1._x_coord;
	double const x2 = node2._x_coord;
	double const y1 = node1._y_coord;
	double const y2 = node2._y_coord;
	
	return std::lround(std::sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
}


} //namespace HK