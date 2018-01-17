#include "HK_subroutines.hpp"
#include "my_subroutines.hpp"
#include "HK_classes.hpp"
#include "HK_lower_bound_alg.hpp"
#include <cstring>

int main(int argc, char** argv)
{
	std::string filename;
	std::string optimal_tour_filename;
	for(int i=0; i<argc; i++)
	{
		if(std::strcmp(argv[i], "--instance")==0)
		{
			if(i==argc-1) 
			{std::cout<<"The command '--instance' needs the name of a tsp-file as input"<<std::endl;
			return EXIT_FAILURE;
			}
			if(i+1 < argc)
				filename = argv[i+1];
		}
		if(std::strcmp(argv[i], "--solution")==0)
		{
			if(i==argc-1) 
			{std::cout<<"The command '--solution' needs the name of a tour-file as input"<<std::endl;
			return EXIT_FAILURE;
			}
			if(i+1 < argc)
				optimal_tour_filename = argv[i+1];
		}
	}
	 HK::branch_and_bound(filename, optimal_tour_filename);
	// HK::Node node1 = HK::Node(3,5.);
	// HK::Node node2= HK::Node(1.,1.);
	
	// std::cout << "HK::distance(node1, node2)" << HK::distance(node1, node2)<< std::endl;
	
	
	return EXIT_SUCCESS;
	
	
	
}