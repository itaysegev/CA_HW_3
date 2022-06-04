/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */
#define MAX_REG 32
#define NO_WRITE_OP -1
#define INT_MAX 99999
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
#include <vector>

using namespace std;


static int miniDist(int distance[], bool Tset[], int n) // finding minimum distance
{
    int minimum=INT_MAX;
    int ind;
              
    for(int k=0; k < n; k++) 
    {
        if(Tset[k]==false && distance[k] <= minimum)      
        {
            minimum=distance[k];
            ind=k;
        }
    }
    return ind;
}

typedef struct nodes {
   int dest;
   int cost;
   double cost_by_src;
}node;

class Graph {
   list<node> *adjList;
   vector<vector<int>> adjMatrix;
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
        entry_index = n - 2;
        exit_index = n - 1;
        adjList = new list<node>[n];
        for (int i = 0; i < n; i++) {
            vector<int> v;
            for(int j = 0; j < n; j++){
                v.push_back(1);
            }
            adjMatrix.push_back(v);
        }
    }
    void printMat() {
        for (int i = 0; i < n; i++) {
            cout << i << " ";
            for(int j = 0; j < n; j++){
               cout << adjMatrix[i][j] << " ";
            }
            cout << "\n" << endl;
        }
    }
    void addEdge(int source, int dest, double cost) {
        node newNode;
        newNode.dest = dest;
        newNode.cost = (int)cost; //convert to int
        newNode.cost_by_src = cost;
        adjList[source].push_back(newNode);
        adjMatrix[source][dest] = -1 * (int)cost;
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
            double dist = (*i).cost_by_src - (*i).cost; 
            if(dist > 0.2) {
                deps.second = (*i).dest;
            }
            else {
                deps.first = (*i).dest;
            } 
        }
        return deps; 
    }
    void DijkstraAlgo(int* distance, int src) {                             
        bool Tset[n];// boolean array to mark visited and unvisited for each node
        for(int k = 0; k < n; k++) {
            distance[k] = INT_MAX;
            Tset[k] = false;    
        }
        distance[src] = 0;   // Source vertex distance is set 0               
        for(int k = 0; k < n; k++) {
            int m=miniDist(distance, Tset, n); 
            Tset[m]=true;
            for(int k = 0; k < n; k++) {
                // updating the distance of neighbouring vertex
                if(!Tset[k] && adjMatrix[m][k] != 1 && distance[m]!=INT_MAX && distance[m] + adjMatrix[m][k] < distance[k]) {
                    distance[k] = distance[m] + adjMatrix[m][k];
                }
            }
        }
    }
};  

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
    for(i = 0; i < numOfInsts ; i++){
        bool no_dep = true;
        int opcode;
        if(reg_dict[progTrace[i].src1Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src1Idx];
            opcode = progTrace[last_write_op].opcode;
            (*g).addEdge(i, last_write_op, opsLatency[opcode] + 0.1);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(reg_dict[progTrace[i].src2Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src2Idx];
            opcode = progTrace[last_write_op].opcode;
            (*g).addEdge(i, last_write_op, opsLatency[opcode] + 0.3);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(no_dep) { // for entry edge
            (*g).addEdge(i, entry, 0);
        }
        reg_dict[progTrace[i].dstIdx] = i; //update dict[dst_reg] last write op 
    }
    for(i = 0; i < numOfInsts ; i++){
        int opcode = progTrace[i].opcode;
        if(no_other_dep[i]) {
            (*g).addEdge(exit, i, opsLatency[opcode]);
        }
    }
    return g;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    Graph g = *(Graph*)ctx;
    int dist[g.n];
    g.DijkstraAlgo(dist, theInst);
    return  (-1 * dist[g.entry_index]);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    Graph g = *(Graph*)ctx;
    if (theInst < 0 ){
        return -1;
    }
    pair<int, int> deps = g.getDeps(theInst);
    *src1DepInst = deps.first;
    *src2DepInst = deps.second;
    return 0;
}

int getProgDepth(ProgCtx ctx) {
    Graph g = *(Graph*)ctx;
    g.displayEdges();
    g.printMat();
    int exit = g.exit_index;
    return getInstDepth(ctx, exit);
}


