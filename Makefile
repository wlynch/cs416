CC ?= gcc
CFLAGS ?= -gdwarf-2 -g3 -pedantic -Wall

EXEC = wtc
SRCS = wtc.c
OBJS = ${SRCS:.c=.o}

all: ${EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${EXEC}: ${OBJS}
	${CC} ${LDFLAGS} -o ${EXEC} ${OBJS}

test: test_proc

test_proc: ${EXEC}
	./${EXEC} 1 input.in

test_thr: ${EXEC}
	./${EXEC} 2 input.in

test_btproc: ${EXEC}
	./${EXEC} 3 input.in

test_btthrc: ${EXEC}
	./${EXEC} 4 input.in

clean:
	rm ./${EXEC}
	rm ./*.o

.PHONEY: all clean
