#!/usr/bin/env python
from argparse import ArgumentParser

import SIL_SIL_evaluation_module as kpi


def arg_parser():
    parser = ArgumentParser()

    # Add more options if you like
    parser.add_argument(
        "-i",
        "--input_folder",
        dest="input_folder",
        help="path to the folder of exported csv files",
        metavar="FILE",
        required=True,
    )
    parser.add_argument(
        "-r",
        "--reference_file",
        dest="reference_file",
        required=True,
        help="given reference file to compare other csv files with",
    )

    parser.add_argument(
        "-t",
        "--number_of_threads",
        dest="num_thrds",
        help="number of worker threads for more speed of reading and evaluation",
        default=3,
    )

    args = parser.parse_args()
    return {"in_folder": args.input_folder, "num_thrd": args.num_thrds, "ref_file": args.reference_file}


if __name__ == "__main__":
    args = arg_parser()

    csv_parser = kpi.csv_parser(args["in_folder"], args["num_thrd"], args["ref_file"])
    csv_parser.start_kpi()
