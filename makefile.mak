build: 
	g++ main.cpp HK_lower_bound_alg.cpp HK_subroutines.cpp HK_classes.cpp -o main.exe
		
clean: 
	-rm -f *.exe core