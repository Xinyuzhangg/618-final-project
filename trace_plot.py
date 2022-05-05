import subprocess
import logging
import sys
from os import walk
from collections import defaultdict
import matplotlib as mpl
import matplotlib.pyplot as plt
from statistics import mean

FORMAT = '%(asctime)-15s - %(levelname)s - %(module)10s:%(lineno)-5d - %(message)s'
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format=FORMAT)
LOG = logging.getLogger(__name__)

def read_file(file_name):
    f = open(file_name, "r")
    result = {}
    idx = -1
    for l in f:
        if "master" in l:
            idx = int(l.split()[1])

        else:
            if idx != -1:
                result[idx] = float(l.split()[2])
                idx = -1

    return mean(result.values())



def plot(w_num):
    data = defaultdict(lambda : [[], []])
    fs = []
    dir_path = "result/100k/"
    for (dirpath, dirnames, filenames) in walk(dir_path):
        fs.extend(filenames)

    for f in fs:
        cats = f.split("&")
        hash_type = cats[0]
        m = int(cats[1].split("m")[0])
        w = int(cats[2].split("w")[0])

        if w == w_num:
            data[hash_type][0].append(m)
            val = read_file(dir_path + f)
            data[hash_type][1].append(val)



    plt.xlabel('Number of masters')
    plt.ylabel('Calculation time')

    plot_line = ['ro-', 'go-']
    for idx, (k, v) in enumerate(data.items()):
        v = list(zip(*sorted(zip(v[0], v[1]))))
        plt.plot(v[0], v[1], plot_line[idx], label=k)

    leg = plt.legend(loc='best', ncol=2, mode="expand", shadow=True, fancybox=True)
    leg.get_frame().set_alpha(0.3)

    plt.savefig(f'plot/result_w{w_num}.png')
    plt.figure().clear()
    plt.close()
    plt.cla()
    plt.clf()


def plot2(m_num):
    data = defaultdict(lambda : [[], []])
    fs = []
    dir_path = "result/100k/"
    for (dirpath, dirnames, filenames) in walk(dir_path):
        fs.extend(filenames)
    for f in fs:
        cats = f.split("&")
        hash_type = cats[0]
        m = int(cats[1].split("m")[0])
        w = int(cats[2].split("w")[0])
        if m == m_num:
            data[hash_type][0].append(w)
            val = read_file(dir_path + f)
            data[hash_type][1].append(val)


    plt.xlabel('Number of workers')
    plt.ylabel('Speed up')

    plot_line = ['ro-', 'go-']
    for idx, (k, v) in enumerate(data.items()):
        v = list(zip(*sorted(zip(v[0], v[1]))))
        v[1] = [(v[1][0] / val) for val in v[1]]
        plt.plot(v[0], v[1], plot_line[idx], label=k)

    leg = plt.legend(loc='best', ncol=2, mode="expand", shadow=True, fancybox=True)
    leg.get_frame().set_alpha(0.3)

    plt.savefig(f'plot/result_m{m_num}.png')
    plt.figure().clear()
    plt.close()
    plt.cla()
    plt.clf()


if __name__ == '__main__':
    LOG.info("start tracing\n")
    w_num = [1]
    m_num = [1, 2, 4, 8]
    for w in w_num:
        plot(w)
    for m in m_num:
        plot2(m)
