#include <vector>
#include <queue>
#include <random>
#include <omp.h>
#include <cstddef>
#include <set>
#include "CycleTimer.h"
#include "sdd.h"


void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    std::vector<int> clusterdist(2*input_graph.size(), -1);
    for (int i = 0; i < input_graph.size(); i++) {
        std::vector<int> current_frontier;
        std::vector<int> next_frontier;
        int num_internal = 0;
        if (clusterdist[2*i] != -1)
            continue;
        current_frontier.push_back(i);
        clusterdist[2*i] = current_cluster;
        clusterdist[2*i+1] = 0;
        // build next_frontier: lots of repetitions: gives
        // all the outedges
        int current_dist = 0;
        while (current_frontier.size() > 0) {
            int num_out = 0;
            int bt_frontier = 0;
            for (int v : current_frontier) {
                for (int x : input_graph[v]) {
                    if (clusterdist[2*x] == -1) {
                        num_out++;
                        clusterdist[2*x] = current_cluster;
                        clusterdist[2*x+1] = current_dist+1;
                        next_frontier.push_back(x);
                    }
                    else if (clusterdist[2*x] == current_cluster &&
                             clusterdist[2*x+1] == current_dist)
                        bt_frontier++;
                }
            }
            num_internal += bt_frontier >> 1;
            if ((double) num_out <
                    beta * ((double) num_internal)) {
                for (int v : next_frontier) {
                    clusterdist[2*v] = -1;
                    clusterdist[2*v+1] = -1;
                }
                break;
            }
            current_dist++;
            num_internal += num_out;
            current_frontier = next_frontier;
            next_frontier.clear();
        }
        /* current_cluster finished, start next cluster */
        current_cluster++;
    }
    clusters = std::vector<int> (input_graph.size());
    for (int i = 0; i < clusters.size(); i++) {
        clusters[i] = clusterdist[2 * i];
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


void millerPengXuLDD(graph &input_graph, std::vector<int> &clusters, double beta,
        int max_num_threads){
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<double> start_times(input_graph.size());
    std::vector<frontier_elem> tie_breakers (input_graph.size());
    std::random_device rd;
    std::vector<std::mt19937> gens (max_num_threads, std::mt19937(rd()));
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
    std::vector<std::set<int> > putoffs(max_num_threads, std::set<int> ());
    for(int i = 0; i<start_times.size(); i++){
        start_times[i] = mx - start_times[i];
        if(start_times[i] < 1){
            clusters[i] = i;
            frontier.push_back(i);
            tie_breakers[i] = frontier_elem(i, start_times[i]);
        }else{
            putoffs[i%max_num_threads].insert(i);
        }
    }
    omp_lock_t *vtx_locks = (omp_lock_t *) malloc(sizeof(omp_lock_t) * input_graph.size());
    for(int i = 0; i<input_graph.size(); i++)
        omp_init_lock(vtx_locks+i);
    int num_rounds = 1;
    double tt = 0;
    while(!frontier.empty()){
        std::vector<std::vector<int> > next_frontier_pieces(max_num_threads);
        /*
        for(auto x : putoffs)
            for(int y : x)
                printf("%d ", y);
        printf("\n");
        */
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
        double t1 = CycleTimer::currentSeconds();
        #pragma omp parallel num_threads(max_num_threads)
        {
        int t = omp_get_thread_num();
        for(auto x = putoffs[t].begin();
                x != putoffs[t].end();){
            int v = *x;
            if(start_times[v] < num_rounds+1){
                if(clusters[v] == -1 && (tie_breakers[v].cluster == -1
                        || tie_breakers[v].tie_breaker > start_times[v] - num_rounds)){
                    tie_breakers[v] = frontier_elem(v, start_times[v] - num_rounds);
                    clusters[v] = v;
                    frontier.push_back(v);
                }
                auto y = x; x++;
                putoffs[t].erase(y);
            }else if(tie_breakers[v].cluster != -1){
                auto y = x; x++;
                putoffs[t].erase(y);
            }else{
                x++;
            }
        }
        }
        double t2 = CycleTimer::currentSeconds();
        tt += t2-t1;
        for(std::vector<int> piece : next_frontier_pieces){
            for(int v : piece){
                if(clusters[v] == -1){
                    clusters[v] = tie_breakers[v].cluster;
                    frontier.push_back(v);
                }
            }
        }
        num_rounds++;
    }
    free(vtx_locks);
    printf("%lf sec\n", tt);
}
