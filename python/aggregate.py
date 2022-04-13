# %%

import pandas as pd
import argparse

# %%

parser = argparse.ArgumentParser()
parser.add_argument('--folder', type=str, default="",
                    help='an integer for the accumulator')
parser.add_argument('--in_file', type=str, default="results.csv",
                    help='an integer for the accumulator')
parser.add_argument('--out_file', type=str, default="aggregated.csv",
                    help='an integer for the accumulator')

parser.add_argument('--runs', type=int, runs=6,
                    help='an integer for the accumulator')
parser.add_argument('--num_models', type=int, num_models=5,
                    help='an integer for the accumulator')

args = parser.parse_args()

in_file = args.in_file
out_file = args.out_file
folder = args.folder
runs = args.runs
num_models = args.num_models

# %%

df = pd.read_csv(f"{folder}results/runs/{in_file}")

runs = 0
num_models = 5

# %%
# Determine which graphs have had enough runs to use for training

to_keep = []
for name in df["Name"].unique():
    if len(df[df["Name"] == name]) >= runs*num_models:
        to_keep.append(name)
    else:
        print(
            f"Removing: {name} => not enough runs: {len(df[df['Name'] == name])}")

df = df[df["Name"].isin(to_keep)]
print(len(df))

# %%
# Remove unneeded info and duplicates

graph_names = df["Name"].unique()
algorithm_names = df["Algorithm"].unique()

df_info = df[['Name', 'Algorithm', 'Structure', 'Model', 'TrimMoment',
              'TrimType', 'Num_components', 'Num_1', 'Node_avg', 'Node_max', ]]
df_info = df_info.drop_duplicates(subset=["Name", "Algorithm"])

# %%

# Aggregate the runs of a specific model on a specific graph

res = []
for graph in graph_names:
    df_graph = df[df["Name"] == graph]
    for alg in algorithm_names:
        s = df_graph[df_graph["Algorithm"] == alg]

        try:
            res.append([graph, alg, s.iloc[0]["Date"],
                        s['Time'].mean(), s['Time'].std()])
        except:
            print(f"EXCEPT: {graph}, {alg}")
            res.append([graph, alg, 0, 0, 0])

df_agg = pd.DataFrame(
    res, columns=["Name", "Algorithm", "Date", "Time", "std"])

# %%

df_merged = pd.merge(df_agg, df_info, on=["Name", "Algorithm"])
# %%

df_merged.to_csv(f"{folder}results/aggregated/{out_file}")
