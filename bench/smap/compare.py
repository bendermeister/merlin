#! /bin/python
from csv import reader
from tabulate import tabulate
from sys import argv

d = {}

with open(argv[1]) as f:
    r = reader(f)
    for l in r:
        name, a0, a1 = l[0].split('/')
        if name not in d:
            d[name] = {}
        d[name][a0 + "/" + a1] = float(l[3])

out = []

for k, v in d["BM_insert_fixlen"].items():
    bl = d["BM_insert_fixlen_bl"]
    num, _ = k.split("/")
    num = 2**int(num)
    row = []
    row.append("BM_insert_fixlen" + "/" + k)
    row.append(round((v - bl[k]) / num))
    out.append(row)

for k, v in d["BM_insert_randlen"].items():
    bl = d["BM_insert_randlen_bl"]
    num, _ = k.split("/")
    num = 2**int(num)
    row = []
    row.append("BM_insert_randlen" + "/" + k)
    row.append(round((v - bl[k]) / num))
    out.append(row)

for k, v in d["BM_find_fixlen"].items():
    bl = d["BM_find_fixlen_bl"]
    row = []
    row.append("BM_find_fixlen/" + k)
    row.append(round(v - bl[k]))
    out.append(row)

for k, v in d["BM_find_randlen"].items():
    bl = d["BM_find_randlen_bl"]
    row = []
    row.append("BM_find_randlen/" + k)
    row.append(round(v - bl[k]))
    out.append(row)

print(tabulate(out, headers=["BM Name/Arg/KeyLen", "Time"]))
