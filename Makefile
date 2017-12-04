CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lcairo -lm
EXECUTABLES=rcol t2pwma t2pwma_d hmap rcol0

rcol: rcol.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

rcol0: rcol0.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

t2pwma: t2pwma.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
t2pwma_d: t2pwma.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# heat map pairwise matrix
hmap: hmap.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
