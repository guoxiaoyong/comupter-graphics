#!/usr/bin/python
# Filename: mymodule_demo.py
import sys
import urllib

filehandle = urllib.urlopen('http://www.baidu.com')
print filehandle

while (1):
        r = filehandle.read(1024);
        print r;
        if r == '':
                break






