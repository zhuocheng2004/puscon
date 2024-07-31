
include ${SRC_ROOT}/main_rules.mk

.PHONY: all
all: libpuscon.a

libpuscon.a: puscon.o syscall.o printk.o \
	util/libpuscon-util.o lib/libpuscon-lib.o fs/libpuscon-fs.o
	ar rcs $@ $^

util/libpuscon-util.o: FORCE
	${MAKE} -C util/

lib/libpuscon-lib.o: FORCE
	${MAKE} -C lib/

fs/libpuscon-fs.o: FORCE
	${MAKE} -C fs/

.PHONY: clean
clean:
	${MAKE} -C lib/ clean
	${MAKE} -C fs/ clean
	${MAKE} -C util/ clean
	@rm -rf *.a *.o

.PHONY: FORCE
FORCE:
