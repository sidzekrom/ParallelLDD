#include <iostream>
#include <vector>
#include "sdd.h"

void generate_clusters(int num_clusters, int cluster_size, graph &G){
  G = graph(num_clusters*cluster_size, std::vector<int> (0));
  for(int i = 0; i<G.size(); i+=cluster_size){
    for(int j = 0; j<cluster_size; j++){
      for(int k = j+1; k<cluster_size; k++){
        G[i+j].push_back(i+k);
        G[i+k].push_back(i+j);
      }
    }
    if(i+cluster_size< G.size()){
      G[i].push_back(i+cluster_size);
      G[i+cluster_size].push_back(i);
    }

  }
}
int main() {
    /* parse arguments: this specifies which version of LDD to run */
    /* fill in parse arguments code here */
    /* end parse arguments */
    std::cout << "0 for inputted graph\n1 for n weakly-connected cliques of size k\n";
    int type; std::cin >> type;
    graph graph_adj;
    std::vector<int> clusters;
    double beta;
    if(type == 0){
        /* creating graph from standard input right now, try to do something else */
        int num_vertices;
        int num_edges;
        std::cin >> num_vertices >> num_edges >> beta;
    }else if(type == 1){
        int n, k;
        std::cin >> n >> k >> beta;
        generate_clusters(n, k, graph_adj);
    }
    sequentialLDD(graph_adj, clusters, beta);
    std::cout << "Clusters:\n";
    for(auto x : clusters)
        std::cout << x << " ";
    std::cout << "\n";
    return 0;
}
