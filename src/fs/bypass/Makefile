
.PHONY: all
all: libpuscon-bypass.o

include ${SRC_ROOT}/main_rules.mk

libpuscon-bypass.o: inode.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o
