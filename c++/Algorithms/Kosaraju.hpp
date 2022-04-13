#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
#include "../DataStructures/AdjacencyMatrix.hpp"

#ifndef _Kosaraju_H
#define _Kosaraju_H

class Kosaraju
{
   private:
    // Objects
    Graph* graph;
    vector<int> seen_nodes;
    vector<int> node_order;
    map<int, vector<int>> timestamps;

    // variables
    int timestamp, n;
    bool order_is_set;

    // fuctions
    void traverseNode(int);

   public:
    Kosaraju(Graph* g) : graph(g){};

    void traverseKosaraju();
    vector<int> getComponent(int, vector<int>);
    vector<vector<int>> getComponents();

    vector<int> getNodeOrder() { return node_order; };
    int getNextNode(int);

    void printTimestamps()
    {
        for (int i = 0; i < n; i++)
        {
            cout << "i:" << i << " timestamp: " << timestamps[i][0] << ", "
                 << timestamps[i][1] << endl;
        }
    }

    void printSeenNodes()
    {
        cout << "getting order" << endl;
        for (int i = 0; i < seen_nodes.size(); i++)
        {
            cout << seen_nodes[i];
        }
    }
    void setNodeOrder();

    vector<vector<int>> execute();
};

#endif