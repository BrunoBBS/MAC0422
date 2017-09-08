#!/usr/bin/python3
import sys
import random as rd

def generate_proc(pid):
    t0 = rd.randint(0, 1200)
    dt = rd.randint(1, 300)
    dl = t0 + dt + rd.randint(100, 300)
    return (t0 / 10, dt / 10, dl / 10, 'process_' + str(pid))

if len(sys.argv) < 3:
    print ("Usage: python stats.py <input> <out> [verbose]")
    sys.exit(0)

inp = open(sys.argv[1], 'r')
out = open(sys.argv[2], 'r')

class Process():
    def __init__(self, t0, dt, dl):
        self.t0 = t0
        self.dt = dt
        self.dl = dl
        self.late = 0

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
    if tf > proc_obj.dl:
        proc_obj.late = tf - proc_obj.dl

    if len(sys.argv) > 3:
        print ("{}Process {} run from {:.1f} to {:.1f} when it should have ended at {:.1f}\033[0m".
                format('\033[32m' if tf <= proc_obj.dl else '\033[31m', name,
                    proc_obj.t0, tf, proc_obj.dl))
        if tf > proc_obj.dl:
            print ("Process {} was late by {:.1f}".
                    format(name, proc_obj.late))
        print ("Process {} had a delta of {:.1f}".
                format(name, tf - proc_obj.t0))

print ("There were {:d} processes that missed the deadline".
        format(len([0 for proc in processes if processes[proc].late > 0])))
print ("Processes were late by a total of {:.1f} seconds".
        format(sum([processes[name].late for name in processes])))

print ("The process that was late the most was late by {:.1f} seconds".
        format(max([processes[name].late for name in processes])))
if len([0 for proc in processes if processes[proc].late > 0]) != 0:
    print ("Processes were late by average an {:.1f} seconds average".
            format(sum([processes[name].late for name in processes]) /
                len([0 for proc in processes if processes[proc].late > 0])))

if len([0 for proc in processes if processes[proc].late > 0]) != 0:
    print ("How much processes were late has a variance of {:f}".
            format(sum([processes[name].late**2 for name in processes]) /
                len([0 for proc in processes if processes[proc].late > 0])))
inp.close()
out.close()

