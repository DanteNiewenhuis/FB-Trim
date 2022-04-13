# %%

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

from graph_utils import *

# %%
################################################################################
# Load data
################################################################################


base_folder = ".."

df = pd.read_csv(
    f"{base_folder}/results/combined/combined.csv", sep=";")

# %%
################################################################################
# Static Models only
################################################################################

models = ["Time_FB-N-B", "Time_FB-A-B",
          "Time_FB-I-B", "Time_FB-NI-B"]
names = ["Never", "Always", "Initial", "No Initial"]

print(f"Best")
plot_best(df, models, names, save=True, save_name="static")
print(f"Best and Worst")
plot_best_worst(df, models, names, save=True, save_name="static")
plot_best_worst_stacked(df, models, names, save=True, save_name="static")
print(f"Averages")
plot_average(df, models, names, save=True, save_name="static")
print(f"Head2Head")
print_head2head(df, models, names)
print(f"Rank")
plot_rank(df, models, names, save=True, save_name="static")
print(f"RI")
plot_RI(df, models, names, save=True, save_name="static")
increase_table(df, models, names)

# %%
################################################################################
# AI Model included
################################################################################

models_ai = ["Time_FB-N-B", "Time_FB-A-B",
             "Time_FB-I-B", "Time_FB-NI-B", "Time_FB-AI-B"]
names_ai = ["Never", "Always", "Initial", "No Initial", "AI"]

print(f"Best and Worst")
plot_best_worst(df, models_ai, names_ai, save=True, save_name="ai")
plot_best_worst_stacked(df, models_ai, names_ai, save=True, save_name="ai")
print(f"Averages")
plot_average(df, models_ai, names_ai, save=True, save_name="ai")
print(f"Head2Head")
print_head2head(df, models_ai, names_ai)
print(f"Rank")
plot_rank(df, models_ai, names_ai, save=True, save_name="ai")
print(f"RI")
plot_RI(df, models_ai, names_ai, save=True, save_name="ai")
increase_table(df, models_ai, names_ai)
