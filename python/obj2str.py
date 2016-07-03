#!/usr/bin/python
import sys

objfile = sys.argv[1]
ifile = open(objfile, "r")
content = ifile.readlines()
ifile.close()

print "char objstring[] = "
for line in content:
        print '"'+line.strip()+'\\n"'

print ";"

