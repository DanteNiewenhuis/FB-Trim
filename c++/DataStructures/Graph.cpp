#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

#include "../Utils.hpp"
#include "Graph.hpp"

vector<int> getNames(string& file_name)
{
    string line, temp;
    int from, to;
    stringstream ss;

    vector<int> nodes;

    ifstream read_file(file_name, std::ifstream::in);
    if (read_file.is_open())
    {
        while (getline(read_file, line))
        {
            ss.str(line);
            ss >> temp;

            // skip line if it is a comment
            if (temp == "%")
            {
                ss.str(std::string());
                ss.clear();
                continue;
            }

            size_t found = line.find(",");
            if (found != string::npos)
            {
                break;
            }

            from = atoi(temp.c_str());
            ss >> to;

            nodes.push_back(from);
            nodes.push_back(to);

            // clear everything that was left of the line from the stringstream.
            ss.str("");
            ss.clear();
        }
        read_file.close();
    }

    unordered_set<int> s(nodes.begin(), nodes.end());
    vector<int> no_dupes(s.begin(), s.end());
    reverse(no_dupes.begin(), no_dupes.end());

    return no_dupes;
}

/*
    General function for reading an edgelist file.
    The file is read line for line.
    If the line is non empty, it is added to the graph.
*/
void Graph::readFile(string& file_name, int to_skip = 0)
{
    string line, temp;
    int from, to;
    stringstream ss;

    int counter = 0;
    vector<int> nodes_names = getNames(file_name);
    int n = nodes_names.size();
    if (n == 0)
    {
        throw invalid_argument("Could not find any vertices in this graph!");
    }
    setInitial(nodes_names);

    ifstream read_file(file_name, std::ifstream::in);
    if (read_file.is_open())
    {
        while (getline(read_file, line))
        {
            if (counter < to_skip)
            {
                counter++;
                continue;
            }
            ss.str(line);
            ss >> temp;

            // skip line if it is a comment
            if (temp == "%")
            {
                ss.str(std::string());
                ss.clear();
                continue;
            }
            from = atoi(temp.c_str());
            ss >> to;

            addEdge(from - 1, to - 1);

            // clear everything that was left of the line from the stringstream.
            ss.str("");
            ss.clear();
        }
        read_file.close();
    }
}