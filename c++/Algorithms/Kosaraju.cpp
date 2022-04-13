#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
#include "../DataStructures/Graph.hpp"
#include "../Utils.hpp"
#include "Kosaraju.hpp"

void Kosaraju::traverseNode(int node)
{
    seen_nodes.push_back(node);
    timestamps[node][0] = timestamp++;

    vector<int> branches = graph->getOutEdges(node);

    for (auto &branch : branches)
    {
        if (find(seen_nodes.begin(), seen_nodes.end(), branch) ==
            seen_nodes.end())
        {
            traverseNode(branch);
        }
    }

    timestamps[node][1] = timestamp++;
}

/*
    Get the component including the given node.
    Using Kosaraju add all nodes reachable from the node,
*/
vector<int> Kosaraju::getComponent(int node, vector<int> scc)
{
    seen_nodes.push_back(node);

    // remove the node from node order.
    node_order.erase(remove(node_order.begin(), node_order.end(), node),
                     node_order.end());
    vector<int> branches = graph->getOutEdges(node);

    for (auto &branch : branches)
    {
        if (find(seen_nodes.begin(), seen_nodes.end(), branch) ==
            seen_nodes.end())
        {
            scc = getComponent(branch, scc);
        }
    }

    scc.push_back(graph->getNodeName(node));

    return scc;
}

/*
    Determine all components in the graph.
*/
vector<vector<int>> Kosaraju::getComponents()
{
    seen_nodes.clear();
    vector<vector<int>> comps;
    while (!(node_order.empty()))
    {
        comps.push_back(getComponent(node_order[0], vector<int>()));
    }

    return comps;
}

/*
    Traverse the graph using the Kosaraju algorithm.
    For each node note the first and second time the node is traversed.
*/
void Kosaraju::traverseKosaraju()
{
    n = graph->getN();

    // initialize all timestamps at 0.
    for (int i = 0; i < n; i++)
    {
        timestamps[i] = {0, 0};
        if (!order_is_set)
        {
            node_order.push_back(i);
        }
    }
    order_is_set = true;

    // reset the timestamp and seen_nodes;
    seen_nodes = vector<int>();
    timestamp = 1;

    int next_node = getNextNode(0);

    while (next_node != -1)
    {
        traverseNode(next_node);
        next_node = getNextNode(next_node);
    }
}

/*
    Get the next possible node to process based on the node order.
*/
int Kosaraju::getNextNode(int previous_node)
{
    for (int i = previous_node; i < n; i++)
    {
        if (find(seen_nodes.begin(), seen_nodes.end(), i) == seen_nodes.end())
        {
            return i;
        }
    }

    return -1;
}

/*
    Set the node order based on the timestamps.
    The order should be descending on the second timestamp
*/
void Kosaraju::setNodeOrder()
{
    node_order = vector<int>(n);
    for (int i = 0; i < n; i++)
    {
        node_order[i] = i;
    }

    sort(node_order.begin(), node_order.end(),
         [this](const int a, const int b) {
             return timestamps[a][1] > timestamps[b][1];
         });

    order_is_set = true;
}

vector<vector<int>> Kosaraju::execute()
{
    // // execute the initial Kosaraju and set the new node order
    traverseKosaraju();
    setNodeOrder();

    // // transpose the graph and execute the second
    graph->transpose();
    traverseKosaraju();

    // // return the strongly connected components.
    return getComponents();
}