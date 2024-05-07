#! /bin/python

from csv import reader
from sys import argv
from tabulate import tabulate

def anal(path):
    d = {}

    with open(path) as f:
        r = reader(f)

        for l in r:
            bm, arg = l[0].split('/')
            arg = int(arg)
            if bm not in d:
                d[bm] = {}
            d[bm][arg] = float(l[2])

    out = []

    bm = d["BM_insert"]
    bl = d["BM_insert_bl"]
    for k, v in bm.items():
        row = []
        row.append("BM_insert" + "/" + str(k))
        row.append((v - bl[k]) / k)
        out.append(row)

    bm = d["BM_find"]
    bl = d["BM_find_bl"]
    for k, v in bm.items():
        row = []
        row.append("BM_find" + "/" + str(k))
        row.append((v - bl[k]))
        out.append(row)

    return out

if len(argv) == 2:
    out = anal(argv[1])
    print(tabulate(out, headers=["BM Name/Arg", "Time"]))

elif len(argv) == 3:
    a = anal(argv[1])
    b = anal(argv[2])

    for i in range(len(a)):
        a[i][1] = a[i][1] - b[i][1]

    print(tabulate(a, headers=["BM Name/Arg", "Time"]))

else:
    print("not implemented")
