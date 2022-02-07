PREFIX			?= /usr/local
BSD_INSTALL_PROGRAM	?= install

st4b: st4b.o apm.o time.o iface.o
	${CC} ${CFLAGS} -o $@ st4b.o apm.o time.o iface.o

all: st4b

install: all
	${BSD_INSTALL_PROGRAM} st4b ${PREFIX}/bin/st4b

clean:
	rm -f *.core *.o st4b
