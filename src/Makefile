
.PHONY: all
all: libpuscon.a util/libpuscon-util.a

libpuscon.a: puscon.o
	ar rcs $@ $^

util/libpuscon-util.a: FORCE
	${MAKE} -C util

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	${MAKE} -C util clean
	rm -rf *.a *.o

FORCE:
