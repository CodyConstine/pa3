#!/usr/bin/python

with open("List.txt", "r") as ins:
    array = []
    for line in ins:
        array.append(line.strip())

while array.count("..") > 0:
    array.remove("..")
while array.count(".") > 0:
    array.remove(".")
#print(array)

completed = []
for a in array:
    found = 0;
    for c in completed:
#        print(a[:len(a)-2])
#        print(c[:len(c)])
        if(a[:len(a)-2]==c[0]):
            found = 1;
            num=int(a[len(a)-1:len(a)])
            c[num]="y"

    if(found==0):
        listadd = [a[:len(a)-2],"n","n","n","n"]
        num=int(a[len(a)-1])
        listadd[num]="y"
        completed.append(listadd)

#    print(completed)

fi = open("listout.txt", "w")
for element in completed:
    if(element[1]=="y" and element[2]=="y" and element[3]=="y" and element[4]=="y"):
        el=element[0]
        fi.write(el[1:len(el)]+"\n")
    else:
        el=element[0]
        fi.write(el[1:len(el)]+" [incomplete]\n")
