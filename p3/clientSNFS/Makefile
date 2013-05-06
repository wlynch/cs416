CC = gcc

CFLAGS = -c -g -Wall
CFLAGS +=  $(shell pkg-config fuse --cflags)
CFLAGS +=  $(shell PKG_CONFIG_PATH=${PKG_CONFIG_PATH} pkg-config libprotobuf-c --cflags)
LDFLAGS += $(shell pkg-config fuse --libs)
LDFLAGS += $(shell PKG_CONFIG_PATH=${PKG_CONFIG_PATH} pkg-config libprotobuf-c --libs)

#LDFLAGS += -lpthread

CLIENT_EXEC = clientSNFS

SRCS= rpc.c log.c fuse_ops_helpers.c fuse_ops.c client.c ../protobuf-model/ping.pb-c.c ../protobuf-model/fs.pb-c.c
OBJS = ${SRCS:.c=.o}

all: protobuf ${CLIENT_EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

protobuf:
	cd ../protobuf-model; make

${CLIENT_EXEC}:${OBJS}
	${CC} -o ${CLIENT_EXEC} ${OBJS} ${LDFLAGS}

test: ${CLIENT_EXEC} kill
	# a very opinionated test scenario
	./${CLIENT_EXEC} -mount ./temp -address localhost -port 8080

kill: ${CLIENT_EXEC}
	$(shell pkill clientSNFS)

clean:
	rm -f ${CLIENT_EXEC}
	rm -f ${OBJS}
