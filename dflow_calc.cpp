#define MAX_REG 32
#define NO_WRITE_OP -1
#define INT_MAX 99999
#include "dflow_calc.h"
#include <iostream>
#include <limits.h>
#include <list>
#include <stack>
#include <map>
#define NINF INT_MIN
using namespace std;
 
// Graph is represented using adjacency list. Every
// node of adjacency list contains vertex number of
// the vertex to which edge connects. It also
// contains weight of the edge
class AdjListNode {
    int v;
    int weight;
    double dist;
   
public:
    AdjListNode(int _v, double _w)
    {
        v = _v;
        weight =(int) _w;
        dist =  _w;

    }
    int getV() { return v; }
    int getWeight() { return weight; }
    double getDist() { return dist; }
};
   
// Class to represent a graph using adjacency list
// representation
class Graph {
    // Pointer to an array containing adjacency lists
    list<AdjListNode>* adj;
    // A function used by longestPath
    void topologicalSortUtil(int v, bool visited[],
                             stack<int>& Stack);
    void showList(int src, list<AdjListNode> lt);
public:
    int V; // No. of vertices' 
    int entry_index;
    int exit_index;
    Graph(int V); // Constructor
    Graph() {
        V = 0;
    }
    ~Graph(); // Destructor
    // function to add an edge to graph
    void addEdge(int u, int v, double weight);
    void displayEdges();
   
    // Finds longest distances from given source vertex
    void longestPath(int s, int dist[]);
    pair<int, int> getDeps(unsigned int theInst);
};
   
Graph::Graph(int V) // Constructor
{
    this->V = V;
    this->entry_index = V - 1;
    this->exit_index = V - 2;
    this->adj = new list<AdjListNode>[V];
}
 
Graph::~Graph() // Destructor
{
    delete [] adj;
}
void Graph::displayEdges() {
        for(int i = 0; i < V; i++) {
            list<AdjListNode> tempList = adj[i];
            showList(i, tempList);
        }
}

void Graph::showList(int src, list<AdjListNode> lt) {
     list<AdjListNode> :: iterator i;
     AdjListNode tempNode(0,0);
     for(i = lt.begin(); i != lt.end(); i++) {
         tempNode = *i;
         cout << "(" << src << ")---("<<tempNode.getV() << "|"<<tempNode.getWeight()<<") ";
     }
     cout << endl;
}
pair<int, int> Graph::getDeps(unsigned int theInst) {
        pair<int, int> deps;
        deps.first = -1;
        deps.second = -1;
        list<AdjListNode> lt = adj[theInst];
        list<AdjListNode> :: iterator i;
        for(i = lt.begin(); i != lt.end(); i++) {
            double dist = (*i).getDist() - (*i).getWeight(); 
            if(dist > 0.2) {
                deps.second = (*i).getV();
                if(deps.second == entry_index){
                    deps.second = -1;
                }
            }
            else {
                deps.first = (*i).getV();
                if(deps.first == entry_index){
                    deps.first = -1;
                }
                
            } 
        }
        return deps; 
}
 
 
void Graph::addEdge(int u, int v, double weight)
{
    AdjListNode node(v, weight);
    adj[u].push_back(node); // Add v to u's list
}
   
void Graph::topologicalSortUtil(int v, bool visited[],
                                stack<int>& Stack)
{
    // Mark the current node as visited
    visited[v] = true;
   
    // Recur for all the vertices adjacent to this vertex
    list<AdjListNode>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i) {
        AdjListNode node = *i;
        if (!visited[node.getV()])
            topologicalSortUtil(node.getV(), visited, Stack);
    }
   
    // Push current vertex to stack which stores topological
    // sort
    Stack.push(v);
}
   
// The function to find longest distances from a given vertex.
// It uses recursive topologicalSortUtil() to get topological
// sorting.
void Graph::longestPath(int s, int dist[])
{
    stack<int> Stack;
    // Mark all the vertices as not visited
    bool* visited = new bool[V];
    for (int i = 0; i < V; i++)
        visited[i] = false;
   
    // Call the recursive helper function to store Topological
    // Sort starting from all vertices one by one
    for (int i = 0; i < V; i++)
        if (visited[i] == false)
            topologicalSortUtil(i, visited, Stack);
   
    // Initialize distances to all vertices as infinite and
    // distance to source as 0
    for (int i = 0; i < V; i++)
        dist[i] = NINF;
    dist[s] = 0;
    // Process vertices in topological order
    while (Stack.empty() == false) {
        // Get the next vertex from topological order
        int u = Stack.top();
        Stack.pop();
   
        // Update distances of all adjacent vertices
        list<AdjListNode>::iterator i;
        if (dist[u] != NINF) {
            for (i = adj[u].begin(); i != adj[u].end(); ++i){
                if (dist[i->getV()] < dist[u] + i->getWeight())
                    dist[i->getV()] = dist[u] + i->getWeight();
            }
        }
        // if(dist[entry_index] != NINF) {
        //     cout << dist[entry_index] << endl; 
        // }
        // cout <<"A" << endl;
    }     
    delete [] visited;
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
    for(i = 0; i < numOfInsts; i++){
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
            no_other_dep[entry] = false;
        }
        reg_dict[progTrace[i].dstIdx] = i; //update dict[dst_reg] last write op 
    }
    for(i = 0; i < numOfInsts; i++){
        int opcode = progTrace[i].opcode;
        if(no_other_dep[i]) {
            (*g).addEdge(exit, i, opsLatency[opcode]);
        }
    }
    return g;
}

void freeProgCtx(ProgCtx ctx) {
    delete (Graph*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    Graph* g = (Graph*)ctx;
    if ((theInst < 0) || (theInst >= (*g).V) ){
        return -1;
    }
    int dist[(*g).V];
    (*g).longestPath(theInst, dist);
    return dist[(*g).entry_index];
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    Graph* g = (Graph*)ctx;
    if ((theInst < 0) || (theInst >= (*g).V) ){
        return -1;
    }
    pair<int, int> deps = (*g).getDeps(theInst);
    *src1DepInst = deps.first;
    *src2DepInst = deps.second;
    return 0;
}

int getProgDepth(ProgCtx ctx) {
    Graph* g = (Graph*)ctx;
    int exit = (*g).exit_index;
    int dist[(*g).V];
    (*g).longestPath(exit, dist);
    cout << getInstDepth(ctx, 3071) << endl;
    cout << getInstDepth(ctx, 1) << endl;
    cout << getInstDepth(ctx, 18) << endl;;
    cout << getInstDepth(ctx, 22) << endl;;

    (*g).displayEdges();
    int max_idx = exit;
    // for (int i = 0; i < (*g).V; i++){
    //     if (dist[i] > dist[max_idx]) {
    //         max_idx = i;
    //     }
    // }
    return dist[max_idx];
}
