# FB-AI-Trim: Efficient Trimming for Strongly Connected Components Calculation

FB-AI-Trim is an AI-enhanced implementation of the Forward-Backward algorithm (FB) to calculate strongly connected components (in directed graphs).

## Basic information

The most important aspects of the artifact are:  

    - Algorithms: FB-Trim with 5 variants (4 static and 1 AI-based). 
    - Code: C++, Python3, bash.
    - Compilation: g++ and CMake
    - Environment: Experiments were performed on Ubuntu and CentOS, 
        and confirmed to work on macOS Catalina.
    - Hardware: Machines with at regular CPUs and at least 8GB memory 
        are sufficient for executing all parts of the artifact.
    - How to use: Download and decompress the artifact check dependencies, compile the code, 
        run the various test scripts, and observe the results.
    - Time to prepare (approximately): less than 5 minutes,including downloading extra-dependencies.
    - Time needed to complete experiments (approximately)?:
        All experiments may take more ~20hours, depending on the extent of retraining and testing.
    - Publicly available?: Yes.

## Components and requirements

The artifact code includes:

    - The code for implementing the FB-AI-Trim algorithm.
    - The code for training the AI model.
    - Scripts for various graph operations, 
      from downloading graphs to results analysis and plotting.

The code is written using three languages:

    - C++ for all interactions with the graphs (mostly the algorithm itself). 
      The C++ code is written using C++ 14, and requires CMake 3.15 and gcc 9.4.0. 
    - Python for result analysis, model training, and graph downloading. 
      All python code is written using Python 3.7.6. 
      The required packages are shown below, with the version that has been used for the data in the article. 
      Pytorch for C++ is also required to build and run the code. 
      The installation of PyTorch for C++ is explained at https://pytorch.org/cppdocs/installing.html. 
    - Bash for scripting purposes.

### Datasets

The artifact includes the graphs discussed in this paper.More graphs can be downloaded using two provided scripts, one for graphs in the KONECT repository  and one for graphs in the network repository. For both these scripts, the user can provide a range of vertices and edges, and all graphs that satisfy this range are then downloaded and placed in folders.

## Installation

The code and data are accessible by uncompressing the artifact. The code needs to be recompiled using Cmake, once all dependencies, including PyTorch C++, are satisfied.

## Execution

There are two ways to process a graph: *info*, used to get topological information from a graph, and *main*, used to get the strongly connected components of a given graph using FB-Trim.

### Collecting graph information

To collect graph information, the *info* application is used with two arguments: the folder where the graph is stored and a name of a dataset, which is used to name and create an output file, in the *results* folder, where the statistics will be stored. This *info* application is used in the scripts that collect data across all graphs in a dataset.

### Calculating SCC

To execute FB-Trim on a graph, the *main* program is used.
When executed, it needs a path to a graph, and a flag indicating which trimming model to use. For example:
\begin{verbatim*
./executables/main "path_to_graph" -AI
\end{verbatim*

It is important to execute the FB-Trim from the main folder for proper logging of results.

The trim model can be provided using the following flags:

    - -I: initial
    - -NI: not initial
    - -A: Always
    - -NO: Never
    - -AI: AI trimming

Additional flags can be added:

    - -log : results are logged to a \verb|csv| file.
    - -NR : needs to be added when using a graph gathered from the 
    -       NR dataset due to slightly different file structure.
    - -k : use the kosaraju algorithm to process the graph.

A full example of how to run the analysis of the graphs used in this paper is included in *./bash/processing_konect.sh*.

## Results

All graphs that are processed are logged, and results are stored in the *results* folder. The next step is to aggregate and combine this data, operations supported by Python scripts. The combined data can now be used to analyse the results and train a new AI model.

The first step is to run *aggregate.py* located in the *Python* folder. This scripts aggregates averages the different runs of a model on a single graph and store them in *results/aggregated*. Next, the aggregated results can be combined by running *combine.py*. This creates a new csv in *results/combined* were each row is the aggregated results of all models on a single graph.

### Analysis

All functions used to analyse and plot results are provided in *graph\_utils.py*. Running *result\_analysis.py* creates the main plots shown in our paper, and save them to *results/images*.

### Training

Functions to train a new model are defined in *Python/AI/utils.py*. Running *Python/AI/pre.py* shows an example of how to train a new model. *Note*, trained networks should be saved exactly as defined in *Python/AI/pre.py*.
