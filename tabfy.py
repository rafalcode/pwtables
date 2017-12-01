#!/usr/bin/env python2.7
import sys, regex
from collections import defaultdict

def prtab(OQ, OQSZ, OQSZ1, OT, MA2, mx, mn):
    # first row:
    for i in OT:
        print "%s\t" % i,
    print "%s" % OQ[0] # another one for the 0.0 entry
    for i in xrange(OQSZ):
        print "%s\t" % OQ[i],
        for j in xrange(OQSZ):
            print "%s\t" % MA2[i][j],
        print "%s" % MA2[i][OQSZ]
    print "%s" % OT[0],
    for j in xrange(OQSZ):
        print "%s\t" % MA2[OQSZ][j],
    # Finally we have the last entry to take care of:
    print "%s" % (MA2[OQSZ][OQSZ])

def main():
    """ Generate a table from mash results """
    argquan=len(sys.argv)
    if argquan != 2:
        print "This script requires one argument: a file listing of the mash re sults files"
        sys.exit(2)
    RGX0=regex.compile(r'([0-9a-zA-Z]+)_([0-9a-zA-Z]+)\.') # reflecting the filename
    RGX2=regex.compile(r'(\d+)/1000')
    with open(sys.argv[1]) as x: fl = x.read().splitlines()
    L=[]
    TL=[] # query positions
    mx=0
    mn=100.0
    for i in fl:
        # for each of the files in the filelisting
        m=RGX0.findall(i)
        with open(i) as x: mshr = x.read()
        n=RGX2.findall(mshr)
        pd=(1000-int(n[0]))/10.
        L.append( (m[0][0],m[0][1],pd) ) # append a 3-tuple ... query name, target, distance measure
        TL.append( m[0][1] ) # append the name of the target
        if pd>mx:
            mx=pd
        if pd<mn:
            mn=pd
    # unique-fication of TL is required and I original used the set() function, but actually
    # you lose info with that (such as how many repetitions), so the following way is better:
    TLD = {i:TL.count(i) for i in TL} # generate dictionary of the target list.  Values are the counts.
    # dicionaries aren't terribly amenable to sorting, so I sorted into a list instead.
    OT=[]
    for key, value in sorted(TLD.iteritems(), key=lambda (k,v): (v,k), reverse= False):
    # for key, value in sorted(TLD.iteritems(), key=lambda (k,v): (v,k)):
        OT.append(key)
    # OK, OT now holds the unique list of targets in order of their number of mentions.
    # it can be used on the TLD dictionary
    # the following takes a list and turns its elements into a dict, where the keys are the list elements, and the values are the index
    # position they occupied in the list
    TP={k: v for v, k in enumerate(OT)}
    # next a "collections" idiom for gather all the mentions of each query together
    d = defaultdict(list)
    for k, v1, v2 in L:
        d[k].append( (v1, v2, TP[v1]) ) # so the position (which was ordered) c orresponding to the query is element no.3
    szd={} # dictionary of sizes
    for i in d.items():
        szd[i[0]]=len(i[1])
    OQ=[]
    # this is a way of listing out dict items in order
    for key, value in sorted(szd.iteritems(), key=lambda (k,v): (v,k), reverse=True):
        OQ.append(key)

    # We'd like to have our data in a amtrix however. Here's how:
    OQSZ=len(OQ) ## we'll want one more
    OQSZ1=OQSZ+1 ## we'll want one more
    MA2=[ [0 for i in xrange(OQSZ1)] for j in xrange(OQSZ1)]
    for i in xrange(OQSZ):
        d[OQ[i]].sort(key=lambda tup: tup[2]) # sort the list on the value of the third member of the tuple
        disz=len(d[OQ[i]])
        for j in xrange(disz):
            MA2[i][j]=d[OQ[i]][j][1]
    # mirror upper left to lower right, index conversion .... not for the faint-hearted, needs separate unit-testing
    for i in xrange(1,OQSZ1):
        piv=OQSZ1-1-i
        for j in xrange(piv, OQSZ1):
            MA2[i][j] = MA2[OQSZ1-1-j][OQSZ1-1-i]
    prtab(OQ, OQSZ, OQSZ1, OT, MA2, mx, mn)

if __name__=='__main__':
    main()
