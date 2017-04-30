#include <vector>
#include <queue>
#include "sdd.h"
#include <random>
void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<int> dist (input_graph.size(), 0);
    for (int i = 0; i < input_graph.size(); i++) {
        std::vector<int> current_frontier;
        int num_internal = 0;
        std::vector<int> next_frontier;
        if (clusters[i] == -1) {
            current_frontier.push_back(i);
        }else
            continue;
        clusters[i] = current_cluster;
        // build next_frontier: lots of repetitions: gives
        // all the outedges
        int current_dist = 0;
        while (true) {
            for (int v : current_frontier) {
                for (int x : input_graph[v]) {
                    if (clusters[x] == -1) {
                        next_frontier.push_back(x);
                    }else if(clusters[x] == current_cluster && dist[x] == current_dist)
                        num_internal++;
                }
            }
            if ((double) next_frontier.size() <
                    beta * ((double) num_internal)) {
                break;
            }
            current_dist++;
            num_internal += next_frontier.size();
            current_frontier.clear();
            for (int v : next_frontier) {
                if(clusters[v] == -1){
                    current_frontier.push_back(v);
                    clusters[v] = current_cluster;
                    dist[v] = current_dist;
                }
            }
            next_frontier.clear();
        }
        /* current_cluster finished, start next cluster */
        current_cluster++;
    }
    return;
}
class frontier_elem{
    public:
    int vtx;
    int cluster;
    double tie_breaker;
    frontier_elem(int v, int c, double t) : vtx(v), cluster(c), tie_breaker(t){}
    /*
    frontier_elem(){
        vtx = 0; cluster = 0; tie_breaker = 0;    
    };
    */
};
void millerPengXuLDD(graph &input_graph, std::vector<int> &clusters, double beta){
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<double> start_times(input_graph.size());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<double> d(beta);
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = d(gen);
    }
    double mx = 0;
    int start = 0;
    for(int i = 0; i<start_times.size(); i++){
        if(mx > start_times[i]){
            mx = start_times[i];
            start = i;
        }
    }
    std::vector<frontier_elem> frontier;
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = mx - start_times[i];
        frontier.push_back(frontier_elem(i, i, start_times[i] % 1.0));
    }
    int num_rounds = 0;
    while(!frontier.empty()){
        std::vector<frontier_elem> next_frontier;
        for(auto x : frontier){
            if(start_times[x.vtx] > num_rounds+1){
                next_frontier.push_back(x);
            }else if(start_times[x.vtx] > num_rounds){
                if(clusters[x.vtx] == -1){
                    clusters[x.vtx] = x.cluster;
                }
            }
        }
    }
}
