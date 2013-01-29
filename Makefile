CC ?= gcc
CFLAGS ?= -pedantic -Wall

EXEC = wtc
SRCS = parser.c wtc.c
OBJS = ${SRCS:.c=.o}

all: ${EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${EXEC}: ${OBJS}
	${CC} ${LDFLAGS} -o ${EXEC} ${OBJS}

test: ${EXEC}
	./${EXEC} input.in

clean:
	rm ./${EXEC}
	rm ./*.o

.PHONEY: all clean
