
MAKEFLAGS	+= --no-print-directory


CC		?= gcc
LD		?= ld

export	CC LD


EXTRACPPFLAGS	?= 
CPPFLAGS	+= -I${PWD}/include/ ${EXTRACPPFLAGS}

export	CPPFLAGS


SRC_ROOT	= ${PWD}

export SRC_ROOT


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

.PHONY: run
run: puscon-cli kernel
	./src/main/puscon --ansi-color --level 7 ./src/kernel/kernel ./test/test_load_static.elf

FORCE:
