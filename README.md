# pwtables
r2 taken from tabfy.py
although very similar.

r2\_.py the tailored and working version of this.
Use this on the allreps output
which was obtained via find -name snprep -exec cat {} >> allreports.txt \;
more or less
output to tsv extension for clarity.

and then run t2pwma\_ to get heatmap.

all a bit creaky hopefully I Can clean it up when I'm not running another three projects att the same time.

# realisations (due to old code being used
r2.py and derivative were aimed at making a visually pleasing
matrix with zeros where target == query.
