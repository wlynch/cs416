CC ?= gcc
CFLAGS ?=  -g -pedantic -Wall
LIBS = -lpthread

EXEC = wtc
SRCS = shared_memory.c wtc_proc_bt.c wtc.c
OBJS = ${SRCS:.c=.o}

all: ${EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${EXEC}: ${OBJS}
	${CC} ${LDFLAGS} -o ${EXEC} ${OBJS} ${LIBS}

test: test_btproc

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
