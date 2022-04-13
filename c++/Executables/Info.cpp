#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// My files
#include "../DataStructures/AdjacencyMatrix.hpp"
#include "../Utils.hpp"

using namespace std;

AdjacencyMatrix* createGraph(string file_path)
{
    AdjacencyMatrix* g;
    bool nr = false;
    if (nr)
    {
        g = new AdjacencyMatrix(file_path, 2);
    }
    else
    {
        g = new AdjacencyMatrix(file_path, 1);
    }
    return g;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        cout << "Please provide a folder name" << endl;
        return 0;
    }

    string name = argv[1];
    string file_path = "data/" + name + "/edges";
    string median_name = argv[2];
    string median_path = "results/node_info/" + median_name;

    cout << "processing model: " << file_path << endl;

    ifstream f(file_path.c_str());
    if (!f.good())
    {
        cout << "File not available" << endl;
        return 1;
    }

    cout << "writing to:" << median_path << endl;

    // create graph and read the given file.
    AdjacencyMatrix* g = createGraph(file_path);

    writeMedians(g, name, median_path);
}