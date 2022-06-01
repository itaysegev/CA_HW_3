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
#include <list>

using namespace std;

class Graph
{
    map<int, list <pair <int, int>>> l; // adjacency list
public:
    void add_edge(int node, int neighbour, int distance) {
        l[node].push_back(make_pair(neighbour, distance));
    }
 
    void print_graph()
    {
        // now we will iterate over all the keys in the map
        // then we will print the linked list of neighbors
        // associated with these nodes
        for(auto p : l)
        {
            // iterate over all the neighbors of this particular node
            int node = p.first;
            list <pair <int, int>> neighbour = p.second;
 
            cout << "Neighbors of: " << node << " are:\n";
 
            for(auto nbr : neighbour)
            {
                int dest = nbr.first;
                int distance = nbr.second;
 
                cout << "Neighbour: " << dest << " " << " Distance: "<< distance << endl;
            }
            cout << endl;
        }
    }
};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    Graph* g = new Graph;
    map<int, int> reg_dict; // reg_dict[reg] = last_write_op by index 
    map<int, bool> no_other_dep;
    int entry = -1;
    int exit = -2;
    int i;
    for(i = 0; i < MAX_REG; i++) {
        reg_dict[i] = NO_WRITE_OP;
        no_other_dep[i] = true; // for exit edge
    }
    /// initialize dict with no write op
    for(i = 0; i < numOfInsts ; i++){
        bool no_dep = true;
        if(reg_dict[progTrace[i].src1Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src1Idx];
            (*g).add_edge(i, last_write_op, opsLatency[i]);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(reg_dict[progTrace[i].src2Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src2Idx];
            (*g).add_edge(i, last_write_op, opsLatency[i]);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(no_dep) { // for entry edge
            (*g).add_edge(i, entry, opsLatency[i]);
        }
        reg_dict[progTrace[i].dstIdx] = i; //update dict[dst_reg] last write op 
    }
    for(i = 0; i < numOfInsts ; i++){
        if(no_other_dep[i]) {
            (*g).add_edge(exit, i, 0);
        }
    }
    return &g;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    cout << "SSSS" << endl;
    Graph* g = (Graph*)ctx;
    (*g).print_graph();
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    Graph* g = (Graph*)ctx;
    (*g).print_graph();
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


