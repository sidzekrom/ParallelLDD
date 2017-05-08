#include <vector>
#include <queue>
#include "sdd.h"
#include <random>


void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    std::vector<int> clusterdist(3*input_graph.size(), -1);
    for (int i = 0; i < input_graph.size(); i++) {
        std::vector<int> current_frontier;
        std::vector<int> next_frontier;
        int num_internal = 0;
        int num_out;
        if (clusterdist[3*i] != -1)
            continue;
        current_frontier.push_back(i);
        clusterdist[3*i] = current_cluster;
        clusterdist[3*i+1] = 0;
        clusterdist[3*i+2] = 1;
        // build next_frontier: lots of repetitions: gives
        // all the outedges
        int current_dist = 0;
        while (current_frontier.size() > 0) {
            num_out = 0;
            for (int v : current_frontier) {
                for (int x : input_graph[v]) {
                    if (clusterdist[3*x] == -1) {
                        num_out++;
                        if (clusterdist[3*x+2] == -1) {
                            clusterdist[3*x+2] = 1;
                            next_frontier.push_back(x);
                        }
                    }
                    else if (clusterdist[3*x] == current_cluster &&
                             clusterdist[3*x+1] == current_dist)
                        num_internal++;
                }
            }
            if ((double) num_out <
                    beta * ((double) num_internal)) {
                for (int v : next_frontier) {
                    clusterdist[3*v+2] = -1;
                }
                break;
            }
            current_dist++;
            num_internal += num_out;
            current_frontier.clear();
            for (int v : next_frontier) {
                if (clusterdist[3*v] == -1) {
                    current_frontier.push_back(v);
                    clusterdist[3*v] = current_cluster;
                    clusterdist[3*v+1] = current_dist;
                }
            }
            next_frontier.clear();
        }
        /* current_cluster finished, start next cluster */
        current_cluster++;
    }
    clusters = std::vector<int> (input_graph.size());
    for (int i = 0; i < clusters.size(); i++) {
        clusters[i] = clusterdist[3 * i];
    }
    return;
}


class frontier_elem{
    public:
    int vtx;
    int cluster;
    double tie_breaker;
    frontier_elem(int v, int c, double t) : vtx(v), cluster(c), tie_breaker(t){}
    frontier_elem(){ vtx = -1; cluster = 0; tie_breaker = 0; }
};


void millerPengXuLDD(graph &input_graph, std::vector<int> &clusters, double beta){
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<double> start_times(input_graph.size());
    std::vector<frontier_elem> tie_breakers (input_graph.size());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<double> d(beta);
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = d(gen);
    }
    double mx = 0;
    for(int i = 0; i<start_times.size(); i++){
        if(mx < start_times[i]){
            mx = start_times[i];
        }
    }
    std::vector<frontier_elem> frontier;
    std::vector<int> putoffs;
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = mx - start_times[i];
        if(start_times[i] < 1){
            clusters[i] = i;
            frontier.push_back(frontier_elem(i, i, start_times[i]));
        }else{
            putoffs.push_back(i);
        }
    }
    int num_rounds = 1;
    while(!frontier.empty()){
        std::vector<frontier_elem> next_frontier;
        std::vector<int> next_putoffs;
        for(auto x : frontier){
            for(int v : input_graph[x.vtx]){
                if(clusters[v] == -1){
                    next_frontier.push_back(frontier_elem(v, x.cluster, x.tie_breaker));
                }
            }
        }
        //Sort out the new frontier
        for(int x : putoffs){
            if(start_times[x] < num_rounds+1)
                next_frontier.push_back(frontier_elem(x, x, start_times[x] - num_rounds));
            else
                next_putoffs.push_back(x);
        }
        for(auto x : next_frontier){
            if(tie_breakers[x.vtx].vtx == -1 || tie_breakers[x.vtx].tie_breaker > x.tie_breaker){
                tie_breakers[x.vtx] = x;
            }
        }
        frontier.clear();
        putoffs.clear();
        for(auto x : next_frontier){
            if(clusters[x.vtx] == -1){
                frontier.push_back(tie_breakers[x.vtx]);
                clusters[x.vtx] = tie_breakers[x.vtx].cluster;
            }
        }
        putoffs = next_putoffs;
        next_frontier.clear();
        next_putoffs.clear();
        num_rounds++;
    }
}
