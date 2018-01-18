build: 
	g++ main.cpp HK_lower_bound_alg.cpp HK_subroutines.cpp HK_classes.cpp -o main.o
		
clean: 
	-rm -f *.o core
