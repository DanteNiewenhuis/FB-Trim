# %%

import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

import argparse

# %%

parser = argparse.ArgumentParser()
parser.add_argument('--folder', type=str, default="",
                    help='an integer for the accumulator')
parser.add_argument('--in_file', type=str, default="aggregated.csv",
                    help='an integer for the accumulator')
parser.add_argument('--out_file', type=str, default="combined.csv",
                    help='an integer for the accumulator')

args = parser.parse_args()

in_file = args.in_file
out_file = args.out_file
folder = args.folder
# %%

folder = "../"

# %%

df = pd.read_csv(f"{folder}results/aggregated/{in_file}")


# %%

algorithms = list(df["Algorithm"].unique())

new_df = df[list(set(df.columns) - set(["Time", "Algorithm", "trim"]))
            ].drop_duplicates(subset=["Name"])

for algorithm in algorithms:
    new_df = pd.merge(new_df, df[df["Algorithm"] == algorithm][[
                      "Name", "Time"]], on="Name", how="outer")
    new_df = new_df.rename(columns={"Time": f"Time_{algorithm}"})


# %%

df_konect = pd.read_csv(
    f"{folder}results/node_info/KONECT.csv", sep=",", index_col=0)

df_nr = pd.read_csv(
    f"{folder}results/node_info/NR.csv", sep=",", index_col=0)

df_info = pd.concat([df_konect, df_nr])

# %%

df_merged = pd.merge(new_df, df_info, on="Name", how="left")


# %%
df_merged.to_csv(f"{folder}results/combined/{out_file}", sep=";")

# %%
