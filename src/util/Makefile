
include ${SRC_ROOT}/main_rules.mk

.PHONY: all
all: libpuscon-util.o

libpuscon-util.o: idmap.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o