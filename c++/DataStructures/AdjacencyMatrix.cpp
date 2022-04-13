#include "AdjacencyMatrix.hpp"

#include <sys/stat.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../Utils.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
/////////// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

AdjacencyMatrix::AdjacencyMatrix(string file_path, int to_skip = 1)
{
    readFile(file_path, to_skip);
};

AdjacencyMatrix::AdjacencyMatrix(AdjacencyMatrix* base, vector<int> nodes)
{
    int base_n = base->getN();
    n = nodes.size();

    if (n >= base_n)
    {
        cout << "ERROR: too many nodes given" << endl;
        return;
    }

    // initialize
    matrix = vector<vector<bool>>(n, vector<bool>(n, 0));
    sumIn = vector<int>(n, 0);
    sumOut = vector<int>(n, 0);

    vector<int> base_names = base->getNodeNames();
    vector<vector<bool>> base_matrix = base->getEdges();
    bool edge;
    for (int i = 0; i < n; i++)
    {
        node_names.push_back(base->getNodeName(nodes[i]));

        for (int j = 0; j < n; j++)
        {
            if (base_matrix[nodes[i]][nodes[j]])
            {
                matrix[i][j] = true;
                // add the value to sumIn and sumOut
                sumOut[i] += 1;
                sumIn[j] += 1;

                m += 1;
            }
        }
    }
};

/*
    Tranpose the graph inplace.
*/
void AdjacencyMatrix::transpose()
{
    vector<vector<bool>> g = vector<vector<bool>>(n, vector<bool>(n, 0));

    for (int i = 0; i < n; i++)
    {
        vector<int> inEdges = getInEdges(i);
        for (int j = 0; j < n; j++)
        {
            if (find(inEdges.begin(), inEdges.end(), j) != inEdges.end())
            {
                g[i][j] = 1;
            }
        }
    }

    matrix = g;
}

void AdjacencyMatrix::setInitial(vector<int> names)
{
    int n = names.size();
    sumIn = vector<int>(n, 0);
    sumOut = vector<int>(n, 0);

    node_names = names;
    // vector<int> vec;
    for (int i = 0; i < n; i++)
    {
        matrix.push_back(vector<bool>(n, 0));
    }
}

void AdjacencyMatrix::addNode()
{
    vector<bool> new_vec(++n, false);
    matrix.push_back(new_vec);

    for (int i = 0; i < n - 1; i++)
    {
        matrix[i].push_back(0);
    }
}

/*
    Add a new edge to the graph.
    If either the from or the to node is new, add a new node.
*/
void AdjacencyMatrix::addEdge(int from, int to)
{
    if (from > n) addNode();
    if (to > n) addNode();

    // add the edge
    matrix[from][to] = true;
    sumOut[from] += 1;
    sumIn[to] += 1;
    m++;
}

/*
    return true if edge exists
*/
bool AdjacencyMatrix::getEdge(int from, int to) const
{
    return matrix[from][to];
}

/*
    return all edges from a node
*/
vector<int> AdjacencyMatrix::getOutEdges(int from) const
{
    vector<int> vec;
    for (int i = 0; i < n; i++)
    {
        if (matrix[from][i])
        {
            vec.push_back(i);
        }
    }

    return vec;
}

/*
    return all edges from a node
*/
vector<int> AdjacencyMatrix::getInEdges(int to) const
{
    vector<int> vec;
    for (int i = 0; i < n; i++)
    {
        if (matrix[i][to])
        {
            vec.push_back(i);
        }
    }

    return vec;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////// FWD
////////////////////////////////////////////////////////////////////////////////

void AdjacencyMatrix::FWD_helper(int node, vector<int>* seen_nodes) const
{
    if (!(find((*seen_nodes).begin(), (*seen_nodes).end(), node) ==
          (*seen_nodes).end()))
    {
        return;
    }

    seen_nodes->push_back(node);
    vector<int> temp = getOutEdges(node);

    for (auto& i : temp)
    {
        FWD_helper(i, seen_nodes);
    }
}

vector<int> AdjacencyMatrix::FWD(int from) const
{
    vector<int> seen_nodes;
    FWD_helper(from, &seen_nodes);

    return seen_nodes;
}

void AdjacencyMatrix::FWD_BFS_helper(vector<int> currentNodes,
                                     vector<int>* seen_nodes) const
{
    vector<int> next_nodes;
    vector<int> temp;
    for (auto& i : currentNodes)
    {
        if (!(find((*seen_nodes).begin(), (*seen_nodes).end(), i) ==
              (*seen_nodes).end()))
        {
            continue;
        }

        temp = getOutEdges(i);
        seen_nodes->push_back(i);
        for (auto& j : temp)
        {
            next_nodes.push_back(j);
        }
    }

    if (next_nodes.size() > 0) FWD_BFS_helper(next_nodes, seen_nodes);
}

vector<int> AdjacencyMatrix::FWD_BFS(int node) const
{
    vector<int> seen_nodes;
    FWD_BFS_helper(vector<int>(1, node), &seen_nodes);

    cout << "seen nodes " << node << " : ";
    for (auto& i : seen_nodes)
    {
        cout << i << ", ";
    }
    cout << endl;
    return seen_nodes;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////// BWD
////////////////////////////////////////////////////////////////////////////////

void AdjacencyMatrix::BWD_helper(int node, vector<int>* seen_nodes) const
{
    if (!(find((*seen_nodes).begin(), (*seen_nodes).end(), node) ==
          (*seen_nodes).end()))
    {
        return;
    }

    seen_nodes->push_back(node);
    vector<int> temp = getInEdges(node);

    for (auto& i : temp)
    {
        BWD_helper(i, seen_nodes);
    }
}

vector<int> AdjacencyMatrix::BWD(int from) const
{
    vector<int> seen_nodes;
    BWD_helper(from, &seen_nodes);

    return seen_nodes;
}

ostream& AdjacencyMatrix::print(ostream& os) const
{
    os << "  ";
    for (int i = 0; i < n; i++)
    {
        os << node_names[i];
    }
    os << endl;

    os << "  ";
    for (int i = 0; i < n; i++)
    {
        os << "-";
    }
    os << endl;

    for (int i = 0; i < n; i++)
    {
        os << node_names[i] << "|";
        for (int j = 0; j < n; j++)
        {
            os << matrix[i][j];
        }
        os << endl;
    }
    return os;
}

void AdjacencyMatrix::writeToFile(string foldername)
{
    cout << "creating: " << foldername << endl;
    int status = mkdir(foldername.c_str(), 0777);

    string filename = foldername + "/edges";
    ofstream myFile;
    myFile.open(filename, ios::out);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (matrix[i][j])
            {
                myFile << node_names[i] << " " << node_names[j] << endl;
            }
        }
    }

    myFile.close();
}
