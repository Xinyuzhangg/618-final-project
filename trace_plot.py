import subprocess
import logging
import sys
from os import walk
from collections import defaultdict
import matplotlib as mpl
import matplotlib.pyplot as plt

FORMAT = '%(asctime)-15s - %(levelname)s - %(module)10s:%(lineno)-5d - %(message)s'
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format=FORMAT)
LOG = logging.getLogger(__name__)


def main():
    # f = open("100k.txt", "r")
    print("yes")


def read_file(file_name):
    f = open(file_name, "r")
    result = {}
    idx = -1
    for l in f:
        if "master" in l:
            idx = int(l.split()[1])
        else:
            if idx != -1:
                result[idx] = float(l.split()[1])
                idx = -1

    return max(result.values())

def plot():
    data = defaultdict(lambda : [[], []])
    fs = []
    for (dirpath, dirnames, filenames) in walk("result/100k"):
        fs.extend(filenames)
    for f in fs:
        cats = f.split("&")
        hash_type = cats[0]
        m = int(cat[1].split("m")[0])
        n = int(cat[2].split("n")[0])
        data[hash_type][0].append(m)
        val = read_file(f)
        data[hash_type][1].append(val)

    plt.xlabel('Number of masters')
    plt.ylabel('Calculation time')
    for k, v in dict:
        plt.plot(v[0], v[1], 'ro', label=k)
    plt.savefig('result.png')


if __name__ == '__main__':
    LOG.info("start tracing\n")

    np = 8
    m = 4
    hash = "flat"
    d = "100k"

    for m in [2, 4, 8, 16]:
        params = ["mpirun"]
        args = f"-np {np} ./hashmap -t parallel -m {m} -h {hash} -d {d}"
        params += args.split()
        subprocess.call(params)