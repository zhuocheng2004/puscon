
.PHONY: all
all: libpuscon-lib.o

include ${SRC_ROOT}/main_rules.mk

libpuscon-lib.o: usercopy.o
	${LD} -r -o $@ $^

.PHONY: clean
clean:
	@rm -rf *.a *.o