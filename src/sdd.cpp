#include <vector>
#include <queue>
#include <random>
#include <omp.h>
#include <cstddef>
#include "CycleTimer.h"
#include "sdd.h"
#define MAX_THREADS 64
void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<int> dist (input_graph.size(), -1);
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
    int cluster;
    double tie_breaker;
    frontier_elem(int c, double t) : cluster(c), tie_breaker(t){}
    frontier_elem(){ cluster = -1; tie_breaker = 0; }
};
void millerPengXuLDD(graph &input_graph, std::vector<int> &clusters, double beta){
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<double> start_times(input_graph.size());
    std::vector<frontier_elem> tie_breakers (input_graph.size());
    std::random_device rd;
    std::vector<std::mt19937> gens (MAX_THREADS, std::mt19937(rd()));
    std::exponential_distribution<double> d(beta);
    #pragma parallel for
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = d(gens[omp_get_thread_num()]);
    }
    double mx = 0;
    for(int i = 0; i<start_times.size(); i++){
        if(mx < start_times[i]){
            mx = start_times[i];
        }
    }
    std::vector<int> frontier;
    std::vector<int> putoffs;
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = mx - start_times[i];
        if(start_times[i] < 1){
            clusters[i] = i;
            frontier.push_back(i);
            tie_breakers[i] = frontier_elem(i, start_times[i]);
        }else{
            putoffs.push_back(i);
        }
    }
    for(int i = 0; i<10; i++)
        printf("%lf ", start_times[i]);
    printf("\n");
    omp_lock_t *vtx_locks = (omp_lock_t *) malloc(sizeof(omp_lock_t) * input_graph.size());
    for(int i = 0; i<input_graph.size(); i++)
        omp_init_lock(vtx_locks+i);
    int num_rounds = 1;
    double t1 = CycleTimer::currentSeconds();
    while(!frontier.empty()){
        std::vector<std::vector<int> > next_frontier_pieces(MAX_THREADS);
        std::vector<int> next_putoffs;
        #pragma omp parallel for
        for(int i = 0; i<frontier.size(); i++){
            int t = omp_get_thread_num();
            frontier_elem z = tie_breakers[frontier[i]];
            for(int v : input_graph[frontier[i]]){
                if(clusters[v] == -1){
                    omp_set_lock(vtx_locks+v);
                    if(tie_breakers[v].cluster == -1 || tie_breakers[v].tie_breaker > 
                            z.tie_breaker){
                        tie_breakers[v] = z;
                        omp_unset_lock(vtx_locks+v);
                        next_frontier_pieces[t].push_back(v);
                    }else
                        omp_unset_lock(vtx_locks+v);
                }
            }
        }
        frontier.clear();
        for(int x : putoffs){
            if(start_times[x] < num_rounds+1){
               if(clusters[x] == -1 && (tie_breakers[x].cluster = -1
                    || tie_breakers[x].tie_breaker > start_times[x] - num_rounds)){
                tie_breakers[x] = frontier_elem(x, start_times[x] - num_rounds);
                clusters[x] = x;
                frontier.push_back(x);
               }
            }else
                next_putoffs.push_back(x);
        }
        for(std::vector<int> piece : next_frontier_pieces){
            for(int v : piece){
                if(clusters[v] == -1){
                    clusters[v] = tie_breakers[v].cluster;
                    frontier.push_back(v);
                }
            }
        }
        putoffs = next_putoffs;
        num_rounds++;
    }
    free(vtx_locks);
    double t2 = CycleTimer::currentSeconds();
    printf("%lf sec\n", t2-t1);
}
