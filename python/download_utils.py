# %%
import wget
from zipfile import ZipFile
import os

from bs4 import BeautifulSoup
import urllib

from tqdm import tqdm

import pandas as pd
import re


def extract_zip(output_folder, network_name):
    try:
        zf = ZipFile(f"{output_folder}/temp.zip")
        zf.extractall(f"{output_folder}/{network_name}")
        zf.close()
        return True
    except:
        print("ERROR: not a valid zip")
        return False


def extract_bz2(output_folder, url):
    os.system(f'tar -xf {output_folder}/temp.zip -C {output_folder}')
    return True


def download_network_Konect(network_name, url, output_folder):
    success = True
    if (network_name in os.listdir(output_folder)):
        print(
            f"WARNING: folder {output_folder}/{network_name} already present")
        return success

    print(f"downloading: {url}")
    wget.download(url, out=f"{output_folder}/temp.zip")

    if url[-3:] == "bz2":
        success = extract_bz2(output_folder, url)
    else:
        success = extract_zip(output_folder, network_name)

    print(f"removing {output_folder}/temp.zip")
    os.remove(f"{output_folder}/temp.zip")

    dr = re.split(".tar", re.split("tsv.", url)[1])[0]
    for f in os.listdir(f"{output_folder}/{dr}"):
        if f[:4] == "out.":
            os.rename(f"{output_folder}/{dr}/{f}",
                      f"{output_folder}/{dr}/edges")

    os.rename(f"{output_folder}/{dr}", f"{output_folder}/{network_name}")
    return success


def download_KONECT_from_series(s, output_folder):
    download_network_Konect(s['Name'], s.download_link, output_folder)


def download_network_NR(network_name, url, output_folder):
    success = True
    if (network_name in os.listdir(output_folder)):
        print(
            f"WARNING: folder {output_folder}/{network_name} already present")
        return success

    print(f"downloading: {url}")
    wget.download(url, out=f"{output_folder}/temp.zip")

    if url[-3:] == "bz2":
        success = extract_bz2(output_folder, url)
    else:
        success = extract_zip(output_folder, network_name)

    print(f"removing {output_folder}/temp.zip")
    os.remove(f"{output_folder}/temp.zip")

    dr = re.split("\.", re.split("/", url)[-1])[-2]
    dr = network_name
    for f in os.listdir(f"{output_folder}/{dr}"):
        print(f"f: {f}")
        if f[-4:] == ".mtx":
            os.rename(f"{output_folder}/{dr}/{f}",
                      f"{output_folder}/{dr}/edges")
            break
        if f[-6:] == ".edges":
            os.rename(f"{output_folder}/{dr}/{f}",
                      f"{output_folder}/{dr}/edges")
            break

    os.rename(f"{output_folder}/{dr}", f"{output_folder}/{network_name}")
    # return success


def download_NR_from_series(s, output_folder):
    download_network_NR(s['Name'], s.download_link, output_folder)


def toKB(value):
    int_val = int(value[:-2].strip())
    scale = value[-2:]
    if scale == "KB":
        return int_val
    if scale == "MB":
        return int_val * 1000
    if scale == "GB":
        return int_val * 1000000

    print("ERROR: no good format found")


def removeK(value):
    if value == "-":
        return 0

    if value[-1] == "K":
        return int(value[:-1].strip()) * 1000
    if value[-1] == "M":
        return int(value[:-1].strip()) * 1000000

    try:
        return int(value[:-1].strip())
    except Exception:
        return 0

# Create a csv with all NR networks and their information


def getNR(output_folder):
    url = "http://networkrepository.com/networks.php"

    u2 = urllib.request.urlopen(url)

    soup = BeautifulSoup(u2, "lxml")

    table = soup.find(id="myTable")

    rows = table.findAll("tr")

    columns = [h.text.strip() for h in table.findAll("th")]

    df = pd.DataFrame(columns=columns)
    for row in tqdm(rows[1:]):

        data = row.findAll("td")

        r = {}

        r[columns[0]] = data[0].find('a').text.strip()
        print(f"name: {r[columns[0]]}")

        r[columns[-1]] = data[-1].find('a')['href']

        for i in range(1, len(data)-1):

            if columns[i] == "Size":
                r[columns[i]] = toKB(data[i].text)
                continue

            if columns[i] in ["|V|", "|E|", "dmax", "davg", "|T|", "Tavg", "Tmax", "K"]:
                r[columns[i]] = removeK(data[i].text)
                continue
            r[columns[i]] = data[i].text

        if len(data) == len(columns)-1:
            r["Size"] = toKB(data[-2].text)

        df = df.append(r, ignore_index=True)

    print("DONE")

    df.to_csv(f"{output_folder}/NR_data.csv", sep=";")


def handleInfo(network_info, r):
    for inf in network_info:
        columns = inf.findAll("td")

        if columns[0].text == "Availability":
            try:
                r["download_link"] = "http://konect.cc/" + \
                    inf.findAll('a')[0]['href'][6:]
            except Exception:
                r["download_link"] = ""

        if columns[0].text == "Network format":
            form = columns[2].text

            if "undirected" in form:
                r["directed"] = False
                continue
            if "directed" in form:
                r["directed"] = True
                continue
            r["directed"] = False

    return r


def handleStats(network_info, r):
    for i in range(len(network_info)):
        inf = network_info[i]
        columns = inf.findAll("td")

        if columns[0].text.strip() == "Maximum degree":
            r["dmax"] = int(columns[2].text.strip().replace(
                " ", "").replace(",", ""))
        if columns[0].text.strip() == "Average degree":
            r["davg"] = float(columns[2].text.strip().replace(
                u"\xa0", u"").replace(",", ""))

    return r


# Create a csv with all Konect networks and their information
def getKONECT(output_folder):
    url = "http://konect.cc/networks/"

    u2 = urllib.request.urlopen(url)

    soup = BeautifulSoup(u2, "lxml")

    table = soup.findAll("table")[0]

    rows = table.findAll("tr")

    columns = ["name", "n", "m", "directed", "dmax", "davg", "download_link"]

    df = pd.DataFrame(columns=columns)

    for row in tqdm(rows[1:]):
        r = {}
        columns = row.findAll("td")

        r["name"] = columns[1].find('a').text
        print(f"name: {r['name']}")
        r["n"] = int(columns[3].text.replace(",", ""))
        r["m"] = int(columns[4].text.replace(",", ""))

        nwurl = url + columns[1].find('a')['href']

        u2 = urllib.request.urlopen(nwurl)

        nwsoup = BeautifulSoup(u2, "lxml")

        r["directed"] = True

        r = handleInfo(nwsoup.findAll("table")[0].findAll("tr"), r)

        r = handleStats(nwsoup.findAll("table")[1].findAll("tr"), r)

        if r["download_link"] == "":
            print("Dataset cannot be downloaded")
            continue

        df = df.append(r, ignore_index=True)

    df.to_csv(f"{output_folder}/KONECT_data.csv", sep=";")
