
.PHONY: all
all: libpuscon-util.a

libpuscon-util.a: idmap.o
	ar rcs $@ $^

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	rm -rf *.a *.o
