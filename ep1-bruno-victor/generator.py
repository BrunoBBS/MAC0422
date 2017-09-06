#!/usr/bin/python3
import sys
import random as rd

def generate_proc(pid):
    t0 = rd.randint(0, 1200)
    dt = rd.randint(1, 300)
    dl = t0 + dt + rd.randint(100, 300)
    return (t0 / 10, dt / 10, dl / 10, 'process_' + str(pid))

if len(sys.argv) < 3:
    print ("Usage: python generate.py <filename> <number of processes>")
    sys.exit(0)

f = open(sys.argv[1], 'w')

proc_cnt = 0

try:
    proc_cnt = int(sys.argv[2])
except ValueError:
    print (sys.argv[2] + ' is not a valid integer')
    sys.exit(-1)

if proc_cnt < 0:
    print ("Can't generate " + str(proc_cnt) + " processes!")
    sys.exit(-1)

for i in range(proc_cnt):
    process = generate_proc(i + 1)
    line = ' '.join([str(item) for item in process])
    print (line)
    f.write(line + '\n')

f.close()

