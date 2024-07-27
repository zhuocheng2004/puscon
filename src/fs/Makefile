

.PHONY: all
all: libpuscon-fs.o

libpuscon-fs.o: dcache.o filesystems.o inode.o super.o \
		bypass/libpuscon-bypass.o ramfs/libpuscon-ramfs.o
	${LD} -r -o $@ $^

bypass/libpuscon-bypass.o: FORCE
	${MAKE} -C bypass/

ramfs/libpuscon-ramfs.o: FORCE
	${MAKE} -C ramfs/

%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	${MAKE} -C bypass/ clean
	${MAKE} -C ramfs/ clean
	@rm -rf *.a *.o

.PHONY: FORCE
FORCE:
