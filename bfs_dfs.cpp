#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

/* ================ SEQUENTIAL BFS ================= */
void sequentialBFS(vector<vector<int>> &adj, int V, int source) {
    vector<bool> visited(V, false);
    queue<int> q;
    visited[source] = true;
    q.push(source);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
}

/* ================ SEQUENTIAL DFS ================= */
void sequentialDFSUtil(vector<vector<int>> &adj, int u, vector<bool> &visited) {
    visited[u] = true;
    for (int v : adj[u]) {
        if (!visited[v])
            sequentialDFSUtil(adj, v, visited);
    }
}

void sequentialDFS(vector<vector<int>> &adj, int V, int source) {
    vector<bool> visited(V, false);
    sequentialDFSUtil(adj, source, visited);
}

/* ================ PARALLEL BFS ================= */
void parallelBFS(vector<vector<int>> &adj, int V, int source) {
    vector<int> visited(V, 0);
    vector<int> frontier, next_frontier;
    visited[source] = true;
    frontier.push_back(source);
    while (!frontier.empty()) {
        next_frontier.clear();
        #pragma omp parallel
        {
            vector<int> local_next;
            #pragma omp for nowait
            for (int i = 0; i < (int)frontier.size(); i++) {
                int u = frontier[i];
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        if (__sync_bool_compare_and_swap(&visited[v], 0, 1)) {
                            local_next.push_back(v);
                        }
                    }
                }
            }
            #pragma omp critical
            next_frontier.insert(next_frontier.end(),
                                local_next.begin(),
                                local_next.end());
        }
        frontier.swap(next_frontier);
    }
}

/* ================ PARALLEL DFS ================= */
void parallelDFSUtil(vector<vector<int>> &adj, int u, vector<int> &visited, int depth = 0) {
    #pragma omp taskgroup
    {
        for (int v : adj[u]) {
            if (__sync_bool_compare_and_swap(&visited[v], 0, 1)) {
                if (depth < 4) {
                    #pragma omp task
                    parallelDFSUtil(adj, v, visited, depth + 1);
                } else {
                    parallelDFSUtil(adj, v, visited, depth + 1);
                }
            }
        }
    }
}

void parallelDFS(vector<vector<int>> &adj, int V, int source) {
    vector<int> visited(V, 0);
    visited[source] = 1;
    #pragma omp parallel
    {
        #pragma omp single
        parallelDFSUtil(adj, source, visited, 0);
    }
}

/* ================== MAIN ================== */
int main() {
    int V = 10000;
    int source = 0;
    vector<vector<int>> adj(V);

    srand(42);
    int E = 50000;
    cout << "Generating random graph with " << V << " vertices and " << E << " edges...\n";
    for(int i = 0; i < E; i++){
        int u = rand() % V;
        int v = rand() % V;
        if(u != v){
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
    cout << "Graph generated. Starting traversals...\n\n";

    double start, end;

    // Sequential BFS
    start = omp_get_wtime();
    sequentialBFS(adj, V, source);
    end = omp_get_wtime();
    cout << "Sequential BFS Time: " << end - start << " seconds\n";

    // Parallel BFS
    start = omp_get_wtime();
    parallelBFS(adj, V, source);
    end = omp_get_wtime();
    cout << "Parallel BFS Time:   " << end - start << " seconds\n";

    // Sequential DFS
    start = omp_get_wtime();
    sequentialDFS(adj, V, source);
    end = omp_get_wtime();
    cout << "Sequential DFS Time: " << end - start << " seconds\n";

    // Parallel DFS
    start = omp_get_wtime();
    parallelDFS(adj, V, source);
    end = omp_get_wtime();
    cout << "Parallel DFS Time:   " << end - start << " seconds\n";

    return 0;
}