# %%

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

    def __init__(self, N=8):
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
    def __init__(self, X, Y):
        self.X = X
        self.Y = Y

    def __len__(self):
        return self.X.shape[0]

    def __getitem__(self, index: int):
        return self.X[index, :], self.Y[index, :]


def split_df(df, ratio=0.7):
    train_len = round(len(df) * ratio)

    train_points = np.random.choice(len(df), train_len, replace=False)
    test_points = [x for x in range(len(df)) if x not in train_points]

    return df.iloc[train_points], df.iloc[test_points]


def getLabels(times):
    Y = np.zeros_like(times.values)
    Y[np.arange(len(times)), times.values.argmin(1)] = 1
    Y = torch.FloatTensor(Y)

    return Y


def calcAcc(output, labels):
    size = output.shape[0] * output.shape[1]
    correct = (output.round() == labels).sum()
    return correct.float() / size


def calcRatio(output):
    return output.round()[:, 0].sum() / output.shape[0]


def timeLoss(output, timeIncrease):
    return (output * timeIncrease).sum() / output.shape[0]


def save_net(net, name):
    sm = torch.jit.script(net)
    sm.save(f"../../AI_models/{name}.pt")


def print_results(name, net, X, Y):
    out = net(X)
    criterion = nn.MSELoss(reduce=False)
    mse_loss = criterion(out, Y).mean(axis=1)
    mse_loss = mse_loss.mean()
    acc = calcAcc(out, Y)
    ratio = calcRatio(out)
    print(f"{name} =>  MSEloss: {mse_loss.item():.3f}, acc: {acc:.3f}, ratio: {ratio:.3f}")
    print()


def make_X_Y(df, in_columns, label_columns):

    X = df[in_columns]
    X = torch.tensor(np.array(X)).float()

    Y = getLabels(df[label_columns])

    return X, Y


def train_model(net, X_train, Y_train, X_test, Y_test):

    train_dataset = GraphData(X_train, Y_train)
    train_dataloader = DataLoader(train_dataset, batch_size=64, shuffle=True)

    criterion = nn.MSELoss(reduce=False)

    optimizer = optim.Adam(net.parameters())
    scheduler = optim.lr_scheduler.ReduceLROnPlateau(
        optimizer, patience=100, verbose=True)

    for i in range(5000):
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
