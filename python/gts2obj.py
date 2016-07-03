#!/usr/bin/python

import sys

gtsfile = sys.argv[1]
ifile = open(gtsfile, "r")
content = ifile.readlines()
ifile.close()

numVEF = content[0].split()
numV = int(numVEF[0])
numE = int(numVEF[1])
numF = int(numVEF[2])

V = content[1:numV+1]
E = content[numV+1:numV+numE+1]
F = content[numV+numE+1:numV+numE+numF+1]


print "# vertices"
for v in V:
        print 'v', v,

print "# faces"
for f in F:
        f0, f1, f2 = f.split()
        f0 = int(f0)-1
        f1 = int(f1)-1
        f2 = int(f2)-1
        e0 = E[f0].split();
        e1 = E[f1].split();
        e2 = E[f2].split();
        s = set([int(e0[0]), int(e0[1]), int(e1[0]), int(e1[1]), int(e2[0]), int(e2[1])])
        print 'f', s.pop(), s.pop(), s.pop()



