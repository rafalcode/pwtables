#!/usr/bin/env python2.7
import sys, regex
from collections import defaultdict

def prtabu(OQ, OQSZ, OQSZ1, OT, MA2, mx, mn):
    # first row:
    for i in OT:
        print "%s\t" % i,
    print "%s" % OQ[0] # to which is appended a extra lable .. being hgte first query which will be 0.0 entries
    for i in xrange(OQSZ):
        print "%s\t" % OQ[i],
        for j in xrange(i):
            print "999\t",
        for j in xrange(OQSZ-i):
            print "%s\t" % MA2[i][j],
        print "%s" % MA2[i][OQSZ]
    print "%s" % OT[0],
    for j in xrange(OQSZ-1):
        print "999\t",
    for j in xrange(OQSZ-OQSZ+1):
        print "%s\t" % MA2[OQSZ][j],
    # Finally we have the last entry to take care of:
    print "%s" % (MA2[OQSZ][OQSZ])

# the original
def prtab0(OQ, OQSZ, OQSZ1, OT, MA2, mx, mn):
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

    # regex
    # ./comp_435_439/recodemerge.tab_woh.snprepNum F2 snps = 112238
    RGX0=regex.compile(r'\.\/comp_(\d+)_(\d+)[^=]+= (\d+)$')
    # File comp_456_1038/recodemerge.tab_woh.fa: numseq=3, F2 uniform seq size at 22735. SNP Count: 156
    RGX2=regex.compile(r'File comp_(\d+)_(\d+)[^=]+=[^:]+: (\d+)$')
    with open(sys.argv[1]) as x: fl = x.read().splitlines()
    L=[]
    TL=[] # query positions: no! target list
    mx=0
    mn=10000000
    for i in fl:
        m=RGX2.match(i)
        pd= int(m.groups(1)[2]) # come out as strings of course ... pd, pair distance ... this we can convert to integer.
        L.append( (m.groups(1)[0], m.groups(1)[1], pd) )
        TL.append(m.groups(1)[1])
        if pd>mx:
            mx=pd
        if pd<mn:
            mn=pd

    TLD = {i:TL.count(i) for i in TL} # generate dictionary of the target list.  Values are the counts.
    OT=[] # ordered targets
    for key, value in sorted(TLD.iteritems(), key=lambda (k,v): (v,k), reverse= False):
    # for key, value in sorted(TLD.iteritems(), key=lambda (k,v): (v,k)):
        OT.append(key)
    # so OT is a list of the targets in increasing number of counts
    TP={k: v for v, k in enumerate(OT)} # should giv target names and value is index within OT, but TP no ordered.
    # next a "collections" idiom for gather all the mentions of each query together
    d = defaultdict(list)
    for k, v1, v2 in L:
        d[k].append( (v1, v2, TP[v1]) ) # so the position (which was ordered) c orresponding to the query is element no.3
    print "len d is %i" % len(d)
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
    # for i in xrange(1,OQSZ1):
    #     piv=OQSZ1-1-i
    #     for j in xrange(piv, OQSZ1):
    #         MA2[i][j] = MA2[OQSZ1-1-j][OQSZ1-1-i]
    prtabu(OQ, OQSZ, OQSZ1, OT, MA2, mx, mn)

if __name__=='__main__':
    main()
