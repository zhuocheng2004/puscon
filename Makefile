
MAKEFLAGS	+= --no-print-directory

ARCH		?= x86_64

export ARCH


CC		?= cc
LD		?= ld

export	CC LD


CFLAGS	+= -I${PWD}/include/ -I${PWD}/include/arch/${ARCH}/ -Wall -Wno-unused

ifdef DEBUG
CFLAGS	+= -g
endif

ifdef ANDROID
CFLAGS	+= -D__android__
endif

export	CFLAGS


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
