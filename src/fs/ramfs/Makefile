
.PHONY: all
all: libpuscon-ramfs.o

include ${SRC_ROOT}/main_rules.mk

libpuscon-ramfs.o: inode.o file.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o
