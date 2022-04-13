#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "DataStructures/AdjacencyMatrix.hpp"

using namespace std;

#ifndef _Utils_H
#define _Utils_H

enum DataType
{
    matrix,
    list
};
enum ModelType
{
    kosaraju,
    DQ
};
enum TrimType
{
    Both,
    In,
    Out
};
enum TrimMoment
{
    Initial,
    NotInitial,
    Always,
    Never,
    AI
};

static const vector<string> data_names = {"matrix", "list"};
static const vector<string> model_names = {"kosaraju", "FB"};
static const vector<string> trim_names = {"B", "I", "O"};
static const vector<string> moment_names = {"I", "NI", "A", "N", "AI"};

template <typename T>
void print_vec(const vector<T> vec, string name, string delim = "\n")
{
    cout << name << ": " << endl;
    for (auto& i : vec)
    {
        cout << i << delim;
    }
    cout << endl;
}

string getCurrentTime();
void logComponents(vector<vector<int>> components, double execution_time,
                   string name, ModelType model, TrimType trim,
                   TrimMoment moment, DataType structure, double overhead);

void printComponents(vector<vector<int>> components);
void writeMedians(AdjacencyMatrix* g, string name, string file_path);

string getAlgorithmName(TrimType type, TrimMoment moment);
vector<int> getMediansSelect(vector<int> vec, int n);

vector<int> getMediansSort(vector<int> vec, int n);

#endif