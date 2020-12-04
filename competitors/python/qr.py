import sys
import numpy as np
import pandas as pd
from  argparse import ArgumentParser


def dump_qr(dump_file_path, r):
    np.savetxt(dump_file_path, np.asarray(r), delimiter=',')

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("-d", "--data_path", dest="data_path", required=True)
    parser.add_argument("-D", "--dump_file", dest="dump_file", required=False)

    np.set_printoptions(threshold=sys.maxsize, precision=20)
    pd.set_option('display.max_columns', 500)
    args = parser.parse_args()
    data_path = args.data_path
    dump_file = args.dump_file

    data = pd.read_csv(data_path, delimiter=",", header=None)
    r = np.linalg.qr(data, mode='r')
    print(r)
    if dump_file is not None:
        dump_qr(dump_file, r)