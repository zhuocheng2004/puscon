
include ${SRC_ROOT}/main_rules.mk

.PHONY: all
all: puscon

puscon: main.o ../libpuscon.a
	${CC} -o $@ $^

.PHONY: clean
clean:
	@rm -rf puscon *.o
