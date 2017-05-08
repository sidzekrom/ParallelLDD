#include <vector>

#ifndef __SEQUENTIAL_H_INCLUDED__
#define __SEQUENTIAL_H_INCLUDED__

typedef std::vector<std::vector<int> > graph;

void sequentialLDD(graph& input_graph, std::vector<int>& clusters, double beta);

void millerPengXuLDD(graph &input_graph, std::vector<int> &clusters, double beta,
        int max_num_threads);
#endif
