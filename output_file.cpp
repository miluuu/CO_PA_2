std::ofstream solution_file;
		solution_file.open(optimal_tour_filename, std::ofstream::out);
		solution_file<<"TYPE : TOUR"<<std::endl;
		solution_file<<"DIMENSION : "<<num_nodes<<std::endl;
		solution_file<<"TOUR_SECTION"<<std::endl;
		solution_file<<"1"<<std::endl;
		NodeId previous_node = 0;
		NodeId current_node = 0;
		NodeId next_node;
		for(size_type i = 0; i<num_nodes - 1; i++)
		{
			NodeId temp_node = opt_min_1_tree.incident_edges(current_node).at(0).nodes().first;
			if(temp_node != current_node && temp_node != previous_node)	next_node = temp_node;
			else
			{
				temp_node = opt_min_1_tree.incident_edges(current_node).at(0).nodes().second;
				if(temp_node != current_node && temp_node != previous_node)	next_node = temp_node;
				else
				{
					temp_node = opt_min_1_tree.incident_edges(current_node).at(1).nodes().first;
					if(temp_node != current_node && temp_node != previous_node)	next_node = temp_node;
					else
						next_node = opt_min_1_tree.incident_edges(current_node).at(1).nodes().second;
				}
			}
			solution_file<<to_tsplib_id(next_node)<<std::endl;
			previous_node = current_node;
			current_node = next_node;
		}
		solution_file<<"-1"<<std::endl;
		solution_file<<"EOF"<<std::endl;
		solution_file.close();