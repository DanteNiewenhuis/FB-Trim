#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "../AI/AINetwork.hpp"
#include "../DataStructures/AdjacencyMatrix.hpp"
#include "../Utils.hpp"

using namespace std;

#ifndef _FB_H
#define _FB_H

class FB
{
   private:
    string graph_name, log_layer_file, log_trim_file;
    vector<vector<int>> res;
    AINetwork *net;

    // variables used for early stopping
    chrono::high_resolution_clock::time_point start;
    // const int max_depth = 20, max_time = 300000;
    // const int max_depth = 20, max_time = 1800000;
    const int max_depth = 20, max_time = 3600000;
    bool stopLoop();

    // trim type
    TrimType trim_type;
    TrimMoment trim_moment;
    double total_overhead = 0.0;

    // variables used for splitting graphs.
    int sub_count = 1, min_size = 400;

    // pushing to results
    void pushToResults(vector<int>);
    void pushToResultsVec(vector<vector<int>>);

    // logging functions
    void logLayer(int, int, vector<int>, vector<int>, vector<int>, vector<int>,
                  string);
    void logTrim(double time, int initial_n, int new_n, int loops);
    void logAI(double time, bool prediction, int depth);

    // Trimming functions
    AdjacencyMatrix *trimGraph(AdjacencyMatrix *g);
    AdjacencyMatrix *trimAI(AdjacencyMatrix *g, int depth);

    // Pivot methods
    int getPivotBoth(AdjacencyMatrix *g);
    int getPivotIn(AdjacencyMatrix *g);
    int getPivotOut(AdjacencyMatrix *g);

    // FB steps
    tuple<vector<int>, vector<int>, vector<int>, vector<int>> combineFWD_BWD(
        vector<int> FWD, vector<int> BWD, int n);
    void newThread(AdjacencyMatrix *g, int depth, string t);
    void next(AdjacencyMatrix *g, int, vector<int> prec, vector<int> succ,
              vector<int> rem);
    void execute(AdjacencyMatrix *g, int, string);

   public:
    FB() { res = vector<vector<int>>(); };
    FB(string name, TrimMoment moment, TrimType type, string layer_log,
       string trim_log, AINetwork *network, int max_time)
        : graph_name(name),
          trim_moment(moment),
          trim_type(type),
          log_layer_file(layer_log),
          log_trim_file(trim_log),
          net(network),
          max_time(max_time)
    {
        cout << "max time is: " << max_time << endl;
        res = vector<vector<int>>();

        start = chrono::high_resolution_clock::now();
    };

    double get_overhead() { return total_overhead; };
    vector<vector<int>> getComponents(AdjacencyMatrix *);
};

#endif
