import subprocess
import logging
import sys

FORMAT = '%(asctime)-15s - %(levelname)s - %(module)10s:%(lineno)-5d - %(message)s'
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format=FORMAT)
LOG = logging.getLogger(__name__)


if __name__ == "__main__":
    LOG.info("start tracing\n")
    subprocess.call(["ls", "-a"])
    print("trace")