#!/usr/bin/python

import sys


def EdgeProcessed(shape, Edges):
        E1 = [ (shape[0], shape[1]), (shape[1], shape[2]), (shape[2], shape[0]) ] 
        E2 = [ (shape[2], shape[1]), (shape[1], shape[0]), (shape[0], shape[2]) ] 
        
        for e in E1:
                if (e in Edges):
                        return 2
        
        for e in E2:
                if (e in Edges):
                        return 1 

        return 0


       
def ReverseVertices(shape):
        return (shape[2], shape[1], shape[0])


# read obj file
objfile = sys.argv[1] 
ifile   = open(objfile, "r")
content = ifile.readlines()
ifile.close()

# define Vertices and Triangle List
VerticesLines  = []
Triangles = []


# separate vertices and faces
# keep vertices lines as string in list VerticesLines 
for line in content:
    if line[0] == "f":
            vh, v0, v1, v2 = line.split()
            Triangles.append( (int(v0), int(v1), int(v2)) )
    elif line[0] == "v":
            VerticesLines.append(line)


#print len(VerticesLines)
#print len(Triangles)


# start sorting of the faces
Edges = set([])

# initialize Edge set and Vertex set
Tidx = 0
shape = Triangles[Tidx]

if len(sys.argv) == 3:
        shape = ReverseVertices(shape)
        print "# reversed"

Edges.add( (shape[0], shape[1]) )
Edges.add( (shape[1], shape[2]) )
Edges.add( (shape[2], shape[0]) )
                      
OTriangles = []
OTriangles.append( (shape[0], shape[1], shape[2]) )
flag = [0]*len(Triangles)
flag[Tidx] = 1

while len(OTriangles) < len(Triangles):
        print len(OTriangles)
        for n in range( len(Triangles) ):
                if flag[n] == 0:
                        shape = Triangles[n]
                        # info == 0, edge not found
                        # info == 1, edge found, correct direction 
                        # info == 2, edge found, wrong direction 
                        info = EdgeProcessed(shape, Edges) 
                        if info > 0: 
                                if info == 2: 
                                        shape = ReverseVertices(shape) 
                                Edges.add( (shape[0], shape[1]) )
                                Edges.add( (shape[1], shape[2]) )
                                Edges.add( (shape[2], shape[0]) )
                                OTriangles.append(shape) 
                                flag[n] = 1


for n in flag:
        if flag[n] == 0:
                print "#error: ", n


if len(sys.argv) < 4:
        print "# vertices" 
        for line in VerticesLines: 
                print line, 
                
        print "# faces" 
        for face in OTriangles: 
                print "f ", face[0], face[1], face[2]


