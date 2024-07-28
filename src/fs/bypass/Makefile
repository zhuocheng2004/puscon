
include ${SRC_ROOT}/main_rules.mk

.PHONY: all
all: libpuscon-bypass.o

libpuscon-bypass.o: inode.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o
