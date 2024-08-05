
%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $^

%.o: %.s
	${CC} ${CFLAGS} -o $@ -c $^


targets_subdir = $(foreach x,$(1),$x/libpuscon-$x.o)

targets_subdir_clean = $(foreach x,$(1),subdir_$x_clean)

libpuscon-%.o: FORCE
	${MAKE} -C $(dir $@) $(notdir $@)

subdir_%_clean: FORCE
	${MAKE} -C $* clean


.PHONY: FORCE
FORCE:
