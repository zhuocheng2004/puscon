

.PHONY: all
all: libpuscon-bypass.o

libpuscon-bypass.o: inode.o
	${LD} -r -o $@ $^

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	@rm -rf *.a *.o
