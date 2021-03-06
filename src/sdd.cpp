#include <vector>
#include <queue>
#include <omp.h>
#include <cstddef>
#include <set>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "sdd.h"


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
                for (int i = 0; i<input_graph[current_frontier[v]].size(); i++) {
                    int x = input_graph[current_frontier[v]][i];
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
                for(int i = 0; i<next_frontier.size(); i++)
                    clusters[next_frontier[i]] = -1;
                break;
            }
            num_internal += num_outs;
            current_dist++;
            current_frontier = next_frontier;
            for(int i = 0; i<next_frontier.size(); i++)
                dist[next_frontier[i]] = current_dist;
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


void millerPengXuLDD(graph &input_graph, std::vector<int> &clusters, double beta,
        int max_num_threads){
    clusters = std::vector<int> (input_graph.size(), -1);
    std::vector<double> start_times(input_graph.size());
    std::vector<frontier_elem> tie_breakers (input_graph.size());
    srand(time(0));
    std::vector<unsigned int> seeds (max_num_threads, 0);
    for(int i = 0; i<max_num_threads; i++)
        seeds[i] = rand();
    #pragma parallel for schedule(dynamic) num_threads(max_num_threads)
    for(int i = 0; i<start_times.size(); i++){
        double f = ((double) rand_r(&seeds[omp_get_thread_num()])) / RAND_MAX;
        start_times[i] = log(f) / -beta;
    }
    double mx = 0;
    for(int i = 0; i<start_times.size(); i++){
        if(mx < start_times[i]){
            mx = start_times[i];
        }
    }
    printf("%lf\n", mx);
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
    #pragma omp parallel for
    for(int i = 0; i<input_graph.size(); i++)
        omp_init_lock(vtx_locks+i);
    int num_rounds = 1;
    int remaining = 0;
    while(!frontier.empty() || remaining > 0){
        std::vector<std::vector<int> > next_frontier_pieces(max_num_threads);
        #pragma omp parallel for schedule(dynamic, 5) num_threads(max_num_threads)
        for(int i = 0; i<frontier.size(); i++){
            int t = omp_get_thread_num();
            frontier_elem z = tie_breakers[frontier[i]];
            for(int j = 0; j<input_graph[frontier[i]].size(); j++){
                int v = input_graph[frontier[i]][j];
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
        for(int j = 0; j<next_frontier_pieces.size(); j++){
            for(int i = 0; i<next_frontier_pieces[j].size(); i++){
                int v = next_frontier_pieces[j][i];
                if(clusters[v] == -1){
                    clusters[v] = tie_breakers[v].cluster;
                    frontier.push_back(v);
                }
            }
        }
        remaining = 0;
        for(int i = 0; i<putoffs.size(); i++)
          remaining += putoffs[i].size();
        num_rounds++;
    }
    printf("%d\n", num_rounds);
    free(vtx_locks);
}
