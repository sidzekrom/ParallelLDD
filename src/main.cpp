#include <iostream>
#include <vector>
#include "sdd.h"
#include <unistd.h>
#include "CycleTimer.h"

std::string helper_string = "Flags:\n"
                       "v: verbose\n"
                       "g: we will generate the graph. Takes in argument for type. Will prompt for size.\n"
                       "    0: weakly connected cliques\n"
                       "    1: user-inputted graph (TODO)\n"
                       "f: enter a graph from a file (TODO)\n"
                       "b: beta value. Default 0.1\n";

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
int main(int argc, char **argv) {
    /* parse arguments: this specifies which version of LDD to run */
    /* fill in parse arguments code here */
    /* end parse arguments */
    int verbose = 0, type = -1, c;
    double beta = 0.1;
    char *f_name = NULL;
    graph graph_adj;
    while((c = getopt(argc, argv, "vg:b:")) != -1){
        switch (c){
            case 'v':
                verbose = 1;
                break;
            case 'g':
                type = atoi(optarg);
                break;
            case 'f':
                f_name = optarg;
                break;
            case 'b':
                beta = atof(optarg);
                break;
            case 'h':
                std::cout << helper_string;
                return 0;
        }
    }
    if(type == -1){
        std::cout << helper_string; return 0;
    }
    if(type == 1){
        int num_vertices;
        int num_edges;
        std::cout << "Enter number of vertices and edges: ";
        std::cin >> num_vertices >> num_edges;
    }else if(type == 0){
        int n, k;
        std::cout << "Enter number of cliques and clique size: ";
        std::cin >> n >> k;
        generate_clusters(n, k, graph_adj);
    }else{
        std::cout << "Invalid type\n"; return 0;
    }
    std::vector<int> clusters_seq, clusters_mpx;
    double start = CycleTimer::currentSeconds();
    sequentialLDD(graph_adj, clusters_seq, beta);
    double mid = CycleTimer::currentSeconds();
    millerPengXuLDD(graph_adj, clusters_mpx, beta);
    double end = CycleTimer::currentSeconds();
    std::cout << "Seq Clusters took: " << mid - start << " ns\n";
    if(verbose){
        std::cout << "Clusters:\n";
        for(auto x : clusters_seq)
            std::cout << x << " ";
        std::cout << "\n\n";
    }
    std::cout << "Miller,Peng,Xu Clusters took: " << end - mid << " ns\n";
    if(verbose){
        std::cout << "Clusters:\n";
        for(auto x : clusters_mpx)
            std::cout << x << " ";
        std::cout << "\n\n";
    }
    return 0;
}
