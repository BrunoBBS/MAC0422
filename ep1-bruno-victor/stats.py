#!/usr/bin/python3
import sys
import random as rd

def generate_proc(pid):
    t0 = rd.randint(0, 1200)
    dt = rd.randint(1, 300)
    dl = t0 + dt + rd.randint(100, 300)
    return (t0 / 10, dt / 10, dl / 10, 'process_' + str(pid))

if len(sys.argv) < 3:
    print ("Usage: python stats.py <input> <out>")
    sys.exit(0)

inp = open(sys.argv[1], 'r')
out = open(sys.argv[2], 'r')

class Process():
    def __init__(self, t0, dt, dl):
        self.t0 = t0
        self.dt = dt
        self.dl = dl

processes = {}

for line in inp:
    process = line.split(' ')
    if len(process) != 4:
        print ("line '%s' is invalid!" % line[:-1])
        sys.exit(-1)

    name = process[3][:-1]
    t0 = dt = dl = 0
    try:
        t0 = float(process[0])
    except ValueError:
        print ("line '%s' is invalid!" % line[:-1])
        print ("t0 '%s' is not a number!" % process[0])
        sys.exit(-1)

    try:
        dt = float(process[1])
    except ValueError:
        print ("line '%s' is invalid!" % line[:-1])
        print ("dt '%s' is not a number!" % process[1])
        sys.exit(-1)

    try:
        dl = float(process[2])
    except ValueError:
        print ("line '%s' is invalid!" % line[:-1])
        print ("dl '%s' is not a number!" % process[2])
        sys.exit(-1)

    processes[name] = Process(t0, dt, dl)

for line in out:
    process = line.split(' ')
    if len(process) != 3:
        print ("line '%s' is invalid!" % line[:-1])
        sys.exit(-1)

    name = process[0]
    tf = tr = 0
    try:
        tf = float(process[1])
    except ValueError:
        print ("line '%s' is invalid!" % line[:-1])
        print ("tf '%s' is not a number!" % process[1])
        sys.exit(-1)

    try:
        tr = float(process[2])
    except ValueError:
        print ("line '%s' is invalid!" % line[:-1])
        print ("tr '%s' is not a number!" % process[2])
        sys.exit(-1)

    proc_obj = processes[name]
    print ("{}Process {} run from {:.1f} to {:.1f} when it should have ended at {:.1f}\033[0m".
            format('\033[32m' if tf <= proc_obj.dl else '\033[31m', name,
                proc_obj.t0, tf, proc_obj.dl))
    print ("Process {} had a delta of {:.1f}\033[0m".
            format(name, tf - proc_obj.t0))

inp.close()
out.close()

