#!/usr/bin/python

with open("List.txt", "r") as ins:
    array = []
    for line in ins:
        array.append(line.strip())

while array.count("..") > 0:
    array.remove("..")
while array.count(".") > 0:
    array.remove(".")

for element in array:
    if element[0]!=".":
        array.pop(element)
#print(array)

completed = []
for a in array:
    found = 0;
    for c in completed:
        #print(a[:len(a)-2])
        #print(a)
        index=len(a)
        index=-1
        #print(index)
        #print(a[index])
        if(a[:len(a)-2]==c[0]):
            found = 1;
            num=int(a[-1])
            c[num]="y"

    if(found==0):
        listadd = [a[:len(a)-2],"n","n","n","n"]
        #print(a[len(a)-1])
        num=int(a[-1])
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
