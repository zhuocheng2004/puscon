
%.o: %.c
	${CC} ${CPPFLAGS} -o $@ -c $^
