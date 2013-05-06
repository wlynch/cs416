DIRS = protobuf-model clientSNFS serverSNFS

all:
	for d in $(DIRS); do (cd $$d; make ); done

clean:
	for d in $(DIRS); do (cd $$d; make clean ); done
