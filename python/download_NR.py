
# %%

import pandas as pd
from download_utils import download_NR_from_series
import os

import argparse

# %%
################################################################################
# Read info
################################################################################


# base_folder = ".."

# df_konect = pd.read_csv(
#     f"{base_folder}/data/KONECT_data.csv", sep=";", index_col=0)
# df_konect = df_konect[df_konect["directed"]]
# df_nr = pd.read_csv(f"{base_folder}/data/NR_data.csv", sep=";", index_col=0)
# df_nr = df_nr.drop_duplicates(subset="Name")

# %%
################################################################################
# Choose the range of graph size
################################################################################

parser = argparse.ArgumentParser()
parser.add_argument('--lowV', type=int, default=500,
                    help='Lowest number of vertices that need to be present')
parser.add_argument('--highV', type=int, default=10_000,
                    help='Highest number of vertices that need to be present')
parser.add_argument('--lowE', type=int, default=500,
                    help='Lowest number of edges that need to be present')
parser.add_argument('--highE', type=int, default=50_000,
                    help='Highest number of edges that need to be present')
parser.add_argument('--max_downloads', type=int, default=20,
                    help='Maximum number of graphs that are downloaded if available')
parser.add_argument('--base_folder', type=str, default="",
                    help='string needed to get the base folder')

args = parser.parse_args()


lowV = args.lowV
highV = args.highV
lowE = args.lowE
highE = args.highE
max_downloads = args.max_downloads


base_folder = "../"
base_folder = ""
df_nr = pd.read_csv(f"{base_folder}data/NR_data.csv", sep=";", index_col=0)
df_nr = df_nr.drop_duplicates(subset="Name")

clamped_df_nr = df_nr[(df_nr["n"] >= lowV) & (df_nr["n"] < highV)]

clamped_df_nr = clamped_df_nr[(clamped_df_nr["m"] >= lowE) & (
    clamped_df_nr["m"] < highE)]

print(f"Found {len(clamped_df_nr)} graphs to download")

# %%
################################################################################
# Download graphs
################################################################################

if not os.path.exists(f"{base_folder}data/NR_{lowV}-{highV}"):
    os.mkdir(f"{base_folder}data/NR_{lowV}-{highV}")

for i in range(len(clamped_df_nr)):
    if (i >= max_downloads):
        break
    print(f"Downloading graph: {clamped_df_nr.iloc[i]['Name']}")
    try:
        download_NR_from_series(
            clamped_df_nr.iloc[i], f"{base_folder}data/NR_{lowV}-{highV}")
    except:
        print("ERROR")
