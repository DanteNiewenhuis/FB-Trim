# %%
import os
import shutil
import argparse

# %%

parser = argparse.ArgumentParser()
parser.add_argument('--base', type=str,
                    help='an integer for the accumulator')
parser.add_argument('--folder', type=str,
                    help='an integer for the accumulator')

args = parser.parse_args()

# %%

base = args.base
folder = args.folder

files = os.listdir(f"{folder}")
base_files = []

for file in files:
    if base in file and base != file:
        base_files.append(file)

# %%

to_remove = []
for file in base_files:
    if file in to_remove:
        continue

    for other_file in base_files:
        if file == other_file:
            continue
        if other_file in to_remove:
            continue
        
        r = os.system(f'diff "{folder}/{file}" "{folder}/{other_file}"')
        if r == 0:
            to_remove.append(other_file)

# %%

for file in to_remove:
    shutil.rmtree(f"{folder}/{file}")