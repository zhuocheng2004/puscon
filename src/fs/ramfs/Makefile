
include ${SRC_ROOT}/main_rules.mk

.PHONY: all
all: libpuscon-ramfs.o

libpuscon-ramfs.o: inode.o file.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o
