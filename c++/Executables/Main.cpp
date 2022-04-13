#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

// My files
#include "../AI/AINetwork.hpp"
#include "../Algorithms/FB.hpp"
#include "../Algorithms/Kosaraju.hpp"
#include "../DataStructures/AdjacencyMatrix.hpp"
#include "../Utils.hpp"

using namespace std;

bool FLAG_LOGGING = false;
bool FLAG_NR = false;

DataType FLAG_DATATYPE = DataType::matrix;
ModelType FLAG_MODELTYPE = ModelType::DQ;
TrimType FLAG_TRIMTYPE = TrimType::Both;
TrimMoment FLAG_TRIMMOMENT = TrimMoment::Never;
double overhead = 0.0;
int max_time = 5;

/*
    Handle the given flags.
*/
void checkFlags(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (string(argv[i]) == "-m") FLAG_DATATYPE = DataType::matrix;
        if (string(argv[i]) == "-l") FLAG_DATATYPE = DataType::list;

        if (string(argv[i]) == "-log") FLAG_LOGGING = true;
        if (string(argv[i]) == "-NR") FLAG_NR = true;

        if (string(argv[i]) == "-k") FLAG_MODELTYPE = ModelType::kosaraju;

        // initialize trim moment
        if (string(argv[i]) == "-I") FLAG_TRIMMOMENT = TrimMoment::Initial;
        if (string(argv[i]) == "-NI") FLAG_TRIMMOMENT = TrimMoment::NotInitial;
        if (string(argv[i]) == "-A") FLAG_TRIMMOMENT = TrimMoment::Always;
        if (string(argv[i]) == "-NO") FLAG_TRIMMOMENT = TrimMoment::Never;
        if (string(argv[i]) == "-AI") FLAG_TRIMMOMENT = TrimMoment::AI;

        // Initialize trimming type
        if (string(argv[i]) == "-In") FLAG_TRIMTYPE = TrimType::In;
        if (string(argv[i]) == "-Out") FLAG_TRIMTYPE = TrimType::Out;
        if (string(argv[i]) == "-Both") FLAG_TRIMTYPE = TrimType::Both;
    }
}

void makeInitialFiles(string name, string log_layer_file, string log_trim_file)
{
    ofstream myFile;
    myFile.open(log_layer_file, ios::trunc);
    myFile << "depth,t,n,prec,succ,rem,scc" << endl;
    myFile.close();

    ifstream f(log_trim_file.c_str());
    if (!f.good())
    {
        ofstream myFile;
        myFile.open(log_trim_file, ios::trunc);
        myFile << "Name,time,initial_n,new_n,loops" << endl;
        myFile.close();
    }

    if (FLAG_TRIMMOMENT == TrimMoment::AI)
    {
        cout << "create csv: " << FLAG_TRIMTYPE << endl;
        myFile;
        switch (FLAG_TRIMTYPE)
        {
            case TrimType::Both:
                myFile.open("results/AI/Both/" + name + ".csv", ios::trunc);
                break;
            case TrimType::In:
                myFile.open("results/AI/In/" + name + ".csv", ios::trunc);
                break;
            case TrimType::Out:
                myFile.open("results/AI/Out/" + name + ".csv", ios::trunc);
                break;
        }
        myFile << "depth,time,prediction" << endl;
        myFile.close();
    }
}

// Execute the SCC algorithm and log the results.
vector<vector<int>> executeSCC(AdjacencyMatrix* g, string name,
                               string log_trim_file, AINetwork* net)
{
    string log_layer_file = "results/layerlogs/" + name + ".csv";
    makeInitialFiles(name, log_layer_file, log_trim_file);

    switch (FLAG_MODELTYPE)
    {
        case ModelType::kosaraju:
        {
            cout << "KOSARAJU!!!" << endl;
            Kosaraju kos = Kosaraju(g);
            return kos.execute();
        }
        case ModelType::DQ:
        {
            FB fb = FB(name, FLAG_TRIMMOMENT, FLAG_TRIMTYPE, log_layer_file,
                       log_trim_file, net, max_time * 60000);
            vector<vector<int>> comps = fb.getComponents(g);
            return comps;
        }
    }

    return {};
}

AdjacencyMatrix* createGraph(string file_path)
{
    AdjacencyMatrix* g;
    if (FLAG_NR)
    {
        g = new AdjacencyMatrix(file_path, 2);
    }
    else
    {
        g = new AdjacencyMatrix(file_path, 0);
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

    checkFlags(argc, argv);

    string name = argv[1];
    string file_path = "data/" + name + "/edges";

    ifstream f(file_path.c_str());
    if (!f.good())
    {
        cout << "Edges file not available" << endl;
        return 1;
    }

    if (argv[2][0] != '-')
    {
        max_time = atoi(argv[2]);
    }

    cout << "processing model => " << file_path << endl;
    cout << "trim method      => " << moment_names[FLAG_TRIMMOMENT] << "-"
         << trim_names[FLAG_TRIMTYPE] << endl;
    cout << "max time is: " << max_time << " minutes." << endl;

    // create graph and read the given file.
    cout << "create graph" << endl;
    AdjacencyMatrix* g = createGraph(file_path);

    AINetwork* net = new AINetwork(FLAG_TRIMTYPE);
    torch::NoGradGuard no_grad_guard;
    net->warmUp();

    auto t1 = chrono::high_resolution_clock::now();

    cout << "get components" << endl;
    // Get SSC from the given graph.
    vector<vector<int>> components =
        executeSCC(g, name, "results/trim_logs.csv", net);

    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = t2 - t1;

    cout << "execution time: " << ms_double.count() << endl;
    cout << "overhead: " << overhead << endl;

    if (FLAG_LOGGING)
    {
        cout << "log components" << endl;
        logComponents(components, ms_double.count(), name, FLAG_MODELTYPE,
                      FLAG_TRIMTYPE, FLAG_TRIMMOMENT, FLAG_DATATYPE, overhead);
    }
}