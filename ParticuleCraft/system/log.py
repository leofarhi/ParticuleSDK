from .working_dirs import particulecraft_path
import logging
import sys, os

logger = logging.getLogger("ParticuleEngine")
logger.setLevel(logging.DEBUG)

# Format clair
formatter = logging.Formatter(
    "[%(levelname)s] %(asctime)s | %(message)s",
    datefmt="%H:%M:%S"
)

if not logger.hasHandlers():
    # Console handler
    ch = logging.StreamHandler(sys.stdout)
    ch.setLevel(logging.DEBUG)
    ch.setFormatter(formatter)
    logger.addHandler(ch)

    #create __pycache__ directory if it doesn't exist
    if not os.path.exists(os.path.join(particulecraft_path, "__pycache__")):
        os.makedirs(os.path.join(particulecraft_path, "__pycache__"))

    fh = logging.FileHandler( os.path.join(particulecraft_path,"__pycache__", "log.txt"), mode='w', encoding='utf-8')
    fh.setLevel(logging.DEBUG)
    fh.setFormatter(formatter)
    logger.addHandler(fh)