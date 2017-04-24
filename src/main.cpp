#include <iostream>
#include <vector>
#include "sequential.cpp"

int main() {
    /* creating graph from standard input right now, try to do something else */
    int num_vertices;
    int num_edges;
    /* parse arguments: this specifies which version of LDD to run */
    /* fill in parse arguments code here */
    /* end parse arguments */
    std::cin >> num_vertices >> num_edges;
    std::vector<std::vector<int> > graph_adj;
    std::vector<int> clusters;
    
    sequentialLDD(graph_adj, clusters, 0.0);

    return 0;
}
