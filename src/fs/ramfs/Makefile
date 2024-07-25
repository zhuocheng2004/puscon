
.PHONY: all
all: libpuscon-ramfs.o

libpuscon-ramfs.o: inode.o
	${LD} -r -o $@ $^

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	@rm -rf *.a *.o
