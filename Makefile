
MAKEFLAGS	+= --no-print-directory


CC		?= gcc

export	CC


CPPFLAGS	?= 
CPPFLAGS	+= -I${PWD}/include/

export	CPPFLAGS


.PHONY: all
all: libpuscon puscon-cli

.PHONY: libpuscon
libpuscon: FORCE
	${MAKE} -C src/

.PHONY: puscon-cli
puscon-cli: libpuscon FORCE
	${MAKE} -C src/main/

.PHONY: test
test: libpuscon puscon-cli FORCE
	${MAKE} -C test/

.PHONY: clean
clean:
	${MAKE} -C test/ clean
	${MAKE} -C src/main/ clean
	${MAKE} -C src/ clean

.PHONY: run
run: puscon-cli
	./src/main/puscon /bin/ls

FORCE:
