
SO_1	:= libpuscon.so.${PUSCON_VERSION_MAJOR}

.PHONY: all
all: puscon

include ${SRC_ROOT}/main_rules.mk

ifdef SHARED
puscon: main.o ${SO_1}
	${CC} -o $@ main.o ${SO_1}
else
puscon: main.o ../libpuscon.a
	${CC} -o $@ $^
endif

${SO_1}: ../${SO_1}
	@rm -f $@
	ln -s $^ $@

.PHONY: clean
clean:
	@rm -rf puscon *.o *.so*
