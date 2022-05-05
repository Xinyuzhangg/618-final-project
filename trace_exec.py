import subprocess
import logging
import sys
from os import walk
from collections import defaultdict

FORMAT = '%(asctime)-15s - %(levelname)s - %(module)10s:%(lineno)-5d - %(message)s'
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format=FORMAT)
LOG = logging.getLogger(__name__)

if __name__ == '__main__':
    LOG.info("start tracing\n")
    worker = [1, 4, 8, 16, 32, 64]

    d = "100k"

    for hash in ["linear", "flat"]:
        for m in [1, 2, 4, 8]:
            for w in worker:
                np = w + m
                params = ["mpirun"]
                args = f"-np {np} ./hashmap -t parallel -m {m} -h {hash} -d {d}"
                params += args.split()
                subprocess.call(params)