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
    map<int, list <pair <int, double>>> l; // adjacency list
public:
    void add_edge(int node, int neighbour, double distance) {
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
            list <pair <int, double>> neighbour = p.second;
 
            cout << "Neighbors of: " << node << " are:\n";
            pair<int, double> deps;
            
            for(auto nbr : neighbour)
            {
                int dest = nbr.first;
                double distance = nbr.second;
 
                cout << "Neighbour: " << dest << " " << " Distance: "<< distance << endl;
            }
            cout << endl;
        }
    }
    
    pair<int, int> getDeps(unsigned int theInst) {
        pair<int, int> deps;
        deps.first = -1;
        deps.second = -1;
        for(auto p : l) {
            // iterate over all the neighbors of this particular node
            int node = p.first;
            if(node == theInst) {
                list <pair <int, double>> neighbour = p.second;
                for(auto nbr : neighbour) {
                    int dest = nbr.first;
                    double distance = nbr.second;
                    int clean_distance = nbr.second;
                    if(distance - clean_distance > 0.1) {
                        deps.second = dest;
                    }
                    else {
                        deps.first = dest;
                    }
                }
                return deps;
            }
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
            (*g).add_edge(i, last_write_op, opsLatency[i] + 0.1);
            no_other_dep[last_write_op] = false;
            no_dep = false;    
        }
        if(reg_dict[progTrace[i].src2Idx] != NO_WRITE_OP){
            int last_write_op = reg_dict[progTrace[i].src2Idx];
            (*g).add_edge(i, last_write_op, opsLatency[i] + 0.2);
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
    return g;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    Graph g = *(Graph*)ctx;
    g.print_graph();
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    Graph g = *(Graph*)ctx;
    pair<int, int> deps = g.getDeps(theInst);
    *src1DepInst = deps.first;
    *src2DepInst = deps.second;
    cout << theInst << endl;
    cout << *src1DepInst << endl;
    cout << *src2DepInst << endl;
    return 1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


