
MAKEFLAGS	+= --no-print-directory


CC		?= gcc
LD		?= ld

export	CC LD


CPPFLAGS	?= 
CPPFLAGS	+= -I${PWD}/include/

export	CPPFLAGS


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
test: libpuscon puscon-cli FORCE
	${MAKE} -C test/

.PHONY: clean
clean:
	${MAKE} -C test/ clean
	${MAKE} -C src/kernel/ clean
	${MAKE} -C src/main/ clean
	${MAKE} -C src/ clean

.PHONY: run
run: puscon-cli kernel
	./src/main/puscon ./src/kernel/kernel /bin/ls

FORCE:
