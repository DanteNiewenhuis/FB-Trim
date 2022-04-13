# FB-AI-Trim: Efficient Trimming for Strongly Connected Components Calculation

This is the code for FB-AI-Trim.
The code is written in three languages:

- C++ for all interactions with the graphs.
- Python for result analysis, model training, and graph downloading
- Bash for scripting purposes.

## Building

The requirements for the project are as follows:
The C++ code is written using C++ 14. If you want to build the project,
you need cmake 3.15 and gcc 9.4.0. Pytorch for C++ is required to build and run the code. The installation of PyTorch for C++ is explained at "https://pytorch.org/cppdocs/installing.html".
To make it work you might the paths to the libtorch folder inside CMAKELISTS.txt.

All python code is written using Python 3.7.6. 
The requred packages are shown below with the version we have used

- For analysis and training:
  - PyTorch 1.5.0
  - Numpy 1.18.1
  - Pandas 1.1.5
  - Matplotlib 3.1.3
- For Downloading:
  - wget 3.2
  - BeautifulSoup 4.8.2
  - tqdm 4.42.1
  - urllib3 1.25.8

Other versions of these packages might also work.

## Downloading graphs

Graphs can be downloaded using the download_NR.py and download_KONECT.py .
A user can provide a range of vertices and edges.
All graphs that satisfy this range are then downloaded and placed in folders.

## Processing a graph

There are two ways to process a graph: main and info.

### main

main is used to get the strongly connected components of a given graph. 
When executed the main executable needs a path to a graph, and a moment of trimming. 
For example:

  ./executables/main "path_to_graph" -AI

The moment of trimming can be provided using the following flags:

- "-I": initial
- "-NI": not initial
- "-A": Always
- "-NO": Never
- "-AI": AI trimming

There are some extra flags that can be added:

- "-log" : results are logged to a csv
- "-NR" : needs to be added when using a graph gathered from the NR dataset
- "-k" : use the kosaraju algorithm to process the graph
- Flags used to change the type of trimming:
  - "-In": Only incoming edges
  - "-Out": Only outgoing edges
  - "-Both": Only Incoming edges

### info

Info is used to get topological information from a graph.
This is called in the following way:

 ./executables/info "path_to_graph" "dataset_name"

## Processing a whole dataset

The best method of processing a whole dataset is to create a bash script.
An example of such a bash script is provided in bash/processing_konect.sh

## Aggregating data

All graphs that are processed are logged in results/runs/results.csv
The next step is to aggregate and combine this data. 
This is done by first calling python/aggregate.py and then python/combine.py.

## Analysis and training

The combined data can now be used to analyse the results and train a new AI model. 
The graphs created in the paper can be made using python/result_analysis.py.

Tools to train a new AI model are defined in python/AI/utils.