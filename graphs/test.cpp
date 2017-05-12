#include <stdio.h>
#include <vector>
using namespace std;
vector<vector<int> > G;
int main(){
  FILE *f = fopen("PL.txt", "r");
  int n, m;
  fscanf(f, "%d %d", &n, &m);
  G = vector<vector<int> > (n,vector<int> (0));
  for(int i = 0; i<m; i++){
    int a, b; fscanf(f, "%d %d", &a, &b);
    G[a].push_back(b); G[b].push_back(a);
  }
  vector<int> visited(n, 0);
  int num_components = 0;
  for(int i = 0; i<n; i++){
    if(!visited[i]){
      num_components++;
      visited[i] = 1;
      vector<int> D(1, i);
      while(!D.empty()){
        int x = D.back(); D.pop_back();
        for(int i = 0; i<G[x].size(); i++){
          int v = G[x][i];
          if(!visited[v]){
            visited[v] = 1;
            D.push_back(v);
          }
        }
      }
    }
  }
  printf("%d\n", num_components);
}
