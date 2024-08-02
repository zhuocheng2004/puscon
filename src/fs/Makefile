
.PHONY: all
all: libpuscon-fs.o

include ${SRC_ROOT}/main_rules.mk

OBJS	:= dcache.o filesystems.o fs_context.o inode.o \
		namei.o namespace.o  super.o \

SUBDIRS	:= bypass ramfs

libpuscon-fs.o: ${OBJS} $(call targets_subdir,${SUBDIRS})
	${LD} -r -o $@ $^

.PHONY: clean
clean: $(call targets_subdir_clean,${SUBDIRS})
	@rm -rf *.a *.o
