
MAKEFLAGS	+= --no-print-directory

include config.mk

.PHONY: all
all: libpuscon puscon-cli kernel

.PHONY: libpuscon
libpuscon: FORCE
	${MAKE} -C src/

.PHONY: puscon-cli
puscon-cli: libpuscon FORCE
	${MAKE} -C src/main/

.PHONY: kernel
kernel: FORCE
	${MAKE} -C src/kernel/

.PHONY: test
test: libpuscon puscon-cli kernel FORCE
	${MAKE} -C test/

.PHONY: clean
clean:
	${MAKE} -C test/ clean
	${MAKE} -C src/kernel/ clean
	${MAKE} -C src/main/ clean
	${MAKE} -C src/ clean
	rm -rf include/gen/

.PHONY: run
run: puscon-cli kernel
	LD_LIBRARY_PATH=${SRC_ROOT}/src/:${LD_LIBRARY_PATH} ./src/main/puscon --ansi-color --log-level 7 ./src/kernel/kernel ./test/test_load_static.elf

.PHONY: FORCE
FORCE:
