#include <vector>
#include <queue>

void sequentialLDD(std::vector<std::vector<int> >& input_graph,
                    std::vector<int>& clusters, double beta) {
    int current_cluster = 0;
    clusters = std::vector<int> (input_graph.size(), -1);
    for (int i = 0; i < input_graph.size(); i++) {
        std::vector<int> current_frontier;
        int num_internal = 0;
        std::vector<int> next_frontier;
        if (clusters[i] == -1) {
            current_frontier.push_back(i);
        }else
            continue;
        // build next_frontier: lots of repetitions: gives
        // all the outedges
        while (true) {
            for (int v : current_frontier) {
                for (int j = 0; j < input_graph[v].size(); j++) {
                    if (clusters[input_graph[v][j]] == -1) {
                        next_frontier.push_back(input_graph[v][j]);
                    }
                }
            }
            if ((double) next_frontier.size() <
                    beta * ((double) num_internal)) {
                break;
            }
            num_internal += next_frontier.size();
            current_frontier.clear();
            for (int i = 0; i < next_frontier.size(); i++) {
                if (clusters[next_frontier[i]] == -1) {
                    current_frontier.push_back(next_frontier[i]);
                    clusters[next_frontier[i]] = current_cluster;
                }
            }
            next_frontier.clear();
        }
        /* current_cluster finished, start next cluster */
        current_cluster++;
    }
    return;
}
