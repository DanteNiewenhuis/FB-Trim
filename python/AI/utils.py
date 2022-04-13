# %%

from typing import Tuple
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.nn.modules.loss import MSELoss
import torch.optim as optim

import pandas as pd
import numpy as np

from torch.utils.data import Dataset
from torch.utils.data import DataLoader

# %%


class Net(nn.Module):
    """Network used for FB-AI-Trim

    Args:
        nn (int): Size of the initial layer. 
                  The size of the other layers are based on the initial layer.
    """

    def __init__(self, N: int = 8):
        super(Net, self).__init__()
        self.features = nn.Sequential(
            nn.Linear(N, 2*N),
            nn.Dropout(0.2),
            nn.ReLU(),
            nn.Linear(2*N, 4*N),
            nn.Dropout(0.2),
            nn.ReLU(),
            nn.Linear(4*N, 2*N),
            nn.Dropout(0.2),
            nn.ReLU(),
            nn.Linear(2*N, 2),
        )

    def forward(self, x):
        x = self.features(x)

        return x.softmax(dim=1)


class GraphData(Dataset):
    """Simple dataset used for getting batches of graphs

    """

    def __init__(self, X, Y):
        self.X = X
        self.Y = Y

    def __len__(self):
        return self.X.shape[0]

    def __getitem__(self, index: int):
        return self.X[index, :], self.Y[index, :]


def split_df(df: pd.DataFrame, ratio: float = 0.7) -> Tuple[pd.DataFrame, pd.DataFrame]:
    """Split data into a training and testing set

    Args:
        df (pd.DataFrame): A dataset of graphs
        ratio (float, optional): The ratio of training graphs. 
                                 Defaults to 0.7.

    Returns:
        Tuple[pd.DataFrame, pd.DataFrame]: Training set, Test set
    """

    train_len = round(len(df) * ratio)

    train_points = np.random.choice(len(df), train_len, replace=False)
    test_points = [x for x in range(len(df)) if x not in train_points]

    return df.iloc[train_points], df.iloc[test_points]


def getLabels(times: pd.DataFrame) -> torch.FloatTensor:
    """Create a boolean tensor based on the label columns

    Args:
        times (pd.DataFrame): dataframe consisting of two time columns

    Returns:
        torch.FloatTensor: Boolean labels for training
    """
    Y = np.zeros_like(times.values)
    Y[np.arange(len(times)), times.values.argmin(1)] = 1
    Y = torch.FloatTensor(Y)

    return Y


def calcAcc(output, labels):
    """Calculate the accuracy of the output
    """
    size = output.shape[0] * output.shape[1]
    correct = (output.round() == labels).sum()
    return correct.float() / size


def calcRatio(output):
    """ Calculate the ratio of the output is labeled False
    """
    return output.round()[:, 0].sum() / output.shape[0]


def timeLoss(output, timeIncrease):
    """Calculate the time increase of the output.
    """
    return (output * timeIncrease).sum() / output.shape[0]


def save_net(net, name):
    sm = torch.jit.script(net)
    sm.save(f"../../AI_models/{name}.pt")


def print_results(name, net, X, Y):
    """Print the current results of the network
    """
    out = net(X)
    criterion = nn.MSELoss(reduce=False)
    mse_loss = criterion(out, Y).mean(axis=1)
    mse_loss = mse_loss.mean()
    acc = calcAcc(out, Y)
    ratio = calcRatio(out)
    print(f"{name} =>  MSEloss: {mse_loss.item():.3f}, acc: {acc:.3f}, ratio: {ratio:.3f}")
    print()


def make_X_Y(df, in_columns, label_columns):
    """Split the given DataFrame into input (X) and labels (Y)
    """

    X = df[in_columns]
    X = torch.tensor(np.array(X)).float()

    Y = getLabels(df[label_columns])

    return X, Y


def train_model(net, X_train, Y_train, X_test, Y_test, iterations: int = 5000):
    """Train a new network for 5000 iterations. 
    Every 100 iterations, results are printed
    """

    train_dataset = GraphData(X_train, Y_train)
    train_dataloader = DataLoader(train_dataset, batch_size=64, shuffle=True)

    criterion = nn.MSELoss(reduce=False)

    optimizer = optim.Adam(net.parameters())
    scheduler = optim.lr_scheduler.ReduceLROnPlateau(
        optimizer, patience=100, verbose=True)

    for i in range(iterations):
        losses = []
        for batch in train_dataloader:
            X, Y = batch
            net.train()
            optimizer.zero_grad()

            out = net(X)
            loss = criterion(out, Y).mean(axis=1)

            loss = loss.mean()
            loss.backward()
            losses.append(loss.item())

            optimizer.step()

        mean_loss = sum(losses) / len(losses)
        scheduler.step(mean_loss)

        if (i % 100) == 0:
            net.eval()
            print(f"Results after {i} iterations: ")
            print_results("Train", net, X_train, Y_train)
            print_results("Test", net, X_test, Y_test)

# %%
