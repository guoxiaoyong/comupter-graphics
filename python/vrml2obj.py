#!/usr/bin/python

import sys
vrmlfile = sys.argv[1]
ifile = open(vrmlfile, "r")
content = ifile.readlines()
ifile.close()

for line in content:
        print "v ", line,

vrmlfile = sys.argv[2]
ifile = open(vrmlfile, "r")
content = ifile.readlines()
ifile.close()

for line in content:
        v1, v2, v3, v4 = line.split()
        print "f ", int(v1)+1, int(v2)+1, int(v3)+1


