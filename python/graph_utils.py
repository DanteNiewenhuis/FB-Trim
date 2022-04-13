import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

##################################
# Helper functions
##################################

font = {'family': 'normal',
        'size': 16}

plt.rc('font', **font)
label_size = 16


def get_models(df):
    models = []
    for c in df.columns:
        if ("Time" in c):
            models.append(c)

    return models


def add_best_worst(df, models):
    df["best"] = df[models].idxmin(1)
    df["worst"] = df[models].idxmax(1)

    df["best_time"] = df[models].min(1)
    df["worst_time"] = df[models].max(1)

    return df


def add_rank(df, models, model):
    ranking = []

    for j in range(len(df)):
        s = df.iloc[j][models]
        placement = 1
        for other_model in models:
            placement += s[model] > s[other_model]

        ranking.append(placement)

    df[f"rank_{model}"] = ranking
    return df

##################################
# Number of best and worst times per model
##################################


def plot_best(df, models, names=[], save=False, save_name="notname"):
    df = add_best_worst(df, models)

    if names == []:
        names = models

    y = []

    for model in models:
        y.append((df['best'] == model).sum())

    plt.ylabel("Count", fontdict=font)
    plt.bar(x=names, height=y)
    plt.xticks()

    m = np.max(y)
    x = np.arange(len(names))
    for i in range(len(y)):
        plt.text(x=x[i], y=y[i] + m/50, s=f"{y[i]:.0f}",
                 fontdict=dict(fontsize=12), ha='center')
    plt.ylim(0, m*1.2)

    if(save):
        plt.savefig(f"../results/images/best_{save_name}.png")
    plt.title("Best placements", fontdict=font)
    plt.show()


def plot_best_worst(df, models, names=[], save=False, save_name="notname"):
    df = add_best_worst(df, models)

    if names == []:
        names = models

    y_best = []
    y_worst = []

    l = len(df)

    for model in models:
        y_best.append((df['best'] == model).sum())
        y_worst.append((df['worst'] == model).sum())

    x = np.arange(len(models))

    ax = plt.subplot(111)
    ax.bar(x-0.2, y_best, width=0.4, color='g', align='center', label="best")
    ax.bar(x+0.2, y_worst, width=0.4, color='r', align='center', label="worst")
    ax.legend(ncol=3)
    ax.set_xticklabels(names)
    ax.set_xticks(np.arange(len(x)))
    plt.ylabel("Count", fontdict=font)
    plt.xticks(rotation=90)
    plt.tight_layout()

    if(save):
        plt.savefig(f"../results/images/best_worst_{save_name}.png")
    plt.title("Best and Worst placements", fontdict=font)
    plt.show()


def plot_best_worst_stacked(df, models, names=[], save=False, save_name="notname"):
    df = add_best_worst(df, models)

    if names == []:
        names = models

    y_best = []
    y_worst = []
    y_neutral = []

    l = len(df)

    for model in models:
        y_best.append((df['best'] == model).sum())
        y_worst.append((df['worst'] == model).sum())
        y_neutral.append(
            l - (df['best'] == model).sum() - (df['worst'] == model).sum())

    print(f"y_best: {y_best}")
    print(f"y_worst: {y_worst}")
    x = np.arange(len(models))

    worst_bot = np.array(y_best) + np.array(y_neutral)

    ax = plt.subplot(111)
    ax.bar(x, y_best, width=0.4, color='g', align='center', label="best")
    ax.bar(x, y_neutral, bottom=y_best, width=0.4,
           color='gray', align='center', label="neither")
    ax.bar(x, y_worst, bottom=worst_bot, width=0.4,
           color='r', align='center', label="worst")
    ax.legend()
    ax.set_xticklabels(names)
    ax.set_xticks(np.arange(len(x)))
    plt.ylabel("Count", fontdict=font)
    plt.xticks()
    plt.legend(bbox_to_anchor=(-0.05, 1.2), loc='upper left', ncol=3)

    x = np.arange(len(names))
    for i in range(len(y_best)):
        plt.text(x=x[i], y=0, s=f"{y_best[i]:.0f}",
                 fontdict=dict(fontsize=12), ha='center', va='bottom')

    for i in range(len(y_worst)):
        plt.text(x=x[i], y=len(df), s=f"{y_worst[i]:.0f}",
                 fontdict=dict(fontsize=12), ha='center', va='top')

    plt.tight_layout()

    if(save):
        plt.savefig(f"../results/images/best_worst_stacked_{save_name}.png")
    plt.title("Best and Worst placements", fontdict=font)
    plt.show()


##################################
# Average execution time
##################################

def plot_average(df, models, names=[], save=False, save_name="notname"):
    y = []

    if names == []:
        names = models

    for model in models:
        y.append((df[model]).mean())

    print(f"y: {y}")

    plt.ylabel("Miliseconds", fontdict=font)
    plt.bar(x=names, height=y)
    plt.xticks()

    m = np.max(y)
    x = np.arange(len(names))
    for i in range(len(y)):
        plt.text(x=x[i], y=y[i] + m/50, s=f"{y[i]:.0f}",
                 fontdict=dict(fontsize=12), ha='center')
    plt.ylim(0, m*1.2)

    plt.tight_layout()
    if(save):
        plt.savefig(f"../results/images/average_{save_name}.png")
    plt.title("Average Execution time", fontdict=font)
    plt.show()

##################################
# Stadard deviation between runs
##################################


def plot_std(df, models, names=[], save=False, save_name="notname"):
    y = []

    if names == []:
        names = models

    for model in models:
        y.append((df[model]).std())

    plt.ylabel("Miliseconds", fontdict=font)
    plt.bar(x=names, height=y)
    plt.xticks()
    plt.tight_layout()

    if(save):
        plt.savefig(f"../results/images/std_{save_name}.png")
    plt.title("Standard deviation", fontdict=font)
    plt.show()

##################################
# Placement
##################################


def plot_rank(df, models, names, save=False, save_name=""):
    print(f"Average Rank:")
    if names == []:
        names = models

    avgs = []
    for i in range(len(models)):
        model = models[i]
        model_placement = {i: 0 for i in range(1, len(models)+1)}

        for j in range(len(df)):
            s = df.iloc[j][models]
            placement = 1
            for other_model in models:
                placement += s[model] > s[other_model]

            model_placement[placement] += 1

        height = np.array(list(model_placement.values()))
        height = height / height.sum()
        plt.bar(x=model_placement.keys(), height=height)
        plt.xlabel("Rank")
        plt.ylabel("frequency", fontdict=font)

        m = np.max(height)
        x = np.arange(len(models))
        for j in range(len(height)):
            plt.text(x=x[j]+1, y=height[j] + m/50, s=f"{height[j]:.2f}",
                     fontdict=dict(fontsize=12), ha='center')

        plt.ylim([0, 0.6])
        plt.tight_layout()
        if save:
            print(f"saving: model: {names[i]}")
            print(f"names: {names}, i: {i}")
            plt.savefig(
                f"../results/images/Ranking_freq_{names[i]}_{save_name}.png")

        plt.title(names[i], fontdict=font)
        plt.show()
        placements = np.array(list(model_placement.keys()))
        freq = np.array(list(model_placement.values()))
        avgs.append((placements * freq).sum() / freq.sum())

    args = np.argsort(avgs)
    for i in args:
        print(f"{models[i]} ==> {avgs[i]:.2f}")

    m = np.max(avgs)
    x = np.arange(len(names))
    for i in range(len(avgs)):
        plt.text(x=x[i], y=avgs[i] + m/50, s=f"{avgs[i]:.2f}",
                 fontdict=dict(fontsize=12), ha='center')
    plt.ylim(0, m*1.2)

    plt.bar(x=names, height=avgs)
    plt.tight_layout()
    if save:
        plt.savefig(f"../results/images/Ranking_avg_{save_name}.png")

    plt.title("Average Ranking", fontdict=font)
    plt.show()


##################################
# Head to Head
##################################


def print_head2head(df, models, names):
    res = np.zeros((len(models), len(models)))

    s = f"\\begin{{tabular}}{{|c|"
    for name in names:
        s += f"|c"
    s += f"|}}\hline\n"
    s += f"\\diagbox{{\\textbf{{a}}}}{{\\textbf{{b}}}}"
    for name in names:
        s += f" & \\textbf{{{name}}}"
    s += " \\\\\hline\hline\n"
    for i in range(len(models)):
        s += f"\\textbf{{{names[i]}}}"
        for j in range(len(models)):
            modelA = models[i]
            modelB = models[j]
            res[i][j] = (df[modelA] < df[modelB]).mean() * 100
            if (i != j):
                s += f" & {res[i][j]:.2f}\\%"
            else:
                s += f" & \cellcolor{{gray!25}}"

        s += " \\\\\hline\n"
    s += f"\\end{{tabular}}"
    print(s)

##################################
# Number of best and worst times per model
##################################


def calc_diff(df, models):
    df["increase"] = ((df[models].max(1) - df[models].min(1)
                       ) / df[models].min(1)) * 100

    print(df["increase"].max())
    print(df["increase"].min())
    print(df["increase"].mean())

    df["best_time"] = df[models].min(1)

    print(df[models].sub(df["best_time"], axis=0).sum(1)/(len(models)-1))

##################################
# Plot Relative Increase
##################################


def plot_RI(df, models, names, save=False, save_name=""):
    means_total = []
    df = add_best_worst(df, models)

    for model in models:
        df_inc = (df[model] - df["best_time"]) / df["best_time"] * 100

        means_total.append(df_inc.mean())

    m = np.max(means_total)
    x = np.arange(len(names))
    for i in range(len(means_total)):
        plt.text(x=x[i], y=means_total[i] + m/50, s=f"{means_total[i]:.0f}",
                 fontdict=dict(fontsize=15), ha='center')
    plt.ylim(0, m*1.2)

    plt.bar(names, means_total)
    plt.ylabel("Relative Increase", fontdict=font)
    plt.tight_layout()
    if save:
        plt.savefig(f"../results/images/RI_{save_name}.png")

    plt.title("Mean total", fontdict=font)
    plt.show()

    m = np.max(means_total[1:])
    x = np.arange(len(names))
    for i in range(len(means_total[1:])):
        plt.text(x=x[i], y=means_total[i+1] + m/50, s=f"{means_total[i+1]:.0f}",
                 fontdict=dict(fontsize=12), ha='center')
    plt.ylim(0, m*1.2)

    plt.bar(names[1:], means_total[1:])
    plt.ylabel("Relative Increase no Never", fontdict=font)
    plt.tight_layout()
    if save:
        plt.savefig(f"../results/images/RI_{save_name}_no_N.png")

    plt.title("Mean total", fontdict=font)
    plt.show()


##################################
# Print Relative Increase table
##################################


def increase_table(df, models, names):
    df = add_best_worst(df, models)

    s = f"\\begin{{tabular}}{{|c|"
    for name in names:
        s += f"|c"
    s += f"|}}\\hline\n"

    s += "\diagbox{Increase}{Models}"
    for name in names:
        s += f" & \\textbf{{{name}}}"
    s += " \\\\\hline\\hline\n"
    for model in models:
        df[f"inc_{model}"] = (df[model] - df["best_time"]) / \
            df["best_time"] * 100
        m = df[f"inc_{model}"].mean()
        print(f"model: {model} -> {m}")

    thresholds = [(-1, 0), (0, 10), (10, 50), (50, 100),
                  (100, 200), (200, 500), (500, 1000), (1000, np.inf)]
    counter = 0
    for a, b in thresholds:
        if counter == 0:
            s += f"0\\%"
        elif counter == len(thresholds) - 1:
            s += f"$>$ {a}\\%"
        else:
            s += f"$>$ {a} $\\leq$ {b}\\%"

        for model in models:
            s += f" & {((df[f'inc_{model}'] > a) & (df[f'inc_{model}'] <= b)).sum()}"
        s += f"\\\\\\hline\n"

        counter += 1
    s += f"\\end{{tabular}}"
    print(s)

    print()

    inc_sum = 0
    for model in models:
        print(df[f'inc_{model}'].mean())

        inc_sum += df[f'inc_{model}'].mean()

    inc_sum /= 4

    print(inc_sum)
