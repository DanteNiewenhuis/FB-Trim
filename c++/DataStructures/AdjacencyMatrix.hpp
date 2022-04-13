#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "Graph.hpp"

#ifndef _AdjacencyMatrix_H
#define _AdjacencyMatrix_H

class AdjacencyMatrix : public Graph
{
   private:
    vector<vector<bool>> matrix;
    vector<int> node_names;
    void addNode();
    map<int, vector<int>> timestamps;
    vector<int> seen_nodes;
    int timestamp;

   public:
    AdjacencyMatrix();
    AdjacencyMatrix(AdjacencyMatrix*, vector<int>);
    AdjacencyMatrix(string, int);

    void writeToFile(string);

    vector<int> getNodeNames() override { return node_names; }
    int getNodeName(int idx) override { return node_names[idx]; }

    void FWD_helper(int, vector<int>*) const;
    void BWD_helper(int, vector<int>*) const;

    void FWD_BFS_helper(vector<int>, vector<int>* seen_nodes) const;

    ////////////////////////////////////////////////////////////////////////
    ////////////// OVERRIDE
    ////////////////////////////////////////////////////////////////////////
    void transpose() override;
    void setInitial(vector<int>) override;
    void addEdge(int, int) override;

    bool getEdge(int, int) const override;
    vector<vector<bool>> getEdges() { return matrix; };
    vector<int> getOutEdges(int) const override;
    vector<int> getInEdges(int) const;
    vector<int> FWD(int) const override;
    vector<int> BWD(int) const override;
    vector<int> FWD_BFS(int) const override;

    virtual ostream& print(ostream& out) const override;
};

#endif