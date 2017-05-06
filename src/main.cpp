#include <iostream>
#include <vector>
#include "sdd.h"
#include <unistd.h>
#include "CycleTimer.h"
#include <map>
#include <random>

std::string helper_string = "Flags:\n"
                       "v: verbose\n"
                       "g: we will generate the graph. Takes in argument for type. Will prompt you for size parameters.\n"
                       "    0: weakly connected cliques\n"
                       "    1: grid graph\n"
                       "f: enter a graph from a file (TODO)\n"
                       "p: number of processors to use (TODO)\n"
                       "b: beta value. Default 0.1\n";

void generate_cliques(int num_clusters, int cluster_size, graph &G){
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

void write_ppm(int width, int height, std::vector<int> &clusters, const char *name){
    FILE *fp = fopen(name, "wb");
    std::mt19937 gen(CycleTimer::currentSeconds());
    std::map<int, unsigned char [3]> cluster_colors;
    for(int x : clusters){
        if(!cluster_colors.count(x)){
            for(int i = 0; i<3; i++){
                cluster_colors[x][i] = gen();
            }
        }
    }
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            fwrite(cluster_colors[clusters[i*width+j]], 1, 3, fp);
        }
    }
    fclose(fp);
}
void generate_grid(int width, int height, graph &G){
    G = graph(width*height, std::vector<int> (0));
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            int pos = width*i + j;
            if(i > 0){
                G[pos].push_back(pos-width);
            }
            if(i < height-1){
                G[pos].push_back(pos+width);
            }
            if(j > 0){
                G[pos].push_back(pos-1);
            }
            if(j < width-1){
                G[pos].push_back(pos+1);
            }
        }
    }
}
int main(int argc, char **argv) {
    /* parse arguments: this specifies which version of LDD to run */
    /* fill in parse arguments code here */
    /* end parse arguments */
    int verbose = 0, type = -1, c, num_processors = 2;
    double beta = 0.1;
    char *f_name = NULL;
    std::vector<int> params;
    graph graph_adj;
    while((c = getopt(argc, argv, "vg:b:p:")) != -1){
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
            case 'p':
                num_processors = atoi(optarg);
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
        int w, h;
        std::cout << "Enter width and height of grid: ";
        std::cin >> w >> h;
        generate_grid(w, h, graph_adj);
        printf("DONE GENERATING\n");
        params = {w, h};
    }else if(type == 0){
        int n, k;
        std::cout << "Enter number of cliques and clique size: ";
        std::cin >> n >> k;
        generate_cliques(n, k, graph_adj);
    }else{
        std::cout << "Invalid type\n"; return 0;
    }
    std::vector<int> clusters_seq, clusters_mpx;
    double start = CycleTimer::currentSeconds();
    printf("SEQ START\n");
    sequentialLDD(graph_adj, clusters_seq, beta);
    printf("SEQ DONE\n");
    double mid = CycleTimer::currentSeconds();
    millerPengXuLDD(graph_adj, clusters_mpx, beta);
    double end = CycleTimer::currentSeconds();
    std::cout << "Seq Clusters took: " << mid - start << " sec\n";
    if(verbose){
        if(type == 1){
            write_ppm(params[0], params[1], clusters_seq, "grid_seq.ppm");
        }else{
            std::cout << "Clusters:\n";
            for(auto x : clusters_seq)
                std::cout << x << " ";
            std::cout << "\n\n";
        }
    }
    std::cout << "Miller,Peng,Xu Clusters took: " << end - mid << " sec\n";
    if(verbose){
        if(type == 1){
            write_ppm(params[0], params[1], clusters_mpx, "grid_mpx.ppm");
        }else{
            std::cout << "Clusters:\n";
            for(auto x : clusters_mpx)
                std::cout << x << " ";
            std::cout << "\n\n";
        }
    }
    return 0;
}
