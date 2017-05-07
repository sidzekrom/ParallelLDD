#include <vector>
#include <queue>
#include <random>
#include <omp.h>
#include <cstddef>

#include "sdd.h"
void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<int> dist (input_graph.size(), -1);
    omp_lock_t lock;
    omp_init_lock(&lock);
    for (int i = 0; i < input_graph.size(); i++) {
        std::vector<int> current_frontier;
        int num_internal = 0;
        std::vector<int> next_frontier;
        if (clusters[i] != -1)
            continue;
        current_frontier.push_back(i);
        clusters[i] = current_cluster;
        dist[i] = 0;
        // build next_frontier: lots of repetitions: gives
        // all the outedges
        int current_dist = 0;
        while (current_frontier.size() > 0) {
            int num_outs = 0;
            int bt_frontier = 0;
            for (int v = 0; v<current_frontier.size(); v++) {
                for (int x : input_graph[current_frontier[v]]) {
                    num_outs += dist[x] == -1;
                    if (clusters[x] == -1) {
                        clusters[x] = current_cluster;
                        next_frontier.push_back(x);
                    }else if(clusters[x] == current_cluster && dist[x] == current_dist)
                        bt_frontier++;
                }
            }
            num_internal += bt_frontier>>1;
            if (num_outs < beta * ((double) num_internal)) {
                for(int x : next_frontier)
                    clusters[x] = -1;
                break;
            }
            num_internal += num_outs;
            current_dist++;
            current_frontier = next_frontier;
            for(int x : next_frontier)
                dist[x] = current_dist;
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
