/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */

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
    map<InstInfo, list <pair <InstInfo, int>>> l; // adjacency list
public:
    void add_edge(InstInfo node, InstInfo neighbour, int distance) {
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
            InstInfo node = p.first;
            list <pair <InstInfo, int>> neighbour = p.second;
 
            cout << "Neighbors of: " << node.opcode << " are:\n";
 
            for(auto nbr : neighbour)
            {
                InstInfo dest = nbr.first;
                int distance = nbr.second;
 
                cout << "Neighbour: " << dest.opcode << " " << " Distance: "<< distance << endl;
            }
            cout << endl;
        }
    }
};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    Graph g;
    InstInfo entry, exit;
    g.add_edge(entry, progTrace[0], 0);
    int i;
    for(i = 0; i < numOfInsts - 1 ; i++){
        g.add_edge(progTrace[i], progTrace[i+1], 0);

    }
    g.add_edge(progTrace[i], exit, 0);
    // g.print_graph();
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


