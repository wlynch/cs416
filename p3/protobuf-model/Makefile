CC= protoc-c

MODELS= ping.proto
IMPLS= ${MODELS:.proto=.pb-c.c}
HEADERS = ${MODELS:.proto=.pb-c.h}

all:
	protoc-c --c_out . ${MODELS}

clean:
	rm ${IMPLS} ${HEADERS}
