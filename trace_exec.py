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

    np = 32
    m = 4
    hash = "flat"
    d = "100k"

    for m in range(1, 9):
        params = ["mpirun"]
        args = f"-np {np} ./hashmap -t parallel -m {m} -h {hash} -d {d}"
        params += args.split()
        subprocess.call(params)