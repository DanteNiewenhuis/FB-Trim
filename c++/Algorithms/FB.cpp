#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

// My files
#include "../DataStructures/AdjacencyMatrix.hpp"
#include "../Utils.hpp"
#include "FB.hpp"
#include "Kosaraju.hpp"

using namespace std;

mutex vectLock;

/*
    Stop executing if too much time has passed
*/
bool FB::stopLoop()
{
    auto now = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = now - start;

    if (ms_double.count() > max_time)
    {
        cout << "MAX TIME REACHED!!!!!" << endl;
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/////////////// PUSHING TO RESULTS
////////////////////////////////////////////////////////////////////////////////

/*
    push new components to the results vector. Lock the vector before pushing
    so there is no conflict between threads.
*/
void FB::pushToResults(vector<int> vec)
{
    vectLock.lock();
    res.push_back(vec);
    vectLock.unlock();
}

/*
    Same function as above, but works for inserting a list of new components.
*/
void FB::pushToResultsVec(vector<vector<int>> vec)
{
    vectLock.lock();
    res.insert(res.end(), vec.begin(), vec.end());
    vectLock.unlock();
}

////////////////////////////////////////////////////////////////////////////////
/////////////// LOGGING
////////////////////////////////////////////////////////////////////////////////

/*
    Log layer information to the given output file.
*/
void FB::logLayer(int depth, int n, vector<int> prec, vector<int> succ,
                  vector<int> rem, vector<int> scc, string t)
{
    ofstream myFile;
    myFile.open(log_layer_file, ios::app);
    myFile << depth << "," << t << "," << n << "," << prec.size() << ","
           << succ.size() << "," << rem.size() << "," << scc.size() << endl;
    myFile.close();
}

/*
    Log layer information to the given output file.
*/
void FB::logAI(double time, bool prediction, int depth)
{
    ofstream myFile;
    switch (trim_type)
    {
        case TrimType::Both:
            myFile.open("results/AI/Both/" + graph_name + ".csv", ios::app);
            break;
        case TrimType::In:
            myFile.open("results/AI/In/" + graph_name + ".csv", ios::app);
            break;
        case TrimType::Out:
            myFile.open("results/AI/Out/" + graph_name + ".csv", ios::app);
            break;
    }
    myFile << depth << "," << time << "," << prediction << endl;
    myFile.close();

    total_overhead += time;
}

/*
    Log layer information to the given output file.
*/
void FB::logTrim(double time, int initial_n, int new_n, int loops)
{
    ofstream myFile;
    myFile.open(log_trim_file, ios::app);
    myFile << graph_name << "," << time << "," << initial_n << "," << new_n
           << "," << loops << endl;
    myFile.close();
}

////////////////////////////////////////////////////////////////////////////////
/////////////// TRIMMING
////////////////////////////////////////////////////////////////////////////////

/*
    Remove trivial nodes from the graph.
    This is done by deleting the all nodes that have either no incoming or
    outgoing nodes.
*/
AdjacencyMatrix *FB::trimGraph(AdjacencyMatrix *g)
{
    vector<int> sumIn, sumOut, to_keep;
    int n, counter = 0, initial_n = g->getN();
    auto t1 = chrono::high_resolution_clock::now();

    while (true)
    {
        counter++;
        n = g->getN();
        switch (trim_type)
        {
            case TrimType::Both:
                sumIn = g->getSumIn();
                sumOut = g->getSumOut();
                break;
            case TrimType::In:
                sumIn = g->getSumIn();
                break;
            case TrimType::Out:
                sumOut = g->getSumOut();
                break;
            default:
                return g;
        }

        to_keep.clear();

        // Create parallel implementation
        for (int i = 0; i < n; i++)
        {
            switch (trim_type)
            {
                case TrimType::Both:
                {
                    if ((sumIn[i] > 0) && (sumOut[i] > 0))
                        to_keep.push_back(i);
                    else
                        pushToResults(g->idxToNames({i}));
                    break;
                }
                case TrimType::In:
                {
                    if (sumIn[i] > 0)
                        to_keep.push_back(i);
                    else
                        pushToResults(g->idxToNames({i}));
                    break;
                }
                case TrimType::Out:
                {
                    if (sumOut[i] > 0)
                        to_keep.push_back(i);
                    else
                        pushToResults(g->idxToNames({i}));
                    break;
                }
            }
        }
        // stop if no nodes are removed.
        if (to_keep.size() == n) break;

        g = new AdjacencyMatrix(g, to_keep);
    }

    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = t2 - t1;

    logTrim(ms_double.count(), initial_n, g->getN(), counter);

    return g;
}

AdjacencyMatrix *FB::trimAI(AdjacencyMatrix *g, int depth)
{
    auto t1 = chrono::high_resolution_clock::now();

    bool prediction = net->predict(g);

    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = t2 - t1;

    logAI(ms_double.count(), prediction, depth);

    if (prediction) g = trimGraph(g);

    return g;
}

////////////////////////////////////////////////////////////////////////////////
/////////////// FB FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/*
    Split the nodes into four parts: precessors, successors, remaining and scc.
    This is done based on the forward and backward reach of a node.
*/
tuple<vector<int>, vector<int>, vector<int>, vector<int>> FB::combineFWD_BWD(
    vector<int> FWD, vector<int> BWD, int n)
{
    vector<int> prec, succ, rem, scc;

    for (int i = 0; i < n; i++)
    {
        if (!(find(FWD.begin(), FWD.end(), i) == FWD.end()))
        {
            if (!(find(BWD.begin(), BWD.end(), i) == BWD.end()))
            {
                scc.push_back(i);
            }
            else
            {
                succ.push_back(i);
            }
            continue;
        }
        if (!(find(BWD.begin(), BWD.end(), i) == BWD.end()))
        {
            prec.push_back(i);
            continue;
        }

        rem.push_back(i);
    }

    return make_tuple(prec, succ, rem, scc);
}

/*
    Create a new thread that will handle the given subgraph.
*/
void FB::newThread(AdjacencyMatrix *g, int depth, string t)
{
    execute(g, depth + 1, t);
}

/*
    Execute the next steps of the iteration based on the given subgraphs.
*/
void FB::next(AdjacencyMatrix *g, int depth, vector<int> prec, vector<int> succ,
              vector<int> rem)
{
    AdjacencyMatrix *g_prec;
    AdjacencyMatrix *g_succ;
    AdjacencyMatrix *g_rem;

    // make subgraphs
    if (!prec.empty()) g_prec = new AdjacencyMatrix(g, prec);
    if (!succ.empty()) g_succ = new AdjacencyMatrix(g, succ);
    if (!rem.empty()) g_rem = new AdjacencyMatrix(g, rem);

    // delete big graph

    delete (g);

    // process subgraphs

    vector<thread> threads;

    if (!prec.empty())
        threads.push_back(thread(&FB::newThread, this, g_prec, depth, "prec"));

    if (!succ.empty())
        threads.push_back(thread(&FB::newThread, this, g_succ, depth, "succ"));

    if (!rem.empty()) newThread(g_rem, depth, "rem");

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }
}

int FB::getPivotBoth(AdjacencyMatrix *g)
{
    vector<int> sumIn = g->getSumIn();
    vector<int> sumOut = g->getSumOut();
    int n = g->getN();

    int max_value = 0;
    vector<int> max_idxs = vector<int>();
    int value;
    for (int i = 0; i < n; i++)
    {
        value = sumIn[i] * sumOut[i];
        if (value == max_value)
        {
            max_idxs.push_back(i);
        }
        if (value > max_value)
        {
            max_value = value;
            max_idxs = {i};
        }
    }

    return max_idxs[rand() % max_idxs.size()];
}

int FB::getPivotIn(AdjacencyMatrix *g)
{
    vector<int> sumIn = g->getSumIn();
    int n = g->getN();

    int max_value = 0;
    vector<int> max_idxs = vector<int>();
    int value;
    for (int i = 0; i < n; i++)
    {
        if (sumIn[i] == max_value)
        {
            max_idxs.push_back(i);
        }
        if (sumIn[i] > max_value)
        {
            max_value = value;
            max_idxs = {i};
        }
    }

    return max_idxs[rand() % max_idxs.size()];
}

int FB::getPivotOut(AdjacencyMatrix *g)
{
    vector<int> sumOut = g->getSumOut();
    int n = g->getN();

    int max_value = 0;
    vector<int> max_idxs = vector<int>();
    int value;
    for (int i = 0; i < n; i++)
    {
        if (sumOut[i] == max_value)
        {
            max_idxs.push_back(i);
        }
        if (sumOut[i] > max_value)
        {
            max_value = value;
            max_idxs = {i};
        }
    }

    return max_idxs[rand() % max_idxs.size()];
}

/*
    Execute a single iteration of the FB algorithm.
*/
void FB::execute(AdjacencyMatrix *g, int depth, string t)
{
    // Stop de algorithm after 5 minutes.
    if (stopLoop()) return;

    switch (trim_moment)
    {
        case TrimMoment::Always:
            g = trimGraph(g);
            break;
        case TrimMoment::NotInitial:
            if (depth > 1) g = trimGraph(g);
            break;
        case TrimMoment::AI:
            g = trimAI(g, depth);
            break;
    }

    // Stop if no nodes are left.
    if (g->getN() == 0) return;

    // Continue using kosaraju when max depth is reached.
    if (depth >= max_depth)
    {
        Kosaraju kos = Kosaraju(g);
        pushToResultsVec(kos.execute());
        return;
    }

    // Pick random pivot
    srand(time(NULL));
    // int pivot = rand() % g->getN();

    int pivot;
    switch (trim_type)
    {
        case TrimType::Both:
            pivot = getPivotBoth(g);
            break;
        case TrimType::In:
            pivot = getPivotIn(g);
            break;
        case TrimType::Out:
            pivot = getPivotOut(g);
            break;
    }

    // Get forward and backward reach from pivot and sort the results
    vector<int> forward = g->FWD(pivot);
    vector<int> backward = g->BWD(pivot);

    // Determine the precesors, successors, remaining and components from the
    // forward and backward.
    vector<int> prec, succ, rem, scc;

    std::tie(prec, succ, rem, scc) =
        combineFWD_BWD(forward, backward, g->getN());
    logLayer(depth, g->getN(), prec, succ, rem, scc, t);

    pushToResults(g->idxToNames(scc));

    next(g, depth, prec, succ, rem);
}

vector<vector<int>> FB::getComponents(AdjacencyMatrix *g)
{
    if (trim_moment == TrimMoment::Initial)
    {
        g = trimGraph(g);
    }

    execute(g, 1, "base");

    return res;
}