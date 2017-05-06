run_sdd: src/sdd.cpp src/main.cpp
	g++ -std=c++11 -o3 -fopenmp -o run_sdd src/sdd.cpp src/main.cpp
