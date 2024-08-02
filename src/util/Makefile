
.PHONY: all
all: libpuscon-util.o

include ${SRC_ROOT}/main_rules.mk

libpuscon-util.o: idmap.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o