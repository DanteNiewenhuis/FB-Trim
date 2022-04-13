#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifndef _Graph_H
#define _Graph_H

/*
    This class serves as the base for all other datatypes.
    It defines basic functionality that should be present in all datatypes.
    All virtual functions should be overridden by the datatypes.
*/
class Graph
{
   protected:
    int n = 0, m = 0;
    vector<int> sumIn;
    vector<int> sumOut;

   public:
    Graph(){};
    void readFile(string&, int);

    ////////////////////////////////////////////////////////////////////////
    ////////////// GETTERS
    ////////////////////////////////////////////////////////////////////////
    int getN() const { return n; };
    int getM() const { return m; };

    int getSumInNode(int node) const { return sumIn[node]; };
    int getSumOutNode(int node) const { return sumOut[node]; };
    vector<int> getSumIn() const { return sumIn; };
    vector<int> getSumOut() const { return sumOut; };

    ////////////////////////////////////////////////////////////////////////
    ////////////// OPERATORS
    ////////////////////////////////////////////////////////////////////////

    friend ostream& operator<<(ostream& out, const Graph& g)
    {
        return g.print(out);
    }

    ////////////////////////////////////////////////////////////////////////
    ////////////// VIRTUAL FUNCTIONS
    ////////////////////////////////////////////////////////////////////////

    virtual void transpose() { cout << "WARNING: base transpose" << endl; };

    virtual void setInitial(vector<int>) { cout << "WARNING: base setInitial"; }
    virtual void addEdge(int, int) { cout << "WARNING: base addEdge" << endl; };
    virtual bool getEdge(int, int) const
    {
        cout << "Warning: base getEdge" << endl;
        return false;
    };
    virtual vector<int> getOutEdges(int) const
    {
        cout << "WARNING: base getOutEdges" << endl;
        return vector<int>();
    };
    virtual vector<int> FWD(int) const
    {
        cout << "WARNING: base FWD" << endl;
        return vector<int>();
    }
    virtual vector<int> BWD(int) const
    {
        cout << "WARNING: base FWD" << endl;
        return vector<int>();
    }
    virtual vector<int> FWD_BFS(int) const
    {
        cout << "WARNING: base FWD" << endl;
        return vector<int>();
    }

    virtual ostream& print(ostream& out) const
    {
        out << "WARNING: printing base";
        return out;
    }

    virtual vector<int> getNodeNames()
    {
        cout << "WARNING: base FWD" << endl;
        return vector<int>();
    }
    virtual int getNodeName(int idx)
    {
        cout << "WARNING: base FWD" << endl;
        return 0;
    }

    virtual vector<int> idxToNames(vector<int> idx)
    {
        vector<int> names;

        for (auto& i : idx)
        {
            names.push_back(getNodeName(i));
        }

        return names;
    }
};

#endif