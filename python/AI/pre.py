# %%

from re import X
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim

import pandas as pd
import numpy as np
from numpy.core.numeric import ones_like

from utils import *

# %%

###############################
# Loading data
###############################

base_folder = "../../"

df = pd.read_csv(
    f"{base_folder}/results/combined/combined.csv", sep=";")


# %%

###############################
# Defining columns, and split the data
###############################

df_train, df_test = split_df(df)
in_columns = ["min_in", "median_in_2", "median_in_4", "median_in_6",
              "min_out", "median_out_2", "median_out_4", "median_out_6"]

label_columns = ["Time_FB-N-B", "Time_FB-I-B"]

X_train, Y_train = make_X_Y(df_train, in_columns, label_columns)
X_test, Y_test = make_X_Y(df_test, in_columns, label_columns)

# %%

###############################
# Train a new model
###############################

net3 = Net(X_train.shape[1], 0.2)
train_model(net3, X_train, Y_train,
            X_test, Y_test)

# %%

###############################
# Save the model as a jit script
###############################

net3.eval()
sm = torch.jit.script(net3)
sm.save(f"{base_folder}AI_models/pre3_both.pt")
