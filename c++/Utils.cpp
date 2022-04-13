#include "Utils.hpp"

#include <math.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

// #include "DataStructures/AdjacencyMatrix.hpp"

using namespace std;

/*
    Get positional elements of a given vector.
*/
vector<int> getMediansSelect(vector<int> vec, int n)
{
    vector<int> goals;
    goals = {0, n / 4, n / 2, n / 4 * 3, n - 1};

    vector<int> out;
    for (auto &i : goals)
    {
        nth_element(vec.begin(), vec.begin() + i, vec.end());
        out.push_back(vec[i]);
    }

    return out;
}

string getCurrentTime()
{
    auto curr_time = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(curr_time);
    char *tt = ctime(&t);
    tt[24] = 0;

    return tt;
}

vector<int> getMediansSort(vector<int> vec, int n, int medians)
{
    vector<int> goals;
    if (medians == 3)
    {
        goals = {0, n / 4, n / 2, n / 4 * 3, n - 1};
    }
    if (medians == 7)
    {
        goals = {0,         n / 8,     n / 4,     n / 8 * 3, n / 2,
                 n / 8 * 5, n / 4 * 3, n / 8 * 7, n - 1};
    }

    sort(vec.begin(), vec.end());

    vector<int> out;
    for (auto &i : goals)
    {
        out.push_back(vec[i]);
    }

    return out;
}

void writeMedians(AdjacencyMatrix *g, string name, string file_name)
{
    auto t1 = chrono::high_resolution_clock::now();

    vector<int> medians_in = getMediansSort(g->getSumIn(), g->getN(), 7);
    vector<int> medians_out = getMediansSort(g->getSumOut(), g->getN(), 7);

    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = t2 - t1;

    ofstream myFile;
    myFile.open(file_name + ".csv", ios::app);
    myFile << name << "," << g->getN() << "," << g->getM();
    for (auto &i : medians_in)
    {
        myFile << "," << i;
    }
    for (auto &i : medians_out)
    {
        myFile << "," << i;
    }
    myFile << endl;
    myFile.close();
}

/*
    Print the components to the terminal on separate lines.
*/
void printComponents(vector<vector<int>> components)
{
    int counter = 1;
    for (auto &comp : components)
    {
        cout << "component " << counter << " len: " << comp.size() << " : {";
        for (auto &node : comp)
        {
            cout << node << " ";
        }
        cout << "}" << endl;
        counter++;
    }
}

/*
    Log various information about the components that can be used for analysis.
*/
void logComponents(vector<vector<int>> components, double execution_time,
                   string name, ModelType model, TrimType trim,
                   TrimMoment moment, DataType structure, double overhead)
{
    int s, max_nodes = 0, num_trivial = 0, num_components = components.size();
    float median_1, median_2, median_3, avg_nodes = 0.0;
    vector<int> comp_sizes;

    for (int i = 0; i < num_components; i++)
    {
        s = components[i].size();
        comp_sizes.push_back(s);
        avg_nodes += s;
        max_nodes = (s > max_nodes) ? s : max_nodes;
        num_trivial = (s == 1) ? num_trivial + 1 : num_trivial;
    }

    avg_nodes /= num_components;

    // Print results.
    cout << endl
         << "Execution time: " << execution_time << endl
         << "Number of components: " << num_components << endl
         << "Average nodes per component: " << avg_nodes << endl
         << "Number of components with a single node: " << num_trivial << endl
         << "Max nodes per component: " << max_nodes << endl;

    // Log the results to the given file.
    ofstream myFile;
    myFile.open("results/runs/results.csv", ios::app);
    myFile << name << "," << model_names[model] << "-" << moment_names[moment]
           << "-" << trim_names[trim] << "," << data_names[structure] << ","
           << model_names[model] << "," << moment_names[moment] << ","
           << trim_names[trim] << "," << getCurrentTime() << ","
           << execution_time << "," << num_components << "," << num_trivial
           << "," << avg_nodes << "," << max_nodes;

    myFile << endl;

    myFile.close();
}