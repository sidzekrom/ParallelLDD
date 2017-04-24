#include <vector>
#include <queue>

void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    std::queue<int> bfs_queue;
    for (int i = 0; i < input_graph.size(); i++) {
        if (clusters[i] == -1) {
            bfs_queue.push(i);
        }
        int sparsity = 0;
        int num_internal = 0;
        std::vector<int> next_frontier;
        std::vector<int> current_frontier;
        // build next_frontier: lots of repetitions: gives
        // all the outedges
        while (!sparsity) {
            for (int i = 0; i < current_frontier.size(); i++) {
                for (int j = 0; j < input_graph[i].size(); j++) {
                    if (clusters[input_graph[i][j]] == -1) {
                        next_frontier.push_back(input_graph[i][j]);
                    }
                }
            }
            if ((double) next_frontier.size() <
                    beta * ((double) num_internal)) {
                sparsity = 1;
                continue;
            }
            current_frontier.clear();
            for (int i = 0; i < next_frontier.size(); i++) {
                if (clusters[next_frontier[i]] == -1) {
                    current_frontier.push_back(next_frontier[i]);
                    clusters[next_frontier[i]] = current_cluster;
                }
                num_internal++;
            }
            next_frontier.clear();
        }
        /* current_cluster finished, start next cluster */
        current_cluster++;
    }
    return;
}
