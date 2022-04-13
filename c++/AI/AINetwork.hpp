#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

// My files
#include <torch/script.h>
#include <torch/torch.h>

#include "../DataStructures/AdjacencyMatrix.hpp"
#include "../Utils.hpp"

#ifndef _AINetwork_H
#define _AINetwork_H
class AINetwork
{
   private:
    torch::jit::script::Module module;
    TrimType trim_type;
    int median_size;
    bool loaded = false;

   public:
    AINetwork(TrimType t) : trim_type(t)
    {
        string file_path = "AI_models/pre3";

        // Pick model depending on the type of trimming used
        switch (trim_type)
        {
            case TrimType::Both:
                file_path += "_both.pt";
                break;
            case TrimType::In:
                file_path += "_in.pt";
                break;
            case TrimType::Out:
                file_path += "_out.pt";
                break;
        }
        cout << "ai_model: " << file_path << endl;

        ifstream f(file_path.c_str());
        if (f.good())
        {
            module = torch::jit::load(file_path);
            module.eval();
            loaded = true;
        }
        else
            cout << "AI model not found!!!!" << endl;
    };

    /*
        The model is warmed up by execting 20 forward passes.
        This is done because the first few forward passes are much slower
        than normal passes.
    */
    void warmUp()
    {
        std::vector<torch::jit::IValue> inputs;

        switch (trim_type)
        {
            case TrimType::Both:
                inputs.push_back(
                    torch::tensor({{1.0, 2.0, 3.0, 4.0, 1.0, 2.0, 3.0, 4.0}}));
                break;
            case TrimType::In:
                inputs.push_back(torch::tensor({{1.0, 2.0, 3.0, 4.0}}));
                break;
            case TrimType::Out:
                inputs.push_back(torch::tensor({{1.0, 2.0, 3.0, 4.0}}));
                break;
        }
        for (int i = 0; i < 20; i++)
        {
            module.forward(inputs);
        }
    }

    /*
        Returns input for the network based on the given graph
    */
    std::vector<torch::jit::IValue> getInputs(AdjacencyMatrix *g)
    {
        vector<int> sumIn = g->getSumIn();
        vector<int> sumOut = g->getSumOut();

        vector<int> medians_in = getMediansSelect(sumIn, g->getN());
        vector<int> medians_out = getMediansSelect(sumOut, g->getN());

        std::vector<torch::jit::IValue> inputs;

        switch (trim_type)
        {
            case TrimType::Both:
                inputs.push_back(torch::tensor(
                    {{(float)medians_in[0], (float)medians_in[1],
                      (float)medians_in[2], (float)medians_in[3],
                      (float)medians_out[0], (float)medians_out[1],
                      (float)medians_out[2], (float)medians_out[3]}}));
                break;
            case TrimType::In:
                inputs.push_back(torch::tensor(
                    {{(float)medians_in[0], (float)medians_in[1],
                      (float)medians_in[2], (float)medians_in[3]}}));
                break;
            case TrimType::Out:
                inputs.push_back(torch::tensor(
                    {{(float)medians_out[0], (float)medians_out[1],
                      (float)medians_out[2], (float)medians_out[3]}}));
                break;
        }

        return inputs;
    }

    /*
        The model prediction layer has size two. The first element of the final
        layer means not trimming, and the second means not trimming.
        Thus, if output[1] > output[0] the model will trim.
    */
    bool predict(AdjacencyMatrix *g)
    {
        std::vector<torch::jit::IValue> inputs;

        inputs = getInputs(g);

        at::Tensor output = module.forward(inputs).toTensor();

        if (output.argmax(1).item().toInt() == 1)
        {
            return true;
        }
        return false;
    }
};

#endif