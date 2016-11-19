#!/usr/bin/python

import sys
import os.path


filename=sys.argv[1]
incomplete=0
for x in range(1,4):
    fhold="."+filename+"."+str(x)
    if(os.path.isfile(fhold)!=True):
        incomplete+=1

if(incomplete==0):
    fmain = open(filename,"a+")
    print("Opening file"+filename)
    for y in range(1,5):
        f = open("."+filename+"."+str(y), "r")
        for line in f:
            print("SENDING LINE TO FILE: "+line)
            fmain.write(line)
else:
    print("Incomplete File")


#delete parts
for x in range(1,5):
    fileval="."+filename+"."+str(x)
    os.remove(fileval)
