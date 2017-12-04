CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lcairo -lm
EXECUTABLES=rcol t2pwma t2pwma_d hmap rcol0 t2pwma_ t2pwma_d_

rcol: rcol.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

rcol0: rcol0.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# converts a pairwise matrix into a heatmap
t2pwma: t2pwma.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
t2pwma_d: t2pwma.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# a particularly unobvious thing is occuring here
# I have hard coded 999 as the "no value" number.
# I wanted it to skip drawing rects there ... but it does
# anyhow .. this is quite a hard coded version
t2pwma_: t2pwma_.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
t2pwma_d_: t2pwma_.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# heat map pairwise matrix particularly unobvious thing is occuring here
hmap: hmap.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
