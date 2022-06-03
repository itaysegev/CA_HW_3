/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */
#define MAX_REG 32
#define NO_WRITE_OP -1
#include "dflow_calc.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include<set>
#include<algorithm>
#include <list>

using namespace std;

// class Graph {
//     map<int, list <pair <int, double>>> l; // adjacency list
//     map<int, list <pair <int, int>>> adj;
// public:
//     int size;
//     int entry_index;
//     int exit_index;
//     void add_edge(int node, int neighbour, double distance) {
//         adj[node].push_back(make_pair(neighbour, distance));
//         l[node].push_back(make_pair(neighbour, distance));
//     }
 
//     void print_graph()
//     {
//         // now we will iterate over all the keys in the map
//         // then we will print the linked list of neighbors
//         // associated with these nodes
//         for(auto p : adj)
//         {
//             // iterate over all the neighbors of this particular node
//             int node = p.first;
//             list <pair <int, int>> neighbour = p.second;
 
//             cout << "Neighbors of: " << node << " are:\n";
//             pair<int, int> deps;
            
//             for(auto nbr : neighbour)
//             {
//                 int dest = nbr.first;
//                 int distance = nbr.second;
 
//                 cout << "Neighbour: " << dest << " " << " Distance: "<< distance << endl;
//             }
//             cout << endl;
//         }
//     }
    
//     pair<int, int> getDeps(unsigned int theInst) {
//         pair<int, int> deps;
//         deps.first = -1;
//         deps.second = -1;
//         for(auto p : l) {
//             int node = p.first;
//             if(node == theInst) {
//                 list <pair <int, double>> neighbour = p.second;
//                 for(auto nbr : neighbour) { // iterate over all the neighbors of this particular node
//                     int dest = nbr.first;
//                     double distance = nbr.second;
//                     int clean_distance = nbr.second;
//                     if(distance - clean_distance > 0.1) {
//                         deps.second = dest;
//                     }
//                     else {
//                         deps.first = dest;
//                     }
//                 }
//                 return deps;
//             }
//         }
//     }

//     void dijkstra(Graph g, int *dist, int *prev, int start){
//         for(int u = 0; u < g.size; u++) {
//             dist[u] = 9999;   //set as infinity
//             prev[u] = -1;    //undefined previous
//         }
//         dist[start] = 0;   //distance of start is 0
//         set<int> S;       //create empty set S
//         list<int> Q;

//         for(int u = 0; u < g.size; u++) {
//             Q.push_back(u);    //add each node into queue
//         }
//         while(!Q.empty()) {
//             list<int> :: iterator i;
//             i = min_element(Q.begin(), Q.end());
//             int u = *i; //the minimum element from queue
//             cout << u << endl;
//             Q.remove(u); 
//             S.insert(u); //add u in the set
//             list<pair<int, int>> :: iterator it;
//             for(it = g.adj[u].begin(); it != g.adj[u].end();it++) {
//                 if((dist[u]+(it->second)) < dist[it->first]) { //relax (u,v)
//                     dist[it->first] = (dist[u]+(it->second));
//                     prev[it->first] = u;
//                 }
//             }
//         }
//     }
// };

typedef struct nodes {
   int dest;
   int cost;
   double cost_by_src;
}node;

class Graph {
   list<node> *adjList;
   private:
      void showList(int src, list<node> lt) {
        list<node> :: iterator i;
        node tempNode;

        for(i = lt.begin(); i != lt.end(); i++) {
            tempNode = *i;
            cout << "(" << src << ")---("<<tempNode.dest << "|"<<tempNode.cost<<") ";
        }
        cout << endl;
      }
   public:
    int n;
    int entry_index;
    int exit_index;
    Graph() {
        n = 0;
    }

    Graph(int nodeCount) {
        n = nodeCount;
        entry_index = n - 1;
        exit_index = n;
        adjList = new list<node>[n];
    }

    void addEdge(int source, int dest, double cost) {
        node newNode;
        newNode.dest = dest;
        newNode.cost = (int)cost; //convert to int
        newNode.cost_by_src = cost;
        adjList[source].push_back(newNode);
    }

    void displayEdges() {
        for(int i = 0; i<n; i++) {
            list<node> tempList = adjList[i];
            showList(i, tempList);
        }
    }
    pair<int, int> getDeps(unsigned int theInst) {
        pair<int, int> deps;
        deps.first = -1;
        deps.second = -1;
        list<node> lt = adjList[theInst];
        list<node> :: iterator i;
        for(i = lt.begin(); i != lt.end(); i++) {
            if((*i).cost_by_src - (*i).cost > 0.1) {
                deps.second = (*i).dest;
            }
            else {
                deps.first = (*i).dest;
            } 
        }
        return deps; 
    }
    friend void dijkstra(Graph g, int *dist, int *prev, int start);
};

void dijkstra(Graph g, int *dist, int *prev, int start) {
   int n = g.n;

   for(int u = 0; u<n; u++) {
      dist[u] = 9999;   //set as infinity
      prev[u] = -1;    //undefined previous
   }

   dist[start] = 0;   //distance of start is 0
   set<int> S;       //create empty set S
   list<int> Q;

   for(int u = 0; u<n; u++) {
      Q.push_back(u);    //add each node into queue
   }
   while(!Q.empty()) {
       list<int> :: iterator i;
       i = min_element(Q.begin(), Q.end());
       int u = *i; //the minimum element from queue
       Q.remove(u);
       S.insert(u); //add u in the set
       list<node> :: iterator it;
       for(it = g.adjList[u].begin(); it != g.adjList[u].end();it++) {
            if((dist[u]+(it->cost)) < dist[it->dest]) { //relax (u,v)
                dist[it->dest] = (dist[u]+(it->cost));
                prev[it->dest] = u;
            }
       }
   }
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    Graph* g = new Graph(numOfInsts + 2);
    map<int, int> reg_dict; // reg_dict[reg] = last_write_op by index 
    map<int, bool> no_other_dep;
    int entry = g->entry_index;
    int exit = g->exit_index;
    int i;
    for(i = 0; i < MAX_REG; i++) {
        reg_dict[i] = NO_WRITE_OP;
        no_other_dep[i] = true; // for exit edge
    }
    // (*g).entry_index = entry;
    // (*g).exit_index = exit;
    // (*g).size = numOfInsts + 2;
    /// initialize dict with no write op
    for(i = 0; i < numOfInsts ; i++){
        bool no_dep = true;
        if(reg_dict[progTrace[i].src1Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src1Idx];
            (*g).addEdge(i, last_write_op, opsLatency[i] + 0.1);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(reg_dict[progTrace[i].src2Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src2Idx];
            (*g).addEdge(i, last_write_op, opsLatency[i] + 0.2);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(no_dep) { // for entry edge
            (*g).addEdge(i, entry, opsLatency[i]);
        }
        reg_dict[progTrace[i].dstIdx] = i; //update dict[dst_reg] last write op 
    }
    for(i = 0; i < numOfInsts ; i++){
        if(no_other_dep[i]) {
            (*g).addEdge(exit, i, 0);
        }
    }
    return g;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    // Graph g = *(Graph*)ctx;
    int n = 11;
    // Graph g(n);
    // int dist[g.n], prev[g.n];
    // int start = 0;
     // g.print_graph();
    // g.addEdge(0, 10, 1);
    // g.addEdge(1, 10, 1);
    // g.addEdge(2, 0, 1);
    // g.addEdge(3, 0, 4);
    // g.addEdge(4, 0, 2);
    // g.addEdge(4, 1, 2);
    // g.addEdge(5, 3, 7);
    // g.addEdge(5, 1, 7);
    // g.addEdge(6, 2, 0);
    // g.addEdge(6, 3, 0);
    // g.addEdge(7, 0, 0);
    // g.addEdge(7, 3, 0);
    // g.addEdge(8, 6, 0);
    // g.addEdge(8, 5, 0);
    // g.addEdge(9, 8, 0);
    // g.addEdge(9, 7, 0);
    // g.addEdge(11, 9, 0);
    // g.addEdge(11, 4, 0);
    // dijkstra(g, dist, prev, start);
    // g.displayEdges();
    // for(int i = 0; i<g.n; i++) {
    //     if(i != start) {
    //         cout<<start<<" to "<<i<<", Cost: "<<dist[i]<<" Previous: "<<prev[i]<<endl;
    //     }
    // }
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    Graph g = *(Graph*)ctx;
    if (theInst < 0 ){
        return -1;
    }
    g.displayEdges();
    pair<int, int> deps = g.getDeps(theInst);
    *src1DepInst = deps.first;
    *src2DepInst = deps.second;
    return 0;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


